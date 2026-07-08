#include "metrics.hpp"

void Metrics::start() {
    t0 = std::chrono::high_resolution_clock::now();
}

void Metrics::stop() {
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = t1 - t0;

    frameTimeMs = elapsed.count();

    accumulatedTimeMs += frameTimeMs;
    frameCount++;
}

void Metrics::resetAverage() {
    accumulatedTimeMs = 0.0;
    frameCount = 0;
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

double Metrics::getAverageFrameTimeMs() const {
    if (frameCount == 0) {
        return 0.0;
    }

    return accumulatedTimeMs / static_cast<double>(frameCount);
}

double Metrics::getAverageFPS() const {
    double averageFrameTimeMs = getAverageFrameTimeMs();

    if (averageFrameTimeMs <= 0.0) {
        return 0.0;
    }

    return 1000.0 / averageFrameTimeMs;
}

double Metrics::getAverageThroughputMPixelsPerSec(int width, int height) const {
    double averageFrameTimeMs = getAverageFrameTimeMs();

    if (averageFrameTimeMs <= 0.0) {
        return 0.0;
    }

    double pixels = static_cast<double>(width) * static_cast<double>(height);
    double seconds = averageFrameTimeMs / 1000.0;

    return pixels / seconds / 1e6;
}

int Metrics::getMeasuredFrames() const {
    return frameCount;
}