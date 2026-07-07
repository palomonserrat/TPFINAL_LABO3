#include "videoProcessor.hpp"
#include "spatialFilters.hpp"
#include "frequencyFilters.hpp"


cv::Mat VideoProcessor::process(const cv::Mat &frame, int mode, const FilterConfig& config)
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
        return FrequencyFilters::idealLowPass(gray, static_cast<double>(config.cutoffFreq));

    case 5:
        return FrequencyFilters::idealHighPass(gray, static_cast<double>(config.cutoffFreq));

    case 6:
        return FrequencyFilters::gaussianLowPass(gray, static_cast<double>(config.cutoffFreq));

    case 7:
        return FrequencyFilters::gaussianHighPass(gray, static_cast<double>(config.cutoffFreq));

    case 8:
        return FrequencyFilters::bandPass(gray, static_cast<double>(config.bandLow), static_cast<double>(config.bandHigh));

    case 9:
        return FrequencyFilters::bandReject(gray, static_cast<double>(config.bandLow), static_cast<double>(config.bandHigh));

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