#ifndef VIDEO_PROCESSOR_HPP
#define VIDEO_PROCESSOR_HPP

#include <opencv2/opencv.hpp>

struct FilterConfig {
    int mode = 0;
    int kernelSize = 5;
    int cutoffFreq = 50;
    int bandLow = 20;
    int bandHigh = 60;
};

class VideoProcessor {
public:
    cv::Mat process(const cv::Mat &frame, int mode, const FilterConfig &config);

private:
    cv::Mat toGray(const cv::Mat &frame);
};

#endif // VIDEO_PROCESSOR_HPP