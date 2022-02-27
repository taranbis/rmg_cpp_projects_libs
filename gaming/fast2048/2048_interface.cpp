// #ifndef _2048_INTERFACE_HEADER_HPP_
// #define _2048_INTERFACE_HEADER_HPP_ 1
// #pragma once



        // double threshold = 0.9;
        // ImageFinder imageFinder = new ImageFinder(image_source, image_partial1, threshold);
        // imageFinder.FindThenShow();
class ImageFinder
{
private:
    // std::vector<Rectangle> rectangles_{};
    Mat baseImage_;
    Mat subImage_;
    Mat resultImage_;
    double threshold_;

public:
    // std::vector<Rectangle> Rectangles
    // {
    //     return rectangles_;
    // }

    ImageFinder(Mat baseImage, Mat subImage, double threshold): baseImage_(baseImage), subImage_(subImage), threshold_(threshold)
    {
    }

    void findThenShow()
    {
        findImage();
        drawRectanglesOnImage();
        // showImage();
    }

    void drawRectanglesOnImage()
    {
        // ResultImage = BaseImage.Copy();
        // for (const auto& rectangle : rectangles_) {
        //     // baseImage_.Draw(rectangle, new Bgr(Color.Blue), 1);
        //     // rectangle(baseImage_, matchLoc, Point(matchLoc.x + smallImg.cols, matchLoc.y + smallImg.rows), Scalar::all(0),
        //     //           2, 8, 0);
        // }
    }

    void findImage()
    {
        // Mat imgSrc = baseImage_.copy();

    }

    void ShowImage() const
    {
        std::string outFilename = "matched Templates";
        imshow(outFilename, resultImage_);
    }
};


#endif //!_2048_INTERFACE_HEADER_HPP_