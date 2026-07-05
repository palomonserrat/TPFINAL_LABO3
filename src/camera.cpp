#include "camera.hpp"

Camera::Camera(int cameraIndex, int width, int height) {
    cap.open(cameraIndex, cv::CAP_DSHOW);

    if (cap.isOpened()) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    }
}

bool Camera::isOpened() const {
    return cap.isOpened();
}

cv::Mat Camera::getFrame() {
    cv::Mat frame;
    cap >> frame;
    return frame;
}