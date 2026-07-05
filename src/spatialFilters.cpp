#include "spatialFilters.hpp"

#include <algorithm>
#include <cstdint>

namespace SpatialFilters {

cv::Mat boxBlurQ15(const cv::Mat& gray) {
    CV_Assert(gray.type() == CV_8UC1);

    cv::Mat output(gray.size(), CV_8UC1);

    constexpr int KERNEL_SIZE = 5;
    constexpr int RADIUS = KERNEL_SIZE / 2;
    constexpr int Q = 15;
    constexpr int ROUNDING = 1 << (Q - 1);

    // Kernel promedio 5x5 en Q1.15 corregido en DC.
    // Suma = 32768 = 2^15
    constexpr int16_t h[KERNEL_SIZE][KERNEL_SIZE] = {
        {1311, 1311, 1311, 1311, 1311},
        {1311, 1311, 1311, 1311, 1311},
        {1311, 1311, 1304, 1311, 1311},
        {1311, 1311, 1311, 1311, 1311},
        {1311, 1311, 1311, 1311, 1311}
    };

    for (int y = 0; y < gray.rows; y++) {
        for (int x = 0; x < gray.cols; x++) {

            int32_t acc = 0;

            for (int ky = -RADIUS; ky <= RADIUS; ky++) {
                for (int kx = -RADIUS; kx <= RADIUS; kx++) {

                    int yy = std::clamp(y + ky, 0, gray.rows - 1);
                    int xx = std::clamp(x + kx, 0, gray.cols - 1);

                    uint8_t pixel = gray.at<uint8_t>(yy, xx);
                    int16_t coeff = h[ky + RADIUS][kx + RADIUS];

                    acc += static_cast<int32_t>(pixel) * static_cast<int32_t>(coeff);
                }
            }

            // Pasar de Q1.15 a entero de 8 bits con redondeo
            int32_t value = (acc + ROUNDING) >> Q;

            value = std::clamp(value, 0, 255);

            output.at<uint8_t>(y, x) = static_cast<uint8_t>(value);
        }
    }

    return output;
}

}