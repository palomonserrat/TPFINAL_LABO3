#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "camera.hpp"
#include "videoProcessor.hpp"
#include "metrics.hpp"
#include "frequencyFilters.hpp"


const std::vector<std::string> MODE_NAMES = {
    "Original Gris", "Box Blur Q1.15", "Laplaciano 3x3", "Sharpening 3x3",
    "Pasa-bajos Ideal", "Pasa-altos Ideal", "Pasa-bajos Gaussiano", 
    "Pasa-altos Gaussiano", "Band-pass", "Band-reject", "Espectro Puro"
};

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    Camera camera(0, 640, 480);
    if (!camera.isOpened()) {
        std::cerr << "Error: no se pudo abrir la cámara." << std::endl;
        return -1;
    }

    VideoProcessor processor;
    Metrics metrics;
    FilterConfig config;

    // Ventana de controles separada
    std::string ctrlWin = "Controles";
    cv::namedWindow(ctrlWin, cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Modo (0-10)", ctrlWin, &config.mode, 10);
    cv::createTrackbar("Kernel Size", ctrlWin, &config.kernelSize, 15);
    cv::createTrackbar("Frec. Corte", ctrlWin, &config.cutoffFreq, 150);
    cv::createTrackbar("Banda Baja",  ctrlWin, &config.bandLow, 150);
    cv::createTrackbar("Banda Alta",  ctrlWin, &config.bandHigh, 150);

    // Variables de control para el throttling y limpieza de ventanas
    int frameCounter = 0;
    bool frequencyWindowsActive = false;
    cv::Mat cachedSpectrum;
    cv::Mat cachedMesh3D;

    while (true)
    {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;
        cv::flip(frame, frame, 1);

        // Validaciones rápidas
        if (config.kernelSize % 2 == 0) config.kernelSize++; 
        if (config.kernelSize < 3) config.kernelSize = 3;
        if (config.bandLow >= config.bandHigh) config.bandLow = config.bandHigh - 1;

        // 1. PROCESAMIENTO CRÍTICO (A máxima velocidad)
        metrics.start();
        cv::Mat output = processor.process(frame, config.mode, config);
        metrics.stop();

        // HUD estético sobre la imagen procesada
        cv::Mat overlay; output.copyTo(overlay);
        cv::rectangle(overlay, cv::Rect(0, 0, output.cols, 70), cv::Scalar(0, 0, 0), -1);
        cv::addWeighted(overlay, 0.5, output, 0.5, 0, output);

        std::string statsText = "Lat: " + std::to_string((int)metrics.getFrameTimeMs()) + 
                                "ms | FPS: " + std::to_string((int)metrics.getFPS());
        cv::putText(output, "Modo: " + MODE_NAMES[config.mode], cv::Point(15, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        cv::putText(output, statsText, cv::Point(15, 55), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        cv::putText(output, "Pulsa 'ESC' para SALIR", cv::Point(output.cols - 220, output.rows - 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);

        // Mostrar fuentes en tiempo real
        cv::imshow("Original", frame);
        cv::imshow("Procesado", output);

        // 2. PROCESAMIENTO ANALÍTICO SUBMUESTREADO (Throttling)
        frameCounter++;
        
        if (config.mode >= 4 && config.mode <= 10) 
        {
            // Solo recalculamos las ventanas pesadas cada 10 frames o si están vacías
            if (frameCounter % 10 == 0 || cachedSpectrum.empty() || cachedMesh3D.empty()) 
            {
                if (config.mode == 10) {
                    cv::Mat gray;
                    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
                    cachedSpectrum = FrequencyFilters::spectrumMagnitude(gray);
                } else {
                    cachedSpectrum = FrequencyFilters::spectrumMagnitude(output);
                }
                
                // Generar el render 3D de la manta del filtro
                cachedMesh3D = FrequencyFilters::renderMesh3D(config.mode, config.cutoffFreq, config.bandLow, config.bandHigh);
            }

            // Mostrar ventanas analíticas (hacen refresh visual a aprox. 3-5 FPS, super suave)
            cv::imshow("Espectro de Frecuencia", cachedSpectrum);
            cv::imshow("Manta del Filtro 3D", cachedMesh3D);
            frequencyWindowsActive = true;
        } 
        else 
        {
            // Si pasamos a filtros espaciales, limpiamos las ventanas extras para no estorbar
            if (frequencyWindowsActive) {
                cv::destroyWindow("Espectro de Frecuencia");
                cv::destroyWindow("Manta del Filtro 3D");
                cachedSpectrum.release();
                cachedMesh3D.release();
                frequencyWindowsActive = false;
            }
        }
        

        char key = static_cast<char>(cv::waitKey(1));
        if (key == 'q' || key == 27) break;
    }

    cv::destroyAllWindows();
    return 0;
}