#include "camera.hpp"

Camera::Camera(int cameraIndex, int width, int height) {
#ifdef _WIN32
    // DirectShow sólo existe en Windows. En otras plataformas usamos
    // el backend por defecto de OpenCV (V4L2 en Linux, AVFoundation en macOS).
    cap.open(cameraIndex, cv::CAP_DSHOW);
#else
    cap.open(cameraIndex);
#endif

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