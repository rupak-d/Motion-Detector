
#include <iostream>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <direct.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>


using namespace std;
using namespace cv;

int max_deviation = 200000;
int theObject[2] = { 0,0 };

int main(int argc, char* argv[])
{
	printf("Hit ESC key to quit ...\n");
	cv::VideoCapture cap(0); // open the default camera
	//cv::VideoCapture cap("testvideo2.avi"); // open the video
	if (!cap.isOpened()) { // check if we succeeded
		printf("error ‐ can't open the input source\n");
		system("PAUSE");
		return EXIT_FAILURE;
	}
	// Let's just see what the image size is from this camera or file.
	double WIDTH
		= cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double HEIGHT
		= cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	printf("Image width=%f, height=%f\n", WIDTH, HEIGHT);

	// Allocate images
	cv::Mat frame1, frame2, frame3; //the three frames we will be comparing
	cv::Mat differenceImage, differenceImage13, differenceImage23; //resulting difference image
	cv::Mat thresholdImage; //thresholded difference image 

	// Run an infinite loop until user hits the ESC key
	while (1) {
		cap >> frame1; // get image from camera
		if (!frame1.data) {
			printf("image is empty\n");
			system("PAUSE");
			break;
		}
		cv::cvtColor(frame1, frame1, cv::COLOR_BGR2GRAY); // Convert to grayscale.
		cap >> frame2; // get next image from camera
		if (!frame2.data) {
			printf("image is empty\n");
			system("PAUSE");
			break;
		}
		cv::cvtColor(frame2, frame2, cv::COLOR_BGR2GRAY); // Convert to grayscale.
		cap >> frame3; // get next image from camera
		if (!frame3.data) {
			printf("image is empty\n");
			system("PAUSE");
			break;
		}
		cv::cvtColor(frame3, frame3, cv::COLOR_BGR2GRAY); // Convert to grayscale.

		// Intensity Image between 1 & 3 and 2 & 3.
		cv::absdiff(frame1, frame3, differenceImage13);
		cv::absdiff(frame2, frame3, differenceImage23);
		//cv::imshow("1 Difference Image", differenceImage13);
		//cv::imshow("2 Difference Image", differenceImage23);

		// Bitwise & for differenceImage13, differenceImage23.
		cv::bitwise_and(differenceImage13, differenceImage23, differenceImage);
		//cv::imshow("Difference Image", differenceImage);

		// Applying Adaptive Thresholding.
		cv::adaptiveThreshold(differenceImage, thresholdImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 5);
		//cv::imshow("Threshold Image", thresholdImage);

		// Blur & Threshold.
		cv::blur(thresholdImage, thresholdImage, cv::Size(8, 8));
		cv::adaptiveThreshold(thresholdImage, thresholdImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 5);
		//cv::imshow("Final Threshold Image", thresholdImage);

		// Standard Deviation
		Scalar mean, stddev;
		meanStdDev(thresholdImage, mean, stddev);
		
		// if not to much changes then the motion is real (neglect agressive snow, temporary sunlight)
		//if (stddev[0] < max_deviation)
		//{
			int number_of_changes = 0;
			//int min_x = thresholdImage.cols, max_x = 0;
			//int min_y = thresholdImage.rows, max_y = 0;
			// loop over image and detect changes
			for (int j = 0; j < HEIGHT; j += 1) { // height
				for (int i = 0; i < WIDTH; i += 1) { // width
					// check if at pixel (j,i) intensity is equal to 255
					// this means that the pixel is different in the sequence of frames
					if ((thresholdImage.at<uchar>(j, i)) = 255)
					{
						theObject[0] = j, theObject[1] = i;
						int x = theObject[0];
						int y = theObject[1];
						circle(frame1, Point(x, y), 5, (0, 255, 255), 2);
						imshow("Motion Detection Frame", frame1);
						//number_of_changes++;
						//if (min_x>i) min_x = i;
						//if (max_x<i) max_x = i;
						//if (min_y>j) min_y = j;
						//if (max_y<j) max_y = j;
					}
					
				}
			}
			//printf("The number of changes = %d", number_of_changes);
			//if (number_of_changes>5) {
				//check if not out of bounds
				//if (min_x - 10 > 0) min_x -= 10;
				//if (min_y - 10 > 0) min_y -= 10;
				//if (max_x + 10 < thresholdImage.cols - 1) max_x += 10;
				//if (max_y + 10 < thresholdImage.rows - 1) max_y += 10;
				// draw rectangle round the changed pixel
				//Point x(min_x, min_y);
				//Point y(max_x, max_y);
				//cv::rectangle(frame1, rect, (255, 150, 0), 1);
				//cv::rectangle(
					//frame1,
					//cv::Point(x),
					//cv::Point(y),
					//cv::Scalar(255, 255, 255)
				//);
				//imshow("Motion Detection Frame", frame1);
			//}
			//return number_of_changes;
		//}
		
		//wait for
		//system("PAUSE");
		if (cv::waitKey(1) == 27)
			break; // ESC is ascii 27
	}

	return EXIT_SUCCESS;
}
