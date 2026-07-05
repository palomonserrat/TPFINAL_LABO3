#include "spatialFilters.hpp"

#include <algorithm>
#include <cstdint>

namespace SpatialFilters {

cv::Mat boxBlurQ15(const cv::Mat& gray) {
    CV_Assert(gray.type() == CV_8UC1);

    cv::Mat output(gray.size(), CV_8UC1);

    constexpr int RADIUS = 2;
    constexpr int Q = 15;
    constexpr int ROUNDING = 1 << (Q - 1);

    constexpr int COEFF_BASE = 1311;
    constexpr int CENTER_CORRECTION = 7;

    const int rows = gray.rows;
    const int cols = gray.cols;

    for (int y = 0; y < rows; y++) {
        uint8_t* outRow = output.ptr<uint8_t>(y);

        for (int x = 0; x < cols; x++) {
            int sum = 0;

            for (int ky = -RADIUS; ky <= RADIUS; ky++) {
                int yy = y + ky;

                if (yy < 0) {
                    yy = 0;
                } else if (yy >= rows) {
                    yy = rows - 1;
                }

                const uint8_t* inRow = gray.ptr<uint8_t>(yy);

                for (int kx = -RADIUS; kx <= RADIUS; kx++) {
                    int xx = x + kx;

                    if (xx < 0) {
                        xx = 0;
                    } else if (xx >= cols) {
                        xx = cols - 1;
                    }

                    sum += inRow[xx];
                }
            }

            int center = gray.ptr<uint8_t>(y)[x];

            int32_t acc = COEFF_BASE * sum - CENTER_CORRECTION * center;

            int32_t value = (acc + ROUNDING) >> Q;

            if (value < 0) {
                value = 0;
            } else if (value > 255) {
                value = 255;
            }

            outRow[x] = static_cast<uint8_t>(value);
        }
    }

    return output;
}

}