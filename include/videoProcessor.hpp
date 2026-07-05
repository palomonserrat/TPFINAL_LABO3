#pragma once

#include <opencv2/opencv.hpp>

class VideoProcessor {
public:
    cv::Mat process(const cv::Mat& frame, int mode);

private:
    cv::Mat toGray(const cv::Mat& frame);
};