#include "metrics.hpp"

void Metrics::start() {
    t0 = std::chrono::high_resolution_clock::now();
}

void Metrics::stop() {
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = t1 - t0;

    frameTimeMs = elapsed.count();
}

double Metrics::getFrameTimeMs() const {
    return frameTimeMs;
}

double Metrics::getFPS() const {
    if (frameTimeMs <= 0.0) {
        return 0.0;
    }

    return 1000.0 / frameTimeMs;
}

double Metrics::getThroughputMPixelsPerSec(int width, int height) const {
    if (frameTimeMs <= 0.0) {
        return 0.0;
    }

    double pixels = static_cast<double>(width) * static_cast<double>(height);
    double seconds = frameTimeMs / 1000.0;

    return pixels / seconds / 1e6;
}