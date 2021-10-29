#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(int argc, char ** argv)
{
	// Read the image file
	cv::Mat image = imread("/home/mihairobescu/Downloads/pexels-baskin-creative-studios-1766838.jpg");
	// Check for failure
	if (image.empty()) {
		std::cout << "Could not open or find the image" << std::endl;
		std::cin.get(); // wait for any key press
		return -1;
	}

	String windowName = "The Guitar"; // Name of the window

	namedWindow(windowName); // Create a window

	imshow(windowName, image); // Show our image inside the created window.

	waitKey(0); // Wait for any keystroke in the window

	destroyWindow(windowName); // destroy the created window

	return 0;
}