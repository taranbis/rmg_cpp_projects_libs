#include "facial_keypoints.hpp"
#include "facial_keypoints_model.hpp"

#include <boost/program_options.hpp>

#include <torch/script.h>

#include <random>

namespace po = boost::program_options;

/******************************************************************************************************************
 **                                Facial Keypoints Detection
 *
 * This project will be all about defining and training a convolutional neural network to perform facial keypoint
 * detection, and using computer vision techniques to transform images of faces.
 *****************************************************************************************************************/

template <typename DataLoader>
void train(size_t numberOfEpochs, KeypointsModel& model, DataLoader& dataLoader,
           torch::optim::Optimizer& optimizer, size_t datasetSize, size_t logInterval)
{
    for (size_t epoch = 1; epoch <= numberOfEpochs; ++epoch) {
        size_t batchIdx = 0;
        for (const torch::data::Example<>& batch : *dataLoader) {
            // auto data = batch.data.to(device), targets = batch.target.to(device);
            optimizer.zero_grad();
            auto output = model.forward(batch.data);
            auto loss = torch::mse_loss(output, batch.target);
            AT_ASSERT(!std::isnan(loss.template item<float>()));
            loss.backward();
            optimizer.step();

            if (batchIdx++ % logInterval == 0) {
                std::printf("\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f\n", epoch, batchIdx * batch.data.size(0),
                            datasetSize, loss.template item<float>());
            }
        }
    }
}

template <typename DataLoader>
auto test(KeypointsModel& model, DataLoader& dataLoader, size_t datasetSize)
{
    auto testLoss = 0.;
    for (const torch::data::Example<>& batch : *dataLoader) {
        // auto data = batch.data.to(device), targets = batch.target.to(device);
        auto output = model.forward(batch.data);
        auto loss = torch::mse_loss(output, batch.target); //
        AT_ASSERT(!std::isnan(loss.template item<float>()));
        testLoss += loss.template item<float>();
    }

    testLoss /= datasetSize;
    std::printf("Test set: Average loss: %.4f\n", testLoss);

    return testLoss;
}

int main(int argc, char** argv)
{
    po::options_description desc("Program options");

    desc.add_options()("help, h", "print info")("tune", "parameter tuning")("train, t", "train model")(
                "results, r", "show results")("display, d", "display initial images")(
                "learning-rate, lr", "learning rate")("load", po::value<std::string>(), "load model");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    const char* DataRoot = "./facial_keypoints_dataset";
    constexpr std::size_t NumberOfEpochs = 10; // 20 epochs seems to be best; also 30 seem good
    constexpr std::size_t LogInterval = 10;
    constexpr std::size_t TrainDatasetSize = 200;
    constexpr std::size_t TestDatasetSize = 200;

    //************Hyperparameter Tuning **************************//
    if (vm.count("tune")) {
        std::cout << "Hyperparameter tuning: finding best learning rate" << std::endl;
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution dist(0.1, 0.0001);

        // auto learningRates = rmg::linspace(0.1, 0.0001, 50);
        std::vector<double> learningRates;
        for (int i = 0; i < 50; ++i) learningRates.push_back(dist(mt));
        assert(learningRates.size() == 50);

        double bestLearningRate = 0;
        double smallestLoss = {INFINITY};
        const auto hypertuningFn = [&](double learningRate, size_t batchSize) {
            // Recreate the model every time
            auto model = std::make_shared<KeypointsModel>();

            auto localTrainDataset =
                        torch::data::datasets::FaceLandmarksDataset(
                                    "./facial_keypoints_dataset",
                                    torch::data::datasets::FaceLandmarksDataset::Mode::Train, 1000, 40)
                                    .map(torch::data::transforms::Stack<>());

            const size_t LocalTrainDatasetSize = localTrainDataset.size().value();
            DEB(LocalTrainDatasetSize);

            auto localLoader = torch::data::make_data_loader(
                        std::move(localTrainDataset),
                        torch::data::DataLoaderOptions().batch_size(batchSize).workers(2).enforce_ordering(false));

            torch::optim::AdamW localOptimizer(model->parameters(),
                                               torch::optim::AdamWOptions().lr(learningRate).weight_decay(1e-2));
            train(NumberOfEpochs, *model, localLoader, localOptimizer, LocalTrainDatasetSize, LogInterval);

            auto localTestDataset = torch::data::datasets::FaceLandmarksDataset(
                                                "./facial_keypoints_dataset",
                                                torch::data::datasets::FaceLandmarksDataset::Mode::Test, 1000, 40)
                                                .map(torch::data::transforms::Stack<>());

            const size_t LocalTestDatasetSize = localTestDataset.size().value();
            DEB(LocalTestDatasetSize);

            auto testDataLoader = torch::data::make_data_loader(std::move(localTestDataset), batchSize);

            auto loss = test(*model, testDataLoader, LocalTestDatasetSize);

            if (loss < smallestLoss) {
                smallestLoss = loss;

                bestLearningRate = learningRate;
            }

            std::printf("Current learning rate: %.4f; Best learning rate: %.4f\n", learningRate, bestLearningRate);

            return bestLearningRate;
        };
        rmg::PrintVector(learningRates);
        for (const auto& lr : learningRates) {
            std::printf("Trying learning rate: %.4f\n", lr);
            hypertuningFn(lr, 64);
        }
        std::printf("Best learning rate found: %.4f\n", bestLearningRate);
    }


    constexpr std::size_t TrainBatchSize = 64;
    constexpr std::size_t TestBatchSize = 200;
    using FaceLandmarksDataset = torch::data::datasets::FaceLandmarksDataset;
    //******************** Train Model ******************************//
    if (vm.count("train")) {
        std::cout << "Model training started" << std::endl;

        // we apply the Stack collation, which takes a batch of tensors and stacks them into a single tensor along
        // the first dimension:
        auto trainDataset = torch::data::datasets::FaceLandmarksDataset(
                                        DataRoot, torch::data::datasets::FaceLandmarksDataset::Mode::Train,
                                        TrainDatasetSize, TestDatasetSize)
                                        .map(torch::data::transforms::Stack<>());

        const size_t trainDatasetSize = trainDataset.size().value();
        DEB(trainDatasetSize);
        auto trainDataLoader =
                    torch::data::make_data_loader(std::move(trainDataset), torch::data::DataLoaderOptions()
                                                                                       .batch_size(TrainBatchSize)
                                                                                       .workers(2)
                                                                                       .enforce_ordering(false));

        // for (torch::data::Example<>& batch : *trainDataLoader) {
        //     std::cout << "Batch size: " << batch.data.size(0) << " | Image: ";
        //     for (int64_t i = 0; i < batch.data.size(0); ++i) {
        //         if (i % 33 == 0) {
        //             // DEB(batch.data[i].scalar_type());
        //             torch::data::datasets::FaceLandmarksDataset::displayKeyPoints(batch.data[i].view({FaceLandmarksDataset::ImageRows,
        //             FaceLandmarksDataset::ImageColumns, FaceLandmarksDataset::ImageChannels}),
        //                                                                           batch.target[i]);
        //         }
        //     }
        //     std::cout << std::endl;
        // }

        auto testDataset = torch::data::datasets::FaceLandmarksDataset(
                                       DataRoot, torch::data::datasets::FaceLandmarksDataset::Mode::Test,
                                       TrainDatasetSize, TestDatasetSize)
                                       .map(torch::data::transforms::Stack<>());
        // .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))

        const size_t testDatasetSize = testDataset.size().value();
        DEB(testDatasetSize);
        auto testDataLoader = torch::data::make_data_loader(std::move(testDataset), TestBatchSize);

        // torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));
        // ptimizer = optim.SGD(model.parameters(), lr=0.01, momentum=0.9, weight_decay=1e-6, nesterov=True
        // torch::optim::Adam optimizer(model->parameters(), /*lr*/ 0.001);
        // torch::optim::AdamWOptions().lr(0.001).weight_decay(1e-2)); torch::optim::AdamW
        // optimizer(model->parameters(), torch::optim::AdamWOptions().lr(0.0001).weight_decay(1e-5));

        // auto model = KeypointsModel();
        KeypointsModel model;

        // torch::optim::AdamW optimizer(model->parameters(), torch::optim::AdamWOptions().lr(0.05).weight_decay(1e-2));
        torch::optim::AdamW optimizer(model.parameters(), torch::optim::AdamWOptions().lr(0.0642).weight_decay(1e-2));

        train(NumberOfEpochs, model, trainDataLoader, optimizer, trainDatasetSize, LogInterval);

        //******************** Display Results / Test Performance ******************************//
        for (torch::data::Example<>& batch : *testDataLoader) {
            // auto data = batch.data.to(device), targets = batch.target.to(device);

            auto output = model.forward(batch.data);
            size_t idx = 0;
            for (int i = 0; i < batch.data.size(0); ++i) {
                if (idx++ % 50 == 0) {
                    torch::data::datasets::FaceLandmarksDataset::displayKeyPoints(
                                batch.data[i].view({FaceLandmarksDataset::ImageRows,
                                                    FaceLandmarksDataset::ImageColumns,
                                                    FaceLandmarksDataset::ImageChannels}),
                                output[i]);
                }
            }
        }

        // torch::save(model, "KeyPointsModel.pt");
        std::string modelPath = "KeyPointsModel.pt";
        torch::serialize::OutputArchive outputArchive;
        model.save(outputArchive);
        outputArchive.save_to(modelPath);
    }

    if (vm.count("load")) {
        std::cout << "Loading module: " << vm["load"].as<std::string>() << std::endl;
        // torch::jit::script::Module module;
        // try {
        //     // Deserialize the ScriptModule from a file using torch::jit::load().
        //     module = torch::jit::load(vm["load"].as<std::string>());
        // } catch (const c10::Error& e) {
        //     std::cerr << "error loading the model\n";
        //     return -1;
        // }
        auto module = torch::jit::load(vm["load"].as<std::string>());
//   assert(module != nullptr);
        module.eval();

        auto testDataset = torch::data::datasets::FaceLandmarksDataset(
                                       DataRoot, torch::data::datasets::FaceLandmarksDataset::Mode::Test,
                                       TrainDatasetSize, TestDatasetSize)
                                       .map(torch::data::transforms::Stack<>());

        const size_t testDatasetSize = testDataset.size().value();
        DEB(testDatasetSize);
        auto testDataLoader = torch::data::make_data_loader(std::move(testDataset), TestBatchSize);

        //******************** Display Results / Test Performance ******************************//
        for (torch::data::Example<>& batch : *testDataLoader) {
            // auto data = batch.data.to(device), targets = batch.target.to(device);
            // std::vector<torch::jit::IValue> inputs;
            // inputs.push_back(batch.data);

            auto output = module.forward({batch.data}).toTensor();
            // torch::jit::Stack output = model.forward({batch.data}).toTuple()->elements();
            size_t idx = 0;
            for (int i = 0; i < batch.data.size(0); ++i) {
                if (idx++ % 50 == 0) {
                    FaceLandmarksDataset::displayKeyPoints(
                                batch.data[i].view({FaceLandmarksDataset::ImageRows,
                                                    FaceLandmarksDataset::ImageColumns,
                                                    FaceLandmarksDataset::ImageChannels}),
                                output[i]);
                }
            }
        }
    }
    return 0;
}
