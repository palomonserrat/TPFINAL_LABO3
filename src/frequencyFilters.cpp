#include "frequencyFilters.hpp"

#include <cstdint>

namespace FrequencyFilters {

static void fftShift(cv::Mat& complexImage) {
    int cx = complexImage.cols / 2;
    int cy = complexImage.rows / 2;

    cv::Mat q0(complexImage, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(complexImage, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(complexImage, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(complexImage, cv::Rect(cx, cy, cx, cy));

    cv::Mat temp;

    q0.copyTo(temp);
    q3.copyTo(q0);
    temp.copyTo(q3);

    q1.copyTo(temp);
    q2.copyTo(q1);
    temp.copyTo(q2);
}


cv::Mat idealLowPass(const cv::Mat& gray, int cutoffRadius) {
    CV_Assert(gray.type() == CV_8UC1);

    cv::Mat grayFloat;
    gray.convertTo(grayFloat, CV_32F);

    cv::Mat planes[] = {
        grayFloat,
        cv::Mat::zeros(gray.size(), CV_32F)
    };

    cv::Mat complexImage;
    cv::merge(planes, 2, complexImage);

    cv::dft(complexImage, complexImage);

    fftShift(complexImage);

    const int rows = complexImage.rows;
    const int cols = complexImage.cols;

    const int centerY = rows / 2;
    const int centerX = cols / 2;

    const int cutoffSquared = cutoffRadius * cutoffRadius;

    for (int y = 0; y < rows; y++) {
        cv::Vec2f* row = complexImage.ptr<cv::Vec2f>(y);

        for (int x = 0; x < cols; x++) {
            int dy = y - centerY;
            int dx = x - centerX;

            int distanceSquared = dx * dx + dy * dy;

            if (distanceSquared > cutoffSquared) {
                row[x][0] = 0.0f; // Parte real
                row[x][1] = 0.0f; // Parte imaginaria
            }
        }
    }

    fftShift(complexImage);

    cv::Mat filteredFloat;

    cv::idft(
        complexImage,
        filteredFloat,
        cv::DFT_SCALE | cv::DFT_REAL_OUTPUT
    );

    cv::Mat output;
    filteredFloat.convertTo(output, CV_8U);

    return output;
}

}