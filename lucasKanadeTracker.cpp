// LUCAS KANADE TRACKER

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

bool pointTrackingFlag = false;
cv::Point2f currentPoint;

// Function to detect mouse events
void onMouse(int event, int x, int y, int, void*)
{
    // Detect the mouse button down event
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        // Assign the current (x, y) position to currentPoint
        currentPoint = cv::Point2f((float)x, (float)y);

        // Set the tracking flag
        pointTrackingFlag = true;
    }
}

int main (int argc, char* argv[])
{
    // Create the capture object
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr << "Unable to open the webcam. Exiting!" << std::endl;
        return -1;
    }

    // Termination criteria for tracking points
    cv::TermCriteria terminationCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 10, 0.02);

    // Size of the block that is used for matching
    cv::Size windowSize(25, 25);

    // Maximum number of points that you want to track
    const int maxNumPoints = 200;

    std::string windowName = "Lucas Kanade Tracker";
    cv::namedWindow(windowName, 1);
    cv::setMouseCallback(windowName, onMouse, 0);

    cv::Mat prevGrayImage, curGrayImage, image, frame;
    std::vector<cv::Point2f> trackingPoints[2];

    // Image size scaling factor for the input frames
    float scalingFactor = 0.75;

    // Iterate until the user hits the ECS key
    while (true)
    {
        // Capture the current frame
        cap >> frame;

        // Check if the frame is empty
        if (frame.empty())
            break;

        // Resize the frame
        cv::resize(frame, frame, cv::Size(), scalingFactor, scalingFactor, cv::INTER_AREA);

        // Copy the input frame
        frame.copyTo(image);

        // Convert the image to grayscale
        cv::cvtColor(image, curGrayImage, cv::COLOR_BGR2GRAY);

        // Check if there are points to track
        if (!trackingPoints[0].empty())
        {
            // Status vector to indecate whether the flow for the corresponding features has been found
            std::vector<uchar> statusVector;

            // Error vector to indicate the error for the corresponding feature
            std::vector<float> errorVector;

            // Check if previous image is empty
            if (prevGrayImage.empty())
            {
                curGrayImage.copyTo(prevGrayImage);
            }

            // Calculate the optical flow using Lucas-Kanade algorithm
            cv::calcOpticalFlowPyrLK(prevGrayImage, curGrayImage, trackingPoints[0], trackingPoints[1],
                    statusVector, errorVector, windowSize, 3, terminationCriteria, 0, 0.001);

            int count = 0;

            // Minimum distance between any two tracking points
            int minDist = 7;

            for (int i = 0; i < trackingPoints[1].size(); i++)
            {
                if (pointTrackingFlag)
                {
                    // If the new point is within 'minDist' distance from an existing point, it will not be tracked
                    if (cv::norm(currentPoint - trackingPoints[1][i]) <= minDist)
                    {
                        pointTrackingFlag = false;
                        continue;
                    }
                }

                // Check if the status vector is good
                if (!statusVector[i])
                    continue;

                trackingPoints[1][count++] = trackingPoints[1][i];

                // Draw a filled circle for each of the tracking points
                int radius = 8;
                int thickness = 2;
                int lineType = 8;
                cv::circle(image, trackingPoints[1][i], radius, cv::Scalar(0, 255, 0), thickness, lineType);
            }
            trackingPoints[1].resize(count);
        }

        // Refining the location of the feature points
        if (pointTrackingFlag && trackingPoints[1].size() < maxNumPoints)
        {
            std::vector<cv::Point2f> tempPoints;
            tempPoints.push_back(currentPoint);

            // Function to refine the location of the corners to subpixel accuracy.
            // Here, 'pixel' refers to the image patch of size 'windowSize' and not the actual image pixel
            cv::cornerSubPix(curGrayImage, tempPoints, windowSize, cv::Size(-1, -1), terminationCriteria);

            trackingPoints[1].push_back(tempPoints[0]);
            pointTrackingFlag = false;
        }

        // Display the image with the tracking points
        cv::imshow(windowName, image);

        // Check if the user pressed the ECS key
        char ch = cv::waitKey(10);
        if (ch == 27)
            break;

        // Swap the 'points' vectors to update 'previous' to 'current'
        std::swap(trackingPoints[1], trackingPoints[0]);

        // Swap the image to update the previous image to current image
        cv::swap(prevGrayImage, curGrayImage);
    }

    return 0;
}
