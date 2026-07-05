#pragma once

#include <chrono>

class Metrics {
public:
    void start();
    void stop();

    double getFrameTimeMs() const;
    double getFPS() const;
    double getThroughputMPixelsPerSec(int width, int height) const;

private:
    std::chrono::high_resolution_clock::time_point t0;
    double frameTimeMs = 0.0;
};