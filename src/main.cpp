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
    cv::createTrackbar("Frec. Corte", ctrlWin, &config.cutoffFreq, 150);
    cv::createTrackbar("Banda Baja",  ctrlWin, &config.bandLow, 150);
    cv::createTrackbar("Banda Alta",  ctrlWin, &config.bandHigh, 150);

    // Variables de control para el throttling y limpieza de ventanas
    int frameCounter = 0;
    bool frequencyWindowsActive = false;
    cv::Mat cachedSpectrum;
    cv::Mat cachedMesh3D;

    // Variables para detectar cambios de modo/parámetros
    int lastMode = config.mode;
    int lastCutoffFreq = config.cutoffFreq;
    int lastBandLow = config.bandLow;
    int lastBandHigh = config.bandHigh;

    while (true)
    {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;
        cv::flip(frame, frame, 1);

    
        if (config.bandLow >= config.bandHigh) config.bandLow = config.bandHigh - 1;

        // Si cambia el modo o algún parámetro, reiniciamos el promedio
        bool configChanged =
            config.mode != lastMode ||
            config.cutoffFreq != lastCutoffFreq ||
            config.bandLow != lastBandLow ||
            config.bandHigh != lastBandHigh;

        if (configChanged) {
            metrics.resetAverage();

            lastMode = config.mode;
            lastCutoffFreq = config.cutoffFreq;
            lastBandLow = config.bandLow;
            lastBandHigh = config.bandHigh;

            cachedSpectrum.release();
            cachedMesh3D.release();
            frameCounter = 0;

            std::cout << "Nueva medicion - Modo: "
                      << MODE_NAMES[config.mode]
                      << std::endl;
        }

        // 1. PROCESAMIENTO CRÍTICO
        // Se mide únicamente el tiempo del procesamiento DSP.
        metrics.start();

        cv::Mat output = processor.process(frame, config.mode, config);

        metrics.stop();

        // Métricas instantáneas
        double frameTimeMs = metrics.getFrameTimeMs();
        double fps = metrics.getFPS();

        // Métricas promedio
        double avgFrameTimeMs = metrics.getAverageFrameTimeMs();
        double avgFPS = metrics.getAverageFPS();
        double avgThroughput = metrics.getAverageThroughputMPixelsPerSec(
            output.cols,
            output.rows
        );

        int measuredFrames = metrics.getMeasuredFrames();

        // HUD estético sobre la imagen procesada
        cv::Mat overlay;
        output.copyTo(overlay);

        cv::rectangle(
            overlay,
            cv::Rect(0, 0, output.cols, 90),
            cv::Scalar(0, 0, 0),
            -1
        );

        cv::addWeighted(overlay, 0.5, output, 0.5, 0, output);

        std::string modeText =
            "Modo: " + MODE_NAMES[config.mode];

        std::string instantStatsText =
            "Inst | Lat: " + std::to_string(static_cast<int>(frameTimeMs)) +
            " ms | FPS: " + std::to_string(static_cast<int>(fps));

        std::string averageStatsText =
            "Avg  | N: " + std::to_string(measuredFrames) +
            " | Lat: " + std::to_string(avgFrameTimeMs).substr(0, 5) +
            " ms | FPS: " + std::to_string(avgFPS).substr(0, 5) +
            " | MPix/s: " + std::to_string(avgThroughput).substr(0, 5);

        cv::putText(
            output,
            modeText,
            cv::Point(15, 25),
            cv::FONT_HERSHEY_SIMPLEX,
            0.65,
            cv::Scalar(255, 255, 255),
            2
        );

        cv::putText(
            output,
            instantStatsText,
            cv::Point(15, 50),
            cv::FONT_HERSHEY_SIMPLEX,
            0.45,
            cv::Scalar(255, 255, 255),
            1
        );

        cv::putText(
            output,
            averageStatsText,
            cv::Point(15, 75),
            cv::FONT_HERSHEY_SIMPLEX,
            0.45,
            cv::Scalar(255, 255, 255),
            1
        );

        cv::putText(
            output,
            "Pulsa 'ESC' para SALIR",
            cv::Point(output.cols - 220, output.rows - 15),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(0, 0, 255),
            2
        );

        // Mostrar fuentes en tiempo real
        cv::imshow("Original", frame);
        cv::imshow("Procesado", output);

        // 2. PROCESAMIENTO ANALÍTICO SUBMUESTREADO
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
                cachedMesh3D = FrequencyFilters::renderMesh3D(
                    config.mode,
                    config.cutoffFreq,
                    config.bandLow,
                    config.bandHigh
                );
            }

            cv::imshow("Espectro de Frecuencia", cachedSpectrum);
            cv::imshow("Manta del Filtro 3D", cachedMesh3D);
            frequencyWindowsActive = true;
        }
        else
        {
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