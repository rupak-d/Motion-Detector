#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;


int main(int argc, char* argv[])
{
	printf("Hit ESC key to quit ...\n");
	//VideoCapture cap(0); // open the default camera
	VideoCapture cap("bouncingBall.avi"); // 20
	//VideoCapture cap("Fedex.avi"); // 200
	//VideoCapture cap("Surveillance video.avi"); // 6000
	if (!cap.isOpened()) { // check if we succeeded
		printf("error ‐ can't open the input source\n");
		system("PAUSE");
		return EXIT_FAILURE;
	}

	// Allocate images
	Mat frame1, frame2, frame3; //the three frames we will be comparing
	Mat grayImage1, grayImage2, grayImage3; //their grayscale images (needed for absdiff() function)
	Mat differenceImage, differenceImage13, differenceImage23; //resulting difference image
	Mat thresholdImage; //thresholded difference image (for use in findContours() function)

	// Run an infinite loop until user hits the ESC key
	while (1) {
		cap >> frame1; // get first frame from camera
		if (!frame1.data) {
			printf("image is empty\n");
			system("PAUSE");
			break;
		}
		cvtColor(frame1, grayImage1, COLOR_BGR2GRAY); // Convert to grayscale.
		cap >> frame2; // get second frame from camera
		if (!frame2.data) {
			printf("image is empty\n");
			system("PAUSE");
			break;
		}
		cvtColor(frame2, grayImage2, COLOR_BGR2GRAY); // Convert to grayscale.
		cap >> frame3; // get third frame from camera
		if (!frame3.data) {
			printf("image is empty\n");
			system("PAUSE");
			break;
		}
		cvtColor(frame3, grayImage3, COLOR_BGR2GRAY); // Convert to grayscale.

		// Intensity Image between 1 & 3 and 2 & 3
		absdiff(grayImage1, grayImage3, differenceImage13);
		absdiff(grayImage2, grayImage3, differenceImage23);
		//imshow("1 Difference Image", differenceImage13);
		//imshow("2 Difference Image", differenceImage23);

		// differenceImage13 && differenceImage23.
		bitwise_and(differenceImage13, differenceImage23, differenceImage);
		//imshow("Difference Image", differenceImage);

		// Applying Adaptive Thresholding
		adaptiveThreshold(differenceImage, thresholdImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 7, 5);
		//imshow("Threshold Image", thresholdImage);

		//// Apply morphological operations to get rid of noise & for dilation
		Mat structuringElmtC = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));
		morphologyEx(thresholdImage, thresholdImage, MORPH_CLOSE, structuringElmtC);
		dilate(thresholdImage, thresholdImage, structuringElmtC, Point(-1, -1), 1, 1);
		//imshow("Binary image after morph", thresholdImage);

		
		// Calculate standard deviation of the image
		// Helps avoid detection due to wind or lighting changes.
		Scalar mean, stddev;
		meanStdDev(thresholdImage, mean, stddev);

		Mat temp;
		thresholdImage.copyTo(temp);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		// Retrieves all external contours
		findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); 

		// Approximate contours to polygons
		vector<vector<Point> > contours_poly(contours.size());
		vector<Rect> boundRect(contours.size());
		
		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
			boundRect[i] = boundingRect(Mat(contours_poly[i]));
		}


		// Draw bounding rectangles
		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(0, 0, 255);
			if (contourArea(contours[i], false)>20 && stddev[0]<80) // Applying min area & std dev thresholds.
			{
				rectangle(frame1, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
			}
			
		}

		imshow("Frame1", frame1);
		//wait for
		//system("PAUSE");
		if (waitKey(1) == 27)
			break; // ESC is ascii 27
	}
	return EXIT_SUCCESS;
}