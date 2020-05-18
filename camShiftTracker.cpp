// CAMSHIFT TRACKER

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"

#include <iostream>
#include <ctype.h>

cv::Mat image;
cv::Point originPoint;
cv::Rect selectedRect;
bool selectRegion = false;
int trackingFlag = 0;

// Function to track the mouse events
void onMouse(int event, int x, int y, int, void*)
{
    if (selectRegion)
    {
        selectedRect.x = MIN(x, originPoint.x);
        selectedRect.y = MIN(y, originPoint.y);
        selectedRect.width = std::abs(x - originPoint.x);
        selectedRect.height = std::abs(y - originPoint.y);

        selectedRect &= cv::Rect(0, 0, image.cols, image.rows);
    }

    switch (event) {
        case cv::EVENT_LBUTTONDOWN:
            originPoint = cv::Point(x, y);
            selectedRect = cv::Rect(x, y, 0, 0);
            selectRegion = true;
            break;

        case cv::EVENT_LBUTTONUP:
            selectRegion = false;
            if (selectedRect.width > 0 && selectedRect.height > 0)
            {
                trackingFlag = -1;
            }
            break;
    }
}

int main(int argc, char* argv[]){
    // Create the capture object
    // 0 -> input arg that specifies it should take the input from the webcam
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr << "Unable to open the webcam. Existing!" << std::endl;
        return -1;
    }

    char ch;
    cv::Rect trackingRect;

    // range of values for the 'H' channel in HSV ('H' stands for Hue)
    float hueRanges[] = {0, 180};
    const float* histRanges = hueRanges;

    // min value for the 'S' channel in HSV
    int minSaturation = 40;

    // min and max values for the 'V' channel in HSV
    int minValue = 20, maxValue = 245;

    // size of the histogram bin
    int histSize = 8;

    std::string windowName = "CAMShift Tracker";
    cv::namedWindow(windowName, 0);
    cv::setMouseCallback(windowName, onMouse, 0);

    cv::Mat frame, hsvImage, hueImage, mask, hist, backproj;

    // Image size scaling factor for the input frames from the webcam
    float scalingFactor = 0.75;

    // Iterate until the user presses the ESC key
    while (true)
    {
        // Capture the current frame
        cap >> frame;

        // Check if 'frame' is empty
        if (frame.empty())
            break;

        // Resize the frame
        cv::resize(frame, frame, cv::Size(), scalingFactor, scalingFactor, cv::INTER_AREA);

        // Clone the input frame
        frame.copyTo(image);

        // Convert to HSV colorspace
        cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

        if (trackingFlag) {
            // Check for all the values in 'hsvimage' that are within the sepcified range
            // and put result in 'mask'
            cv::inRange(hsvImage, cv::Scalar(0, minSaturation, minValue), cv::Scalar(180, 256, maxValue), mask);

            // Mix the specified channels
            int channels[] = {0, 0};
            hueImage.create(hsvImage.size(), hsvImage.depth());
            cv::mixChannels(&hsvImage, 1, &hueImage, 1, channels, 1);

            if (trackingFlag < 0) {
                // Create images based on selected regions of interest
                cv::Mat roi(hueImage, selectedRect), maskroi(mask, selectedRect);

                // Compute the histogram and normalize it
                cv::calcHist(&roi, 1, 0, maskroi, hist, 1, &histSize, &histRanges);
                cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

                trackingRect = selectedRect;
                trackingFlag = 1;
            }

            // Compute the histogram back projection
            cv::calcBackProject(&hueImage, 1, 0, hist, backproj, &histRanges);
            backproj &= mask;
            cv::RotatedRect rotatedTrackingRect = cv::CamShift(backproj, trackingRect, cv::TermCriteria(
                    cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1));

            // Check if the area of trackingRect is too small
            if (trackingRect.area() <= 1) {
                // use an offset value to make sure the tracking Rect has a minimum size
                int cols = backproj.cols, rows = backproj.rows;
                int offset = MIN(rows, cols) + 1;
                trackingRect = cv::Rect(trackingRect.x - offset, trackingRect.y - offset,
                                        trackingRect.x + offset, trackingRect.y + offset)
                               & cv::Rect(0, 0, cols, rows);
            }

            // Draw the ellipse on the top of the image
            cv::ellipse(image, rotatedTrackingRect, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
        }

        // Apply the 'negative' effect on the selected region of interest
        if (selectRegion && selectedRect.width > 0 && selectedRect.height > 0)
        {
            cv::Mat roi(image, selectedRect);
            cv::bitwise_not(roi, roi);
        }

        // Display the output image
            cv::imshow(windowName, image);

        // get the keyboard input and check if it's 'ESC'
        // 27 -> ASCII value of 'ESC' key
        ch = cv::waitKey(30);
        if (ch == 27) {
            break;
        }
    }

    return 0;
}
