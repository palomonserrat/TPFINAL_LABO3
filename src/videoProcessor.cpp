#include "videoProcessor.hpp"

cv::Mat VideoProcessor::process(const cv::Mat& frame, int mode) {
    cv::Mat gray = toGray(frame);

    switch (mode) {
        case 0:
            return gray;

        case 1:
            return applyBlur(gray);

        case 2:
            return applyGaussian(gray);

        case 3:
            return applySobel(gray);

        case 4:
            return applyLaplacian(gray);

        default:
            return gray;
    }
}

cv::Mat VideoProcessor::toGray(const cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

cv::Mat VideoProcessor::applyBlur(const cv::Mat& gray) {
    cv::Mat output;
    cv::blur(gray, output, cv::Size(5, 5));
    return output;
}

cv::Mat VideoProcessor::applyGaussian(const cv::Mat& gray) {
    cv::Mat output;
    cv::GaussianBlur(gray, output, cv::Size(7, 7), 1.5);
    return output;
}

cv::Mat VideoProcessor::applySobel(const cv::Mat& gray) {
    cv::Mat gradX, gradY;
    cv::Mat absX, absY;
    cv::Mat output;

    cv::Sobel(gray, gradX, CV_16S, 1, 0, 3);
    cv::Sobel(gray, gradY, CV_16S, 0, 1, 3);

    cv::convertScaleAbs(gradX, absX);
    cv::convertScaleAbs(gradY, absY);

    cv::addWeighted(absX, 0.5, absY, 0.5, 0, output);

    return output;
}

cv::Mat VideoProcessor::applyLaplacian(const cv::Mat& gray) {
    cv::Mat lap;
    cv::Mat output;

    cv::Laplacian(gray, lap, CV_16S, 3);
    cv::convertScaleAbs(lap, output);

    return output;
}