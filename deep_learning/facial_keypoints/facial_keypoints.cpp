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
    // constexpr int TrainBatchSize = 1;
    // constexpr int TestBatchSize = 1;
    constexpr int64_t NumberOfEpochs = 10;
    constexpr int64_t LogInterval = 10;

    // we apply the Stack collation, which takes a batch of tensors and stacks them into a single tensor along the first dimension:
    auto trainDataset = torch::data::datasets::FaceLandmarksDataset ("./facial_keypoints_dataset").map(torch::data::transforms::Stack<>());

    const size_t trainDatasetSize = trainDataset.size().value();
    DEB(trainDatasetSize);
    auto trainDataLoader = torch::data::make_data_loader(
                std::move(trainDataset), torch::data::DataLoaderOptions().batch_size(TrainBatchSize).workers(2).enforce_ordering(false));

    for (torch::data::Example<>& batch : *trainDataLoader) {
        std::cout << "Batch size: " << batch.data.size(0) << " | Image: ";
        for (int64_t i = 0; i < batch.data.size(0); ++i) {
            if (i % 33 == 0) {
                torch::data::datasets::FaceLandmarksDataset::displayKeyPoints(
                            batch.data[i].view({512, 512, 3}), batch.target[i]);
            }
        }
        std::cout << std::endl;
    }

    auto testDataset = torch::data::datasets::FaceLandmarksDataset(DataRoot, torch::data::datasets::FaceLandmarksDataset::Mode::Test);
                                    // .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
                                    // .map(torch::data::transforms::Stack<>());
    const size_t testDatasetSize = testDataset.size().value();
    DEB(testDatasetSize);
    auto test_loader = torch::data::make_data_loader(std::move(testDataset), TestBatchSize);

    // torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));

    // for (size_t epoch = 1; epoch <= NumberOfEpochs; ++epoch) {
    //     train(epoch, model, *trainDataLoader, optimizer, trainDatasetSize);
    //     test(model, *test_loader, testDatasetSize);
    // }

    // Load model and run the solver
    // model = KeypointModel()
    // criterion = nn.MSELoss()
    // # optimizer = optim.SGD(model.parameters(), lr=0.01, momentum=0.9, weight_decay=1e-6, nesterov=True)
    // optimizer = optim.Adam(model.parameters(), lr = 0.001)

    return 0;
}
