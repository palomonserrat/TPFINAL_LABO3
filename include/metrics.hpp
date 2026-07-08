#pragma once

#include <chrono>

class Metrics {
public:
    void start();
    void stop();

    void resetAverage();

    double getFrameTimeMs() const;
    double getFPS() const;
    double getThroughputMPixelsPerSec(int width, int height) const;

    double getAverageFrameTimeMs() const;
    double getAverageFPS() const;
    double getAverageThroughputMPixelsPerSec(int width, int height) const;

    int getMeasuredFrames() const;

private:
    std::chrono::high_resolution_clock::time_point t0;

    double frameTimeMs = 0.0;

    double accumulatedTimeMs = 0.0;
    int frameCount = 0;
};