#ifndef _FACIAL_KEYPOINTS_HEADER_HPP_
#define _FACIAL_KEYPOINTS_HEADER_HPP_ 1
#pragma once

#include <iostream>

#include <torch/torch.h>
#include <torch/csrc/autograd/variable.h>
#include <torch/csrc/autograd/function.h>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/quality/qualitymse.hpp>

#include <nlohmann/json.hpp>

#include "util.hpp"
#include "cv_utils.hpp"

namespace torch::data::datasets
{
namespace
{
bool check_is_little_endian()
{
    const uint32_t word = 1;
    return reinterpret_cast<const uint8_t*>(&word)[0] == 1;
}

constexpr uint32_t flip_endianness(uint32_t value)
{
    return ((value & 0xffu) << 24u) | ((value & 0xff00u) << 8u) | ((value & 0xff0000u) >> 8u) |
           ((value & 0xff000000u) >> 24u);
}
} // namespace

class FaceLandmarksDataset : public torch::data::datasets::Dataset<FaceLandmarksDataset>
{
public:
    /// The mode in which the dataset is loaded.
    enum class Mode { Train, Test };

    /// Loads the  dataset from the `root` path.
    explicit FaceLandmarksDataset(const std::string& root, Mode mode = Mode::Train)
            : images_(readImages(root, mode == Mode::Train)), targets_(readTargets(root, mode == Mode::Train))
    {
    }

    /// Returns the `Example` at the given `index`.
    Example<> get(size_t index) override
    {
        return {images_[index], targets_[index]};
        // return images_[index];
    }

    /// Returns the size of the dataset.
    optional<size_t> size() const override
    {
        return images_.size(0);
    }

    /// Returns true if this is the training subset of MNIST.
    // NOLINTNEXTLINE(bugprone-exception-escape)
    bool is_train() const noexcept
    {
        return images_.size(0) == TrainSize;
    }

    /// Returns all images stacked into a single tensor.
    const Tensor& images() const
    {
        return images_;
    }

    /// Returns all targets stacked into a single tensor.
    const Tensor& targets() const
    {
        return targets_;
    }

private:
    // const uint32_t TrainSize = 60000;
    // const uint32_t TestSize = 10000;
    const uint32_t TrainSize = 10;
    const uint32_t TestSize = 10;

    const uint32_t kImageRows = 28;
    const uint32_t kImageColumns = 28;

    Tensor images_;
    Tensor targets_;

    std::string join_paths(std::string head, const std::string& tail)
    {
        if (head.back() != '/') head.push_back('/');
        head += tail;
        return head;
    }

    Tensor readImages(const std::string& root, bool train)
    {
        const auto path = join_paths(root, "images/00001.png");
        // std::ifstream images(path, std::ios::binary);
        // TORCH_CHECK(images, "Error opening images file at ", path);

        const auto count = train ? TrainSize : TestSize;

        const std::string imagePath = cv::samples::findFile(path);
        cv::Mat img = cv::imread(imagePath); // CV_8UC1
        const int rows = img.rows;
        const int cols = img.cols;
        if (img.empty()) { std::cerr << "Could not read the image: " << imagePath << std::endl; }
        std::cout << "Read the image: " << imagePath << std::endl;

        // cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create Window
        // cv::imshow("Display window", img);

        // int k = cv::waitKey(25); // Wait for a keystroke in the window

        // // From http://yann.lecun.com/exdb/mnist/
        //   expect_int32(images, count);

        // auto tensor = torch::empty({count, 1, rows, cols}, torch::kByte);
        // // images.read(reinterpret_cast<char*>(tensor.data_ptr()), tensor.numel());
        // return tensor.to(torch::kFloat32).div_(255);

        // DEB(img.size());
        // uchar* buffer = new uchar[img.rows * img.cols * img.channels()];
        // memcpy(img.data, buffer, img.rows * img.cols * img.channels());

        // //TODO: doesn't take ownership
        Tensor tensor = torch::from_blob(img.data, {img.rows, img.cols, img.channels()}, at::kByte);
        tensor = tensor.flatten();
        // tensor.unsqueeze_(0);
        // tensor = tensor.toType(torch::kFloat).sub(127.5).mul(0.0078125);
        auto newTensor = tensor.to(torch::kFloat32) /* .div_(255) */;
        // // DEB(newTensor);

        return newTensor;
    }

    Tensor readTargets(const std::string& root, bool train)
    {
        // read a JSON file
        const auto path = join_paths(root, "all_data.json");
        std::ifstream jsonFile(path);
        nlohmann::json j;
        jsonFile >> j;

        // serialize to standard output
        // std::cout << std::setw(4) << j << std::endl;
        // std::cout << std::setw(4) << j.at("0") << std::endl;
        // DEB(j.size());
        nlohmann::json faceLandmarks = j.at("1").at("face_landmarks");

        const auto imgPath = join_paths(root, "images/00001.png");
        const auto count = train ? TrainSize : TestSize;

        const std::string imagePath = cv::samples::findFile(imgPath);
        cv::Mat img = cv::imread(imagePath); // CV_8UC1
        const int rows = img.rows;
        const int cols = img.cols;
        if (img.empty()) { std::cerr << "Could not read the image: " << imagePath << std::endl; }
        std::cout << "Read the image: " << imagePath << std::endl;

        // DEB(img.size());
        // rmg::printType(img);

        for (int i = 0; i < faceLandmarks.size(); ++i) {
            int x = faceLandmarks[i][0];
            int y = faceLandmarks[i][1];
            cv::circle(img, cv::Point{x, y}, 1, cv::Scalar{0, 0, 255}, 5);
        }

        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create Window
        cv::imshow("Display window", img);

        int k = cv::waitKey(0); // Wait for a keystroke in the window

        return {};
    }
};

} // namespace torch::data::datasets

#endif

//* Read big file
//     // Buffer size 1 Megabyte (or any number you like)
//     size_t buffer_size = 1 << 20;
//     char* buffer = new char[buffer_size];

//     // std::ifstream jsonFile("./all_data.json");

//     while (jsonFile) {
//         // Try to read next chunk of data
//         jsonFile.read(buffer, buffer_size);
//         // Get the number of bytes actually read
//         size_t count = jsonFile.gcount();
//         // If nothing has been read, break
//         if (!count) break;
//         // Do whatever you need with first count bytes in the buffer
//         for(int i = 0; i < count; ++i){
//             std::cout<<buffer[i]<<std::endl;
//         }
//     }

//     delete[] buffer;