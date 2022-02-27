#include <iostream>
#include <chrono>
#include <thread>
#include <limits>

#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "util.hpp"

#include "game_2048.hpp"
#include "monte_carlo_search.hpp"

using namespace cv;

void ImageFromDisplay(std::vector<uint8_t>& Pixels, const int& Width, const int& Height, int& BitsPerPixel,
                      int startX, int startY)
{
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);

    XWindowAttributes attributes = {0};
    XGetWindowAttributes(display, root, &attributes);

    XImage* img = XGetImage(display, root, startX, startY, Width, Height, AllPlanes, ZPixmap);
    BitsPerPixel = img->bits_per_pixel;
    Pixels.clear();
    Pixels.resize(Width * Height * 4);

    memcpy(&Pixels[0], img->data, Pixels.size());

    XDestroyImage(img);
    XCloseDisplay(display);
}

int main()
{
    // FastRandomGenerator randomGen;
    // std::cout << randomGen.generate(16) << std::endl;

    Board<4,4> board;
    Board<4,4> prevBoard;
    Game game(board);

    const std::vector<int> possibleNumbers{2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

    const std::vector<std::vector<cv::Point>> tileCentres{
                {cv::Point(120, 95), cv::Point(241, 95), cv::Point(361, 95), cv::Point(484, 95)},
                {cv::Point(120, 215), cv::Point(240, 215), cv::Point(364, 215), cv::Point(483, 215)},
                {cv::Point(120, 335), cv::Point(243, 335), cv::Point(361, 335), cv::Point(484, 335)},
                {cv::Point(120, 458), cv::Point(242, 458), cv::Point(362, 458), cv::Point(482, 458)},
    };

    auto calculatePointIndex = [&tileCentres](cv::Point p) {
        std::pair<int, int> minPoint;
        int minDistance = std::numeric_limits<int>::max();
        for (std::size_t c = 0; c < tileCentres.size(); ++c) {
            for (std::size_t r = 0; r < tileCentres[c].size(); ++r) {
                const int norm = cv::norm(tileCentres[c][r] - p);
                if (norm < minDistance) {
                    minDistance = norm;
                    minPoint = {c, r};
                }
            }
        }
        return minPoint;
    };

    std::vector<std::pair<cv::Mat, int>> searchedImages;
    for (const int& number : possibleNumbers) {
        const std::string imgName = std::to_string(number) + ".png";
        const cv::Mat pngImg = imread(imgName, IMREAD_UNCHANGED);
        if (pngImg.empty()) {
            std::cerr << "Could not read image: " << imgName << std::endl;
            return EXIT_FAILURE;
        }
        searchedImages.emplace_back(std::pair<cv::Mat, int>{pngImg, number});
    }

    const int Width = 600;
    const int Height = 550;
    int Bpp = 0;

    const int sizex = 103;
    const int sizey = 103;
    std::vector<std::uint8_t> Pixels;
    std::vector<std::uint8_t> squarePixels;

    bool running = true;
    while (running) {
        ImageFromDisplay(Pixels, Width, Height, Bpp, 650, 300);
        ImageFromDisplay(squarePixels, sizex, sizey, Bpp, 1081, 345);

        for (const std::pair<cv::Mat, int>& searchedImagePair : searchedImages) {
            const cv::Mat searchImage = searchedImagePair.first;
            const int searchedNumber = searchedImagePair.second;
            const cv::Mat img = Mat(Height, Width, Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);

            if (img.empty()) {
                std::cerr << "Can't read one of the images" << std::endl;
                return EXIT_FAILURE;
            }

            // alpha channel does not get read => we need to add it
            cvtColor(searchImage, searchImage, COLOR_BGR2BGRA); // opacity is set to 255

            cv::Mat result;
            const int result_cols = img.cols - searchImage.cols + 1;
            const int result_rows = img.rows - searchImage.rows + 1;
            result.create(result_rows, result_cols, CV_32FC1);

            matchTemplate(img, searchImage, result, TM_CCOEFF_NORMED /* TM_SQDIFF */);

            double minVal;
            minMaxLoc(result, &minVal /* , &maxVal, &minLoc, &maxLoc, Mat() */);

            //! be careful with conversions. keep it in float to do floating point stuff
            const double threshold = 0.75;
            const cv::Mat thresholdImage = result >= threshold;

            auto isIntersecting = [](cv::Point bottomLeft1, cv::Point topRight1, cv::Point bottomLeft2,
                                     cv::Point topRight2) {
                if (topRight1.x < bottomLeft2.x || bottomLeft1.x > topRight2.x)
                    return false; // means they are intersecting
                if (topRight1.y < bottomLeft2.y || bottomLeft1.y > topRight2.y)
                    return false; // means they are intersecting
                return true;
            };

            std::vector<cv::Point> matchingPoints{};
            for (int r = 0; r < thresholdImage.rows; r++) {
                for (int c = 0; c < thresholdImage.cols; c++) {
                    if (thresholdImage.at<uchar>(r, c) > 0) {
                        bool intersected = false;
                        for (const cv::Point& matchingPoint : matchingPoints) {
                            if (isIntersecting(matchingPoint, {matchingPoint.x + sizex, matchingPoint.y + sizey},
                                               {c, r}, {c + sizex, r + sizey})) {
                                intersected = true;
                                break;
                            }
                        }
                        if (!intersected) {
                            matchingPoints.emplace_back(cv::Point{c, r});
                            rectangle(img, cv::Point(c, r), cv::Point(c + sizex, r + sizey), cv::Scalar::all(0), 2, 8, 0);
                            circle(img, cv::Point(c + sizex / 2, r + sizey / 2), 1, Scalar{0, 0, 255}, 5);
                            std::pair<int, int> pointPosition = calculatePointIndex(cv::Point(c + sizex / 2, r + sizey / 2));
                            // results[pointPosition.first][pointPosition.second] = searchedNumber;
                            board.at(pointPosition.first, pointPosition.second) = searchedNumber;
                        }
                    }
                }
            }
            // imshow("Finished Image", img);
        }
        if ((char)cv::waitKey(1) == 'q') {
            cv::destroyAllWindows();
            running = false;
            break;
        }

        game.setBoard(board);
        // std::cout << "can move left: " << std::boolalpha << board.canMoveLeft() << std::endl;
        // std::cout << "Previous board:\n ";
        // prevBoard.printBoard();

        Game::Direction dir = game.findBestMove();
        if (dir == Game::Direction::Left) {
            std::cout << "move left! " << std::endl;
            game.move(Game::Direction::Left);
            game.moveLeft();
        } else if (dir == Game::Direction::Right) {
            std::cout << "move right! " << std::endl;
            game.move(Game::Direction::Right);
            game.moveRight();
        }

        // game.getBoard().print();

        // DEB(prevBoard);
        DEB(board);
        // DEB(game.getBoard());

        if (prevBoard == board /* || board != game.getBoard() */) {
            cv::destroyAllWindows();
            running = false;
            std::cout << "Score: " << game.getScore() << std::endl;
            break;
        }

        prevBoard = board;
        board.setZero();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // }
    }
    return 0;
}