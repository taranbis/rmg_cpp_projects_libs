#include <iostream>

#include <torch/torch.h>
#include <torch/csrc/autograd/variable.h>
#include <torch/csrc/autograd/function.h>

// Define a new Module.
struct Net : torch::nn::Module {
    Net()
    {
        // Construct and register three Linear submodules.
        fc1 = register_module("fc1", torch::nn::Linear(784, 64));
        fc2 = register_module("fc2", torch::nn::Linear(64, 32));
        fc3 = register_module("fc3", torch::nn::Linear(32, 10));
    }

    // Implement the Net's algorithm.
    torch::Tensor forward(torch::Tensor x)
    {
        // Use one of many tensor manipulation functions.
        x = torch::relu(fc1->forward(x.reshape({x.size(0), 784})));
        x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
        return x;
    }

    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
};

int main()
{
    torch::DeviceType deviceType;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA available! Training on GPU." << std::endl;
        deviceType = torch::kCUDA;
    } else {
        std::cout << "Training on CPU." << std::endl;
        deviceType = torch::kCPU;
    }
    torch::Device device(deviceType);

    auto net = std::make_shared<Net>();

    auto dataLoader = torch::data::make_data_loader(
                torch::data::datasets::MNIST("./data").map(torch::data::transforms::Stack<>()), /*batch size*/ 64);

    torch::optim::SGD optimizer(net->parameters(), /*lr*/ 0.01);

    for (std::size_t epoch = 1; epoch <= 10; epoch++) {
        std::size_t batchIdx = 0;
        for (auto& batch : *dataLoader) {
            // Reset gradients.
            optimizer.zero_grad();
            // Execute the model on the input data.
            torch::Tensor prediction = net->forward(batch.data);

            torch::Tensor loss = torch::nll_loss(prediction, batch.target);

            // Compute gradients of the loss w.r.t. the parameters of our model.
            loss.backward();

            // Update the parameters based on the calculated gradients
            optimizer.step();
            // Output the loss and checkpoint every 100 batches.
            if (++batchIdx % 100 == 0) {
                std::cout << "Epoch: " << epoch << " | Batch: " << batchIdx << " | Loss: " << loss.item<float>()
                          << std::endl;
                // Serialize your model periodically as a checkpoint.
                torch::save(net, "net.pt");
            }
        }
    }

    torch::Tensor tensor = torch::eye(3);
    std::cout << tensor << std::endl;
}
