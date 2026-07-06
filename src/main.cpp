#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <string>

#include "camera.hpp"
#include "videoProcessor.hpp"
#include "metrics.hpp"

int main() {
    // Oculta mensajes informativos de OpenCV
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // Inicialización de la cámara
    Camera camera(0, 640, 480);

    if (!camera.isOpened()) {
        std::cerr << "Error: no se pudo abrir la cámara." << std::endl;
        return -1;
    }

    // Inicialización del procesador y métricas
    VideoProcessor processor;
    Metrics metrics;

    int mode = 0;

    std::cout << "Controles:" << std::endl;
    std::cout << "0 - Original gris" << std::endl;
    std::cout << "1 - Box blur 5x5 Q1.15" << std::endl;
    std::cout << "2 - Laplaciano 3x3" << std::endl;
    std::cout << "3 - Sharpening 3x3" << std::endl;
    std::cout << "q o ESC - Salir" << std::endl;

    while (true) {
        // Captura de frame
        cv::Mat frame = camera.getFrame();

        if (frame.empty()) {
            std::cerr << "Error: frame vacío." << std::endl;
            break;
        }

        // Espejar cámara horizontalmente
        cv::flip(frame, frame, 1);

        // Medimos el tiempo de procesamiento
        metrics.start();

        cv::Mat output = processor.process(frame, mode);

        metrics.stop();

        // Cálculo de métricas
        double frameTimeMs = metrics.getFrameTimeMs();
        double fps = metrics.getFPS();
        double throughput = metrics.getThroughputMPixelsPerSec(
            output.cols,
            output.rows
        );

        // Texto a mostrar sobre la imagen procesada
        std::string text =
            "Mode: " + std::to_string(mode) +
            " | Lat: " + std::to_string(frameTimeMs) + " ms" +
            " | FPS: " + std::to_string(fps) +
            " | MPix/s: " + std::to_string(throughput);

        cv::putText(
            output,
            text,
            cv::Point(20, 30),
            cv::FONT_HERSHEY_SIMPLEX,
            0.6,
            cv::Scalar(255),
            2
        );

        // Mostrar ventanas
        cv::imshow("Original", frame);
        cv::imshow("Procesado", output);

        // Leer teclado
        char key = static_cast<char>(cv::waitKey(1));

        if (key == 'q' || key == 27) { // 27 = ESC
            break;
        }

        // Cambio de modo
        if (key >= '0' && key <= '3') {
            mode = key - '0';
            std::cout << "Modo cambiado a: " << mode << std::endl;
        }
    }

    cv::destroyAllWindows();

    return 0;
}