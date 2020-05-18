// GOOD FEATURES TO TRACK

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
    // Define the number of corners
    int numCorners = 100;

    // Check if 'numCorners' is positive
    if (numCorners < 1)
    {
        numCorners = 1;
    }

    cv::RNG rng(12345);
    std::string windowName = "Feature points";

    // Current frame
    cv::Mat frame, frameGray;

    char ch;

    // Create the capture object
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr << "Unable to open the webcam. Existing!" << std::endl;
        return -1;
    }

    // Scaling factor to resize the input frames from the webcam
    float scalingFactor = 0.75;

    // Iterate until the user presses the ESC key
    while (true)
    {
        // Capture the current frame
        cap >> frame;

        // Resize the frame
        cv::resize(frame, frame, cv::Size(), scalingFactor, scalingFactor, cv::INTER_AREA);

        // Convert to grayscale
        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

        // Initialize the parameters for Shi-Tomasi algorithm
        std::vector<cv::Point2f> corners;
        double qualityThreshold = 0.02;
        double minDist = 15;
        int blockSize = 5;
        bool useHarrisDetector = false;
        double k = 0.07;

        // Clone the input frame
        cv::Mat frameCopy;
        frameCopy = frame.clone();

        // Apply corner detection
        cv::goodFeaturesToTrack(frameGray, corners, numCorners, qualityThreshold, minDist, cv::Mat(), blockSize, useHarrisDetector, k);

        // Parameters for the circles to display
        int radius = 8;
        int thickness = 2;
        int lineType = 8;

        // Draw the detected corners using circles
        for (size_t i = 0; i < corners.size(); i++)
        {
            cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            cv::circle(frameCopy, corners[i], radius, color, thickness, lineType, 0);
        }

        // Show what you got
        cv::imshow(windowName, frameCopy);

        // Get the keyboard input and check if it's 'ESC'
        ch = cv::waitKey(30);
        if (ch == 27)
            break;
    }

    // Release the video capture object
    cap.release();

    // Close all windows
    cv::destroyAllWindows();

    return 0;
}