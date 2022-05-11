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

#include "utils.hpp"

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
            : images_(readImages(root, mode == Mode::Train)), keyPoints_(readTargets(root, mode == Mode::Train))
    {
    }

    /// Returns the `Example` at the given `index`.
    Example<> get(size_t index) override
    {
        return {images_[index], keyPoints_[index]};
    }

    /// Returns the size of the dataset.
    optional<size_t> size() const override
    {
        return images_.size(0);
    }

    /// Returns true if this is the training subset
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
        return keyPoints_;
    }

private:
    // const uint32_t TrainSize = 4500;
    // const uint32_t TestSize = 500;
    const uint32_t TrainSize = 10;
    const uint32_t TestSize = 2;

    const uint32_t ImageRows = 512;
    const uint32_t ImageColumns = 512;
    const uint32_t ImageChannels = 3;

    Tensor images_;
    Tensor keyPoints_;

private:
    std::string joinPaths(std::string head, const std::string& tail)
    {
        if (head.back() != '/') head.push_back('/');
        head += tail;
        return head;
    }

    Tensor readImages(const std::string& root, bool train)
    {
        const auto count = train ? TrainSize : TestSize;

        // Tensor rv = torch::empty({count, ImageRows * ImageColumns * ImageChannels}, torch::kByte);
        Tensor rv = torch::empty({count, ImageChannels, ImageRows, ImageColumns}, torch::kByte);

        const auto path = joinPaths(root, "all_data.json");
        const auto imagesPath = joinPaths(root, "images");
        std::ifstream jsonFile(path);
        nlohmann::json j;
        jsonFile >> j;

        // // images.read(reinterpret_cast<char*>(tensor.data_ptr()), tensor.numel());
        // return tensor.to(torch::kFloat32).div_(255);

        // uchar* buffer = new uchar[img.rows * img.cols * img.channels()];
        // memcpy(img.data, buffer, img.rows * img.cols * img.channels());

        for (int i = 0; i < count; ++i) {
            const auto imgPath = joinPaths(imagesPath, j.at(std::to_string(i)).at("file_name"));
            rv[i] = getImgData(imgPath);
        }

        DEB(rv.sizes());
        DEB(rv.scalar_type());
        return rv;
    }

    Tensor readTargets(const std::string& root, bool train)
    {
        const auto count = train ? TrainSize : TestSize;

        // read a JSON file
        const auto path = joinPaths(root, "all_data.json");
        const auto imagesPath = joinPaths(root, "images");
        std::ifstream jsonFile(path);
        nlohmann::json json;
        jsonFile >> json;
        // std::cout << std::setw(4) << j << std::endl;

        Tensor rv = torch::empty({count, 68 * 2}, torch::kFloat32);

        // TODO:remove
        // Tensor images = torch::empty({count, 786432}, torch::kByte);

        for (int j = 0; j < count; ++j) {
            nlohmann::json faceLandmarks = json.at(std::to_string(j)).at("face_landmarks");

            // //TODO:remove
            // const auto imgPath = joinPaths(imagesPath, json.at(std::to_string(j)).at("file_name"));
            // images[j] = getImgData(imgPath);

            for (int i = 0; i < faceLandmarks.size(); ++i) {
                int x = faceLandmarks[i][0];
                int y = faceLandmarks[i][1];

                rv[j][i * 2] = x;
                rv[j][i * 2 + 1] = y;
            }
        }

        // for (int j = 0; j < count; ++j) displayKeyPointsTorch(images[j].view({512, 512, 3}), rv[j]);

        DEB(rv.sizes());
        return rv;
    }

    Tensor getImgData(const std::string& path)
    {
        cv::Mat img = rmg::readImg(path);
        assert(img.rows == ImageRows);
        assert(img.cols == ImageColumns);
        assert(img.channels() == ImageChannels);

        //! doesn't take ownership
        Tensor tensor = rmg::cvMatToTorchTensor(img);
        // tensor.unsqueeze_(0);
        // tensor = tensor.toType(torch::kFloat).sub(127.5).mul(0.0078125);
        // auto newTensor = tensor.to(torch::kFloat32) /* .div_(255) */;

        tensor = tensor.view({3,512,512});
         return tensor;
        // return tensor.flatten();
    };

public:
    static void displayKeyPoints(Tensor tensor, Tensor faceLandmarks)
    {
        int64_t height = tensor.size(0);
        int64_t width = tensor.size(1);
        DEB(height);
        DEB(width);
        cv::Mat img = cv::Mat(cv::Size(width, height), CV_8UC3, tensor.data_ptr<uchar>());
        for (int i = 0; i < faceLandmarks.sizes()[0] / 2; ++i) {
            int x = faceLandmarks[2 * i].item<double>();
            int y = faceLandmarks[2 * i + 1].item<double>();
            cv::circle(img, cv::Point{x, y}, 1, cv::Scalar{0, 0, 255}, 5);
        }

        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create Window
        cv::imshow("Display window", img);

        int k = cv::waitKey(0); // Wait for a keystroke in the window
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