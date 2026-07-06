#include "videoProcessor.hpp"
#include "spatialFilters.hpp"
#include "frequencyFilters.hpp"

cv::Mat VideoProcessor::process(const cv::Mat &frame, int mode)
{
    cv::Mat gray = toGray(frame);

    switch (mode)
    {
    case 0:
        return gray;

    case 1:
        return SpatialFilters::boxBlurQ15(gray);

    case 2:
        return SpatialFilters::laplacian3x3(gray);

    case 3:
        return SpatialFilters::sharpening3x3(gray);

    case 4:
        return FrequencyFilters::idealLowPass(gray, 50);

    case 5:
        return FrequencyFilters::idealHighPass(gray, 50);

    case 6:
        return FrequencyFilters::gaussianLowPass(gray, 30.0);

    case 7:
        return FrequencyFilters::gaussianHighPass(gray, 30.0);

    case 8:
        return FrequencyFilters::bandPass(gray, 20.0, 60.0);

    case 9:
        return FrequencyFilters::bandReject(gray, 20.0, 60.0);

    default:
        return gray;
    }
}

cv::Mat VideoProcessor::toGray(const cv::Mat &frame)
{
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    return gray;
}