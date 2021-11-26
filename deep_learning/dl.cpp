#include <iostream>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "data_utils.hpp"
#include "linear.hpp"
#include "layer.hpp"
#include "layer_utils.hpp"
#include "sequential.hpp"


int actualMain();

template <typename Derived>
void printSize(const Eigen::MatrixBase<Derived> &b) // EigenBase instead of MatrixBase works as well
{
    std::cout << "size; (rows, cols): " << b.size() << "; (" << b.rows() << ", " << b.cols() << ")"
              << std::endl;
}

// int main(){

// }

/**
 * @brief How to use the framework
 *
 * input = torch.randn(3, 5, requires_grad=True)
 * target = torch.randint(5, (3,), dtype=torch.int64)
 * loss = F.cross_entropy(input, target)
 * loss.backward()
 * @param bias
 */
int main()
{
    constexpr size_t batchDim = 100; // numbers of images in a batch;
    constexpr size_t D_in = 3072 /* input dim */, H1 = 1024 /* hidden dim */, H2 = 256/* hidden dim */, D_out = 10 /* output dim */;

    auto fc1  = std::unique_ptr<Linear<double, batchDim, D_in, H1>>(new Linear<double, batchDim, D_in, H1>());
    auto relu = std::unique_ptr<ReLU<double>>(new ReLU<double>());
    auto fc2  = std::unique_ptr<Linear<double, batchDim, H1, H2>>(new Linear<double, batchDim, H1, H2>());
    auto relu2 = std::unique_ptr<ReLU<double>>(new ReLU<double>());
    auto fc3  = std::unique_ptr<Linear<double, batchDim, H2, D_out>>(new Linear<double, batchDim, H2, D_out>());
    // auto lossFn = std::unique_ptr<L2Loss<double, batchDim, D_out>>;

    Sequential<double, batchDim, D_in, D_out> model;
    model << std::move(fc1) << std::move(relu) << std::move(fc2) << std::move(relu2) << std::move(fc3);

    L2Loss<double, batchDim, D_out> lossFn;

    // Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x = Eigen::MatrixXd::Random(batchDim, D_in);
    // Eigen::Matrix<double, Eigen::Dynamic, 1> y = Eigen::Matrix<double, batchDim, 1>::Random();

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x;
    Eigen::Matrix<double, Eigen::Dynamic, 1> y;

    std::tie(x, y) = loadBatchCIFAR10<batchDim>("data_batch_1.bin");

    Eigen::MatrixXd y_pred = Eigen::Matrix<double, batchDim, D_out>::Zero();

    for (std::size_t i = 0; i < 500; ++i) {
        y_pred = model(x);

        double loss = lossFn(y_pred, y);
        //this is the accuracy
        /* if (i % 100 == 99) */ std::cout << "loss: " << loss << std::endl;

        model.zeroGrads();

        model.backward(lossFn.backward());

        double learningRate = 1e-4;

        model.updateParams(learningRate);
    }

    return 0;
}