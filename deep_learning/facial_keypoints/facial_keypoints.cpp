#include "facial_keypoints.hpp"
#include "facial_keypoints_model.hpp"

/******************************************************************************************************************
 ** Facial Keypoints Detection
 *
 * This project will be all about defining and training a convolutional neural network to perform facial keypoint
 * detection, and using computer vision techniques to transform images of faces.
 *****************************************************************************************************************/

int main()
{
    const char* DataRoot = "./facial_keypoints_dataset";
    constexpr int TrainBatchSize = 64;
    constexpr int TestBatchSize = 1000;
    constexpr int64_t NumberOfEpochs = 10;
    constexpr int64_t LogInterval = 10;

    // we apply the Stack collation, which takes a batch of tensors and stacks them into a single tensor along the
    // first dimension:
    auto trainDataset = torch::data::datasets::FaceLandmarksDataset("./facial_keypoints_dataset")
                                    .map(torch::data::transforms::Stack<>());

    const size_t trainDatasetSize = trainDataset.size().value();
    DEB(trainDatasetSize);
    auto trainDataLoader = torch::data::make_data_loader(
                std::move(trainDataset),
                torch::data::DataLoaderOptions().batch_size(TrainBatchSize).workers(2).enforce_ordering(false));

    for (torch::data::Example<>& batch : *trainDataLoader) {
        std::cout << "Batch size: " << batch.data.size(0) << " | Image: ";
        for (int64_t i = 0; i < batch.data.size(0); ++i) {
            if (i % 33 == 0) {
                DEB(batch.data[i].scalar_type());
                torch::data::datasets::FaceLandmarksDataset::displayKeyPoints(batch.data[i].view({512, 512, 3}),
                                                                              batch.target[i]);
            }
        }
        std::cout << std::endl;
    }

    auto testDataset = torch::data::datasets::FaceLandmarksDataset(
                DataRoot, torch::data::datasets::FaceLandmarksDataset::Mode::Test);
    // .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
    // .map(torch::data::transforms::Stack<>());
    const size_t testDatasetSize = testDataset.size().value();
    DEB(testDatasetSize);
    auto testDataLoader = torch::data::make_data_loader(std::move(testDataset), TestBatchSize);

    auto model = std::make_shared<KeypointsModel>();

    // torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));
    // ptimizer = optim.SGD(model.parameters(), lr=0.01, momentum=0.9, weight_decay=1e-6, nesterov=True
    torch::optim::Adam optimizer(model->parameters(), /*lr*/ 0.001);

    for (size_t epoch = 1; epoch <= NumberOfEpochs; ++epoch) {
        // train(epoch, model, *trainDataLoader, optimizer, trainDatasetSize);
        // test(model, *testDataLoader, testDatasetSize);
        //* train
        size_t batchIdx = 0;
        for (torch::data::Example<>& batch : *trainDataLoader) {
            // auto data = batch.data.to(device), targets = batch.target.to(device);
            optimizer.zero_grad();
            auto output = model->forward(batch.data);
            auto loss = torch::mse_loss(output, batch.target);
            AT_ASSERT(!std::isnan(loss.template item<float>()));
            loss.backward();
            optimizer.step();

            if (batchIdx++ % LogInterval == 0) {
                std::printf("\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f\n", epoch, batchIdx * batch.data.size(0),
                            trainDatasetSize, loss.template item<float>());
            }
        }
    }

    //* test
    for (torch::data::Example<>& batch : *trainDataLoader) {
        // auto data = batch.data.to(device), targets = batch.target.to(device);

        auto output = model->forward(batch.data);
        for (int i = 0; i < batch.data.size(0); ++i) {
            torch::data::datasets::FaceLandmarksDataset::displayKeyPoints(batch.data[i].view({512, 512, 3}),
                                                                          output[i]);
        }
    }

    return 0;
}
