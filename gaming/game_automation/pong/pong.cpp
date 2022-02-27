#include <cstdint>
#include <cstring>
#include <vector>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace cv;

void ImageFromDisplay(std::vector<uint8_t> & Pixels, const int & Width, const int & Height, int & BitsPerPixel)
{
	Display *display = XOpenDisplay(nullptr);
	Window root = DefaultRootWindow(display);

	XWindowAttributes attributes = {0};
	XGetWindowAttributes(display, root, &attributes);

	// Width = attributes.width;
	// Height = attributes.height;

	// Width = 900;
	// Height = 430;

	std::cout << "height = " << Height << ", width = " << Width << std::endl;

	XImage *img = XGetImage(display, root, 3850, 150, Width, Height, AllPlanes, ZPixmap);
	BitsPerPixel = img->bits_per_pixel;
	Pixels.clear();
	Pixels.resize(Width * Height * 4);

	memcpy(&Pixels[0], img->data, Pixels.size());

	XDestroyImage(img);
	XCloseDisplay(display);
}

int main()
{
	int Width = 900;
	int Height = 430;
	int Bpp = 0;
	std::vector<std::uint8_t> Pixels;

	bool running = true;

	while (running) {
		ImageFromDisplay(Pixels, Width, Height, Bpp);

		if (Width && Height) {
			// Mat(Size(Height, Width), Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);
			Mat img = Mat(Height, Width, Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]); 

			// namedWindow("WindowTitle", WINDOW_AUTOSIZE);
			imshow("Display window", img);

			// waitKey(0);
		}

		if((char)cv::waitKey(25) == 'q'){
			cv::destroyAllWindows();
			running = false;
			break;
		}
	}
	return 0;
}
