#pragma once

#include <opencv2/opencv.hpp>

class Camera {
public:
    Camera(int cameraIndex, int width, int height);

    bool isOpened() const;
    cv::Mat getFrame();

private:
    cv::VideoCapture cap;
};