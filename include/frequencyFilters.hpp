#pragma once

#include <opencv2/opencv.hpp>

namespace FrequencyFilters {

    cv::Mat idealLowPass(const cv::Mat& gray, int cutoffRadius);
    cv::Mat idealHighPass(const cv::Mat& gray, int cutoffRadius);

    cv::Mat gaussianLowPass(const cv::Mat& gray, double cutoffD0);
    cv::Mat gaussianHighPass(const cv::Mat& gray, double cutoffD0);

    cv::Mat bandPass(const cv::Mat& gray, double D0low, double D0high);
    cv::Mat bandReject(const cv::Mat& gray, double D0low, double D0high);

    // Con fines de visualización
    cv::Mat spectrumMagnitude(const cv::Mat& gray);
}