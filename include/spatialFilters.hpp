#pragma once

#include <opencv2/opencv.hpp>

namespace SpatialFilters {
    cv::Mat boxBlurQ15(const cv::Mat& gray);
    cv::Mat laplacian3x3(const cv::Mat& gray);
    cv::Mat sharpening3x3(const cv::Mat& gray);
}