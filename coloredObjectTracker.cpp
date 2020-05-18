// COLORSPACE-BASED TRACKER

#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

int main (int argc, char** argv) {
    // Create the capture object
    //
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr <<"Unable to open the webcam. Exiting!" << std::endl;
        return -1;
    }

    cv::Mat frame, hsvImage, mask, outputImage;
    char ch;

    // Image size scaling factor for the input frames from the webcam
    float scalingFactor = 0.75;

    // iterate until the user presses the Esc key
    while (true)
    {
        // Initialize the output image before each iteraction
        outputImage = cv::Scalar (0, 0, 0);

        // capture the current frame
        cap >> frame;

        // Check if 'frame' is empty
        cv::resize(frame, frame, cv::Size(), scalingFactor, scalingFactor, cv::INTER_AREA);

        // Convert to HSV colorspace
        cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

        // Define the range of "blue" color in HSV color sapce
        cv::Scalar lowerLimit = cv::Scalar(60, 100, 100);
        cv::Scalar upperLimit = cv::Scalar(180, 255, 255);

        // Threshold the HSV image to get only blue color
        cv::inRange(hsvImage, lowerLimit, upperLimit, mask);

        // Computer bitwise-AND of input image and mask
        cv::bitwise_and(frame, frame, outputImage, mask = mask);

        // Run median filter on the output to smoothen it
        cv::medianBlur(outputImage, outputImage, 5);

        // Display the input and output image
        cv::imshow("Input", frame);
        cv::imshow("Output", outputImage);

        // Get the keyboard input and check if it's 'Esc'
        // 30 -> wait for 30ms
        // 27 -> ASCII value of 'ESC' key
        ch = cv::waitKey(30);
        if (ch == 27) {
            break;
        }
    }

    return 0;
}