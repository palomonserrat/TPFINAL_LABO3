#include "videoProcessor.hpp"
#include "spatialFilters.hpp"

cv::Mat VideoProcessor::process(const cv::Mat& frame, int mode) {
    cv::Mat gray = toGray(frame);

    switch (mode) {
        case 0:
            return gray;

        case 1:
            return SpatialFilters::boxBlurQ15(gray);

        default:
            return gray;
    }
}

cv::Mat VideoProcessor::toGray(const cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    return gray;
}