// HARRIS CORNERS TRACKER

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
    // Read the input value for the size of block
    int blockSize = 3;
    // std::istringstream iss(argv[1]);
    // iss >> blockSize;

    // Check if 'blockSize' is smaller than 2
    if (blockSize < 2)
    {
        blockSize = 2;
    }

    // Detector parameters
    int apertureSize = 5;
    double k = 0.04;

    int thresh = 200;

    cv::RNG rng(12345);
    std::string windowName = "Harris Corner Detector";

    // Current frame
    cv::Mat frame, frameGray;

    char ch;

    // Create the capture object
    // 0 -> input arg that specifies it should take the input from the webcam
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr << "Unable to open the webcam. Exiting!" << std::endl;
    }

    // Scaling factor to resize the input frames from the webcam
    float scalingFactor = 0.75;

    cv::Mat dst, dst_norm, dst_norm_scaled;

    // Iterate until the user presses the ESC key
    while (true)
    {
        // Capture the current frame
        cap >> frame;

        // Resize the frame
        cv::resize(frame, frame, cv::Size(), scalingFactor, scalingFactor, cv::INTER_AREA);

        dst = cv::Mat::zeros(frame.size(), CV_32FC1);

        // Convert to grayscale
        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

        // Detecting corners
        cv::cornerHarris(frameGray, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);

        // Normalizing
        cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
        cv::convertScaleAbs(dst_norm, dst_norm_scaled);

        // Drawing a circle around corners
        for (int j = 0; j < dst_norm.rows; j++)
        {
            for (int i = 0; i < dst_norm.cols; i++)
            {
                if ((int)dst_norm.at<float>(j, i) > thresh)
                {
                    cv::circle(frame, cv::Point(i, j), 8, cv::Scalar(0, 255, 0), 2, 8, 0);
                }
            }
        }

        // Showing the result
        cv::imshow(windowName, frame);

        // Get the keyboard input and check if it's 'ESC'
        // 27 -> ASCII value of 'ESC' key
        ch = cv::waitKey(10);
        if (ch == 27)
            break;
    }

    // Release the video capture object
    cap.release();

    // Close all windows
    cv::destroyAllWindows();

    return 1;
}
