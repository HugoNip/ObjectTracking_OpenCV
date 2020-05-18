#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

// Function to compute the optical flow map
void drawOpticalFlow(const cv::Mat& flowImage, cv::Mat& flowImageGray)
{
    int stepSize = 16;
    cv::Scalar color = cv::Scalar(0, 255, 0);

    // Draw the uniform grid of points on the input image along with the motion vectors
    for(int y = 0; y < flowImageGray.rows; y += stepSize)
    {
        for(int x = 0; x < flowImageGray.cols; x += stepSize)
        {
            // Circles to indicate the uniform grid of points
            int radius = 2;
            int thickness = -1;
            circle(flowImageGray, cv::Point(x,y), radius, color, thickness);

            // Lines to indicate the motion vectors
            cv::Point2f pt = flowImage.at<cv::Point2f>(y, x);
            line(flowImageGray, cv::Point(x,y), cv::Point(cvRound(x+pt.x), cvRound(y+pt.y)), color);
        }
    }
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened()){
        //error in opening the video input
        std::cerr << "Unable to open file!" << std::endl;
        return 0;
    }

    cv::Mat frame1, prvsGray;
    std::string windowName = "Optical Flow";
    cap >> frame1;
    cvtColor(frame1, prvsGray, cv::COLOR_BGR2GRAY);

    while(true){
        cv::Mat frame2, curGray;
        cap >> frame2;
        if (frame2.empty())
            break;
        cvtColor(frame2, curGray, cv::COLOR_BGR2GRAY);

        cv::Mat flowImage(prvsGray.size(), CV_32FC2);
        cv::Mat flowImageGray(prvsGray.size(), CV_32FC2);

        calcOpticalFlowFarneback(prvsGray, curGray, flowImage, 0.5, 3, 15, 3, 5, 1.2, 0);

        // Convert to 3-channel RGB
        cvtColor(prvsGray, flowImageGray, cv::COLOR_GRAY2BGR);

        // Draw the optical flow map
        drawOpticalFlow(flowImage, flowImageGray);

        // Display the output image
        imshow(windowName, flowImageGray);

        // Break out of the loop if the user presses the Esc key
        int keyboard = cv::waitKey(30);
        if (keyboard == 27)
            break;

        std::swap(prvsGray, curGray);
    }
}