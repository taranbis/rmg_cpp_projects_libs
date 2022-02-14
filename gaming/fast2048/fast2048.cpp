#include <random>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <opencv2/ml.hpp>


#include "util.hpp"

#include "utils2048.hpp"
#include "monte_carlo_search.hpp"
//! we can always add 2 equal values
// once we obtain a value score increases by the value we obtain
// 1.
// 2.

using namespace cv;

void ImageFromDisplay(std::vector<uint8_t>& Pixels, const int& Width, const int& Height, int& BitsPerPixel,
                      int startX, int startY)
{
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);

    XWindowAttributes attributes = {0};
    XGetWindowAttributes(display, root, &attributes);

    // Width = attributes.width;
    // Height = attributes.height;

    std::cout << "height = " << Height << ", width = " << Width << std::endl;

    // these are used for the big image
    // XImage* img = XGetImage(display, root, 600, 100, Width, Height, AllPlanes, ZPixmap);
    XImage* img = XGetImage(display, root, startX, startY, Width, Height, AllPlanes, ZPixmap);
    BitsPerPixel = img->bits_per_pixel;
    Pixels.clear();
    Pixels.resize(Width * Height * 4);

    memcpy(&Pixels[0], img->data, Pixels.size());

    XDestroyImage(img);
    XCloseDisplay(display);
}

const int sizex = 103;
const int sizey = 103;

void PreProcessImage(Mat* inImage, Mat* outImage, int sizex, int sizey)
{
    Mat grayImage, blurredImage, thresholdImage, contourImage, regionOfInterest;

    std::vector<std::vector<Point>> contours;

    cvtColor(*inImage, grayImage, COLOR_BGR2GRAY);

    GaussianBlur(grayImage, blurredImage, Size(5, 5), 2, 2);
    adaptiveThreshold(blurredImage, thresholdImage, 255, 1, 1, 11, 2);

    thresholdImage.copyTo(contourImage);

    findContours(contourImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    int idx = 0;
    size_t area = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        if (area < contours[i].size()) {
            idx = i;
            area = contours[i].size();
        }
    }

    Rect rec = boundingRect(contours[idx]);

    regionOfInterest = thresholdImage(rec);

    resize(regionOfInterest, *outImage, Size(sizex, sizey));
}

int main(){
    const std::vector<int> possibleNumbers{2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    const int classes = possibleNumbers.size();
    const int train_samples = 1;

    const int ImageSize = sizex * sizey;
    const char pathToImages[] = ".";

    Mat trainData = Mat(classes * train_samples, ImageSize, CV_32FC1);
    Mat trainClasses = Mat(classes * train_samples, 1, CV_32FC1);

    namedWindow("single", WINDOW_AUTOSIZE);
    namedWindow("all", WINDOW_AUTOSIZE);

    char file[255];
    for (int i = 0; i < possibleNumbers.size() * train_samples; ++i) {
        // sprintf(file, "%s/%d.png", pathToImages, i);
        sprintf(file, "%d.png", possibleNumbers[i % possibleNumbers.size()]);
        Mat src = imread(file, IMREAD_COLOR);
        std::cout << "Reading file: " << file << std::endl;
        if (!src.data) {
            std::cout << "File " << file << " not found\n";
            exit(1);
        }

        src.convertTo(src, CV_32FC4);
        DEB(src.type());
        DEB(src.size());

        Mat img  = Mat::zeros(Size(src.cols, src.rows), CV_32FC1);
        {
            // Split the image into different channels
            Mat rgbChannels[3];
            split(src, rgbChannels);
            DEB(rgbChannels[0].size());
            DEB(rgbChannels[1].size());
            DEB(rgbChannels[2].size());
            // cv::add(rgbChannels[0], rgbChannels[1], img);
            // cv::add(img, rgbChannels[2], img);
            img = (rgbChannels[0] + rgbChannels[1] + rgbChannels[2]);
        }
        DEB(img.type());
        DEB(img.size());


        // Mat outfile;
        // PreProcessImage(&img, &outfile, sizex, sizey);

        for (int r = 0; r < img.rows; r++) {
            for (int c = 0; c < img.cols; c++) { trainData.at<float>(i, r * img.rows + c) = img.at<float>(r, c); }
        }

        trainClasses.at<float>(i) = possibleNumbers[i % possibleNumbers.size()];
    }

    // DEB(trainData.at<float>(0,0));
    // DEB(trainData.data[0]);
    // DEB(trainData.at<float>(1,0));
    // DEB(trainData.at<float>(0,2));
    // DEB(trainData.at<float>(3,4));
    // DEB(trainData.at<float>(4, 12));
    // DEB(trainData.size());

    Ptr kNearest = ml::KNearest::create();
    kNearest->setIsClassifier(true);
    kNearest->setDefaultK(12);
    kNearest->setAlgorithmType(cv::ml::KNearest::Types::BRUTE_FORCE);

    // DEB(trainData.rows);
    // DEB(trainData.cols);

    Ptr trainingData = ml::TrainData::create(trainData, ml::SampleTypes::ROW_SAMPLE, trainClasses);
    kNearest->train(trainingData, 0);

    int wrong = 0;
    int right = 0;
    for (int i = 0; i < possibleNumbers.size(); ++i) {
        std::random_device rd_;
        int iSecret = possibleNumbers[rd_() % 12];
        std::cout <<"Trying to guess number: "<< iSecret << std::endl;
        sprintf(file, "%d.png", iSecret);
        Mat src = imread(file, IMREAD_COLOR);
        if (!src.data) {
            std::cout << "File " << file << " not found\n";
            exit(1);
        }

        src.convertTo(src, CV_32FC4);
        DEB(src.type());
        DEB(src.size());

        Mat img  = Mat::zeros(Size(src.cols, src.rows), CV_32FC1);
        {
            // Split the image into different channels
            Mat rgbChannels[3];
            split(src, rgbChannels);
            DEB(rgbChannels[0].size());
            DEB(rgbChannels[1].size());
            DEB(rgbChannels[2].size());
            // cv::add(rgbChannels[0], rgbChannels[1], img);
            // cv::add(img, rgbChannels[2], img);
            img = (rgbChannels[0] + rgbChannels[1] + rgbChannels[2]);
        }
        DEB(img.type());
        DEB(img.size());

        // single-precision floating-point matrix of `<number_of_samples> * k` size
        // Mat stagedImage = Mat(1, ImageSize, CV_32FC1);
        // PreProcessImage(&img, &stagedImage, sizex, sizey);

        Mat sample2 = Mat(1, ImageSize, CV_32FC1);
        for (int r = 0; r < img.rows; r++) {
            for (int c = 0; c < img.cols; c++) { 
                sample2.at<float>(1, r * img.rows + c) = img.at<float>(r, c); }
        }
        img.~Mat();

        DEB(sample2.type());
        DEB(sample2.size());


        Mat matResults;
        float detectedClass = kNearest->findNearest(sample2, 1, matResults);
        sample2.~Mat();
        DEB(matResults);
        matResults.~Mat();
        DEB(detectedClass);
        if (iSecret != (int)((detectedClass))) {
            std::cout << "Falsch. Ist " << iSecret << " aber geraten ist " << (int)((detectedClass)) << std::endl;
            // exit(1);
            wrong++;
            continue;
        }
        right++;
        std::cout << "Richtig " << (int)((detectedClass)) << "\n";
        // imshow("single", img);
        waitKey(0);
    }

    DEB(wrong);
    DEB(right);
}


int main2()
{
    // FastRandomGenerator randomGen;
    // std::cout << randomGen.generate(16) << std::endl;

    // Board<4,4> board;
    // Game game;
    // int Width = 700;
    // int Height = 750;
    int Width = 600;
    int Height = 550;
    int Bpp = 0;
    std::vector<std::uint8_t> Pixels;
    std::vector<std::uint8_t> squarePixels;

    bool running = true;

    // while (running) {
    ImageFromDisplay(Pixels, Width, Height, Bpp, 650, 300);
    ImageFromDisplay(squarePixels, 104, 104, Bpp, 1081, 345);

    if (Width && Height) {
        Mat img = Mat(Height, Width, Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);
        Mat smallImgEmpty = Mat(104, 104, Bpp > 24 ? CV_8UC4 : CV_8UC3, &squarePixels[0]);
        Mat smallImgEmpty2 = imread("2.png", IMREAD_UNCHANGED);
        Mat smallImg = imread("empty3.png", IMREAD_UNCHANGED);

        if (img.empty() || smallImg.empty()) {
            std::cerr << "Can't read one of the images" << std::endl;
            return EXIT_FAILURE;
        }

        //alpha channel does not get read => we need to add it
        cvtColor(smallImg, smallImg, COLOR_BGR2BGRA);
        cvtColor(smallImgEmpty, smallImgEmpty, COLOR_BGR2BGRA);
        cvtColor(smallImgEmpty2, smallImgEmpty2, COLOR_BGR2BGRA); //opcity is set to 255

        DEB(smallImgEmpty.size());
        DEB(smallImgEmpty2.size());

        Mat a;
        Mat b;
        // a.create(smallImgEmpty, smallImgEmpty2, CV_32FC1);
        absdiff(smallImgEmpty2, smallImgEmpty, a);
        DEB(a);
        DEB(sum(a).cols);
        DEB(sum(a).rows);

        if(sum(a)[0] < 100 && sum(a)[1] < 100 && sum(a)[2] < 100){
            std::cout << "Images are equal" << std::endl;
        }
        cv::compare(smallImgEmpty2 , smallImgEmpty2  , b , cv::CMP_EQ );
        DEB(sum(a));
        imshow("Diff", a);
        // a.count
        DEB(sum(b));
        // DEB(countNonZero(a));
        // DEB(countNonZero(b));
        // NEWLINE();

        Scalar prevStdDev, currentStdDev;
        // meanStdDev(smallImgEmpty, Scalar(), prevStdDev);
        // meanStdDev(smallImgEmpty2, Scalar(), currentStdDev);

        // // Decision Making.
        // if (absdiff(currentStdDev - prevStdDev) < 1) { std::cout << "Images are equal" << std::endl; }

        // namedWindow("Source Image", WINDOW_AUTOSIZE);
        imshow("Initial image", img);
        imshow("Small image", smallImg);

        DEB(smallImg.type());
        DEB(img.type());
        // NEWLINE();

        // auto matchMethod = [&]() {
            // Mat img_display;
            // img.copyTo(img_display);

            Mat result;
            const int result_cols = img.cols - smallImg.cols + 1;
            const int result_rows = img.rows - smallImg.rows + 1;
            result.create(result_rows, result_cols, CV_32FC1);

            matchTemplate(img, smallImg, result, TM_CCOEFF_NORMED/* TM_SQDIFF */);

            double minVal;
            double maxVal;
            Point minLoc;
            Point maxLoc;
            minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

            result.convertTo(result, CV_8UC4);

            // Mat thresholdImage;
            // thresholdImage.create(result_rows, result_cols, CV_8UC4);
            // threshold(result, thresholdImage, 0.9, 255, THRESH_BINARY);
            // double threshold = (minVal + 1e-6) * 50;
            // cv::Mat thresholdImage = result < 100;
            double threshold = 0.08;
            cv::Mat thresholdImage = result >= threshold;


            imshow("thresholdImage", thresholdImage);
            imshow("Result", result);

            for (int r = 0; r < thresholdImage.rows; r++) {
                for (int c = 0; c < thresholdImage.cols; c++) {
                    if (thresholdImage.at<uchar>(r, c) > 0) {
                        rectangle(img, Point(c, r), Point(c + smallImg.cols, r + smallImg.rows), Scalar::all(0), 2,
                                  8, 0);
                    }
                }
            }

            // for (int r = 0; r < thresholdImage.rows; ++r) {
            //     for (int c = 0; c < thresholdImage.cols; ++c) {
            //         if (!thresholdImage.at<unsigned char>(r, c)) // = thresholdedImage(r,c) == 0
            //             cv::circle(img, cv::Point(c, r), smallImg.cols / 2, CV_RGB(0, 255, 0), 1);
            //     }
            // }

            // rectangle(img, minLoc, Point(minLoc.x + smallImg.cols, minLoc.y + smallImg.rows),
            //           Scalar::all(0), 2, 8, 0);
            imshow("Finished Image", img);
            imshow("Result window", result);
            // return;
        // };

        //TODO: not needed here but this is cool;
        // const char* trackbar_label =
        //             "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF "
        //             "\n 5: TM COEFF NORMED";
        // createTrackbar(trackbar_label, "Source Image", &match_method, max_Trackbar);

        // img.isSubmatrix();

        // waitKey(0);
        // matchMethod();
    }

    if ((char)cv::waitKey(0) == 'q') {
        cv::destroyAllWindows();
        running = false;
        // break;
    }
    // }
    return 0;
}