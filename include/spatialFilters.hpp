#pragma once

#include <opencv2/opencv.hpp>

namespace SpatialFilters {
    cv::Mat boxBlurQ15(const cv::Mat& gray);
}