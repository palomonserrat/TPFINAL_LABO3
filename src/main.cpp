#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>

#include "camera.hpp"
#include "videoProcessor.hpp"

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    Camera camera(0, 640, 480);

    if (!camera.isOpened()) {
        std::cerr << "Error: no se pudo abrir la cámara." << std::endl;
        return -1;
    }

    VideoProcessor processor;

    int mode = 0;

    std::cout << "Controles:" << std::endl;
    std::cout << "0 - Original gris" << std::endl;
    std::cout << "1 - Box blur 5x5 Q1.15" << std::endl;
    std::cout << "q o ESC - Salir" << std::endl;

    while (true) {
        cv::Mat frame = camera.getFrame();

        if (frame.empty()) {
            std::cerr << "Error: frame vacío." << std::endl;
            break;
        }

        cv::Mat output = processor.process(frame, mode);

        cv::imshow("Original", frame);
        cv::imshow("Procesado", output);

        char key = static_cast<char>(cv::waitKey(1));

        if (key == 'q' || key == 27) {
            break;
        }

        if (key == '0' || key == '1') {
            mode = key - '0';
            std::cout << "Modo cambiado a: " << mode << std::endl;
        }
    }

    cv::destroyAllWindows();

    return 0;
}