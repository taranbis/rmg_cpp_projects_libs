#include "facial_keypoints.hpp"

/******************************************************************************************************************
 ** Facial Keypoints Detection
 *
 * This project will be all about defining and training a convolutional neural network to perform facial keypoint 
 * detection, and using computer vision techniques to transform images of faces.
 *****************************************************************************************************************/

int main()
{
    const char* kDataRoot = "./data";
    // constexpr int TrainBatchSize = 64;
    // constexpr int TestBatchSize = 1000;
    constexpr int TrainBatchSize = 1;
    constexpr int TestBatchSize = 1;
    constexpr int64_t NumberOfEpochs = 10;
    constexpr int64_t LogInterval = 10;

    // we apply the Stack collation, which takes a batch of tensors and stacks them into a single tensor along the first dimension:
    auto trainDataset = torch::data::datasets::FaceLandmarksDataset ("./facial_keypoints_dataset").map(torch::data::transforms::Stack<>());

    const size_t trainDatasetSize = trainDataset.size().value();
    DEB(trainDatasetSize);
    auto trainDataLoader = torch::data::make_data_loader(
                std::move(trainDataset), torch::data::DataLoaderOptions().batch_size(TrainBatchSize)/* .workers(2) */);

    // for (torch::data::Example<>& batch : *trainDataLoader) {
    //     std::cout << "Batch size: " << batch.data.size(0) << " | Labels: ";
    //     // for (int64_t i = 0; i < batch.data.size(0); ++i) { std::cout << batch.target[i].item<int64_t>() << " "; }
    //     std::cout << std::endl;
    // }

    // auto testDataset = torch::data::datasets::FaceLandmarksDataset(kDataRoot, torch::data::datasets::FaceLandmarksDataset::Mode::Test);
    //                                 // .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
    //                                 // .map(torch::data::transforms::Stack<>());
    // const size_t testDatasetSize = testDataset.size().value();
    // auto test_loader = torch::data::make_data_loader(std::move(testDataset), TestBatchSize);

    // torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));

    // for (size_t epoch = 1; epoch <= NumberOfEpochs; ++epoch) {
    //     train(epoch, model, *trainDataLoader, optimizer, trainDatasetSize);
    //     test(model, *test_loader, testDatasetSize);
    // }

    return 0;
}
