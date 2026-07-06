#pragma once

#include <opencv2/opencv.hpp>

namespace FrequencyFilters {
    cv::Mat idealLowPass(const cv::Mat& gray, int cutoffRadius);
}