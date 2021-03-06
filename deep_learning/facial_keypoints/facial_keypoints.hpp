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

class FaceLandmarksDataset : public torch::data::datasets::Dataset<FaceLandmarksDataset>
{
public:
    /// The mode in which the dataset is loaded.
    enum class Mode { Train, Test };

    /// Loads the  dataset from the `root` path.
    explicit FaceLandmarksDataset(const std::string& root, Mode mode = Mode::Train, size_t trainSize = 4800,
                                  size_t testSize = 200)
            : images_(readImages(root, mode == Mode::Train)), keyPoints_(readTargets(root, mode == Mode::Train)),
              TrainSize(trainSize), TestSize(testSize)
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
    // const uint32_t TrainSize = 4800;
    // const uint32_t TestSize = 200;
    const uint32_t TrainSize = 1000;
    const uint32_t TestSize = 20;

public:
    static constexpr uint32_t ImageRows = 512;
    static constexpr uint32_t ImageColumns = 512;
    // const uint32_t ImageChannels = 3;
    static constexpr uint32_t ImageChannels = 1;

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

        Tensor rv = torch::empty({count, ImageChannels, ImageRows, ImageColumns}, torch::kFloat32);

        const auto path = joinPaths(root, "all_data.json");
        const auto imagesPath = joinPaths(root, "images");
        std::ifstream jsonFile(path);
        nlohmann::json j;
        jsonFile >> j;

        // uchar* buffer = new uchar[img.rows * img.cols * img.channels()];
        // memcpy(img.data, buffer, img.rows * img.cols * img.channels());

        for (int i = 0; i < count; ++i) {
            const auto imgPath = joinPaths(imagesPath, j.at(std::to_string(i)).at("file_name"));
            rv[i] = getImgData(imgPath);
        }

        DEB(rv.sizes());
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

        Tensor rv = torch::empty({count, 68 * 2}, torch::kFloat32);

        for (int j = 0; j < count; ++j) {
            nlohmann::json faceLandmarks = json.at(std::to_string(j)).at("face_landmarks");

            for (int i = 0; i < faceLandmarks.size(); ++i) {
                int x = faceLandmarks[i][0];
                int y = faceLandmarks[i][1];

                rv[j][i * 2] = x;
                rv[j][i * 2 + 1] = y;
            }
        }

        DEB(rv.sizes());
        return rv;
    }
public:
    static Tensor getImgData(const std::string& path)
    {
        // cv::Mat img = rmg::readImg(path);
        const std::string imagePath = cv::samples::findFile(path);
        cv::Mat img = cv::imread(imagePath, cv::IMREAD_GRAYSCALE); // CV_8UC1
        if (img.empty()) std::cerr << "Could not read the image: " << imagePath << std::endl;

        assert(img.rows == ImageRows);
        assert(img.cols == ImageColumns);
        assert(img.channels() == ImageChannels);

        //! doesn't take ownership
        Tensor tensor = rmg::cvMatToTorchTensor(img);
        // tensor.unsqueeze_(0);
        // tensor = tensor.toType(torch::kFloat).sub(127.5).mul(0.0078125);
        tensor = tensor.to(torch::kFloat32).div(255);

        tensor = tensor.view({ImageChannels, ImageRows, ImageColumns});
        return tensor;
        // return tensor.flatten();
    };

    static void displayKeyPoints(Tensor tensor, Tensor faceLandmarks)
    {
        tensor = tensor.mul(255);
        tensor = tensor.to(torch::kByte);
        //TODO: select type (CV_8UC1) based on the channels given here
        int64_t height = tensor.size(0);
        int64_t width = tensor.size(1);
        cv::Mat img = cv::Mat(cv::Size(width, height), CV_8UC1, tensor.data_ptr<uchar>());
        for (int i = 0; i < faceLandmarks.sizes()[0] / 2; ++i) {
            int x = faceLandmarks[2 * i].item<float>();
            int y = faceLandmarks[2 * i + 1].item<float>();
            cv::circle(img, cv::Point{x, y}, 1, cv::Scalar{0, 0, 255}, 5);
        }

        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create Window
        cv::imshow("Display window", img);

        int k = cv::waitKey(0); // Wait for a keystroke in the window
    }
};

} // namespace torch::data::datasets

#endif