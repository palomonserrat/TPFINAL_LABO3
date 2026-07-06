#include "frequencyFilters.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>

namespace FrequencyFilters
{

    enum class FilterType
    {
        IdealLow,
        IdealHigh,
        GaussLow,
        GaussHigh,
        BandPass,
        BandReject
    };

    class SuperChargedFilterEngine
    {
    private:
        int optRows = 0, optCols = 0;
        cv::Mat precomputedMask; // Ahora es de 1 SOLO CANAL (CV_32FC1)

        // Buffers estáticos de 1 solo canal: Memoria congelada y optimizada
        cv::Mat padded;
        cv::Mat dftBuffer;
        cv::Mat idftOutput;
        cv::Mat grayFloat;

    public:
        void updateMask(cv::Size size, FilterType type, double p1, double p2 = 0.0)
        {
            optRows = cv::getOptimalDFTSize(size.height);
            optCols = cv::getOptimalDFTSize(size.width);

            padded.create(optRows, optCols, CV_32F);
            dftBuffer.create(optRows, optCols, CV_32F);
            idftOutput.create(optRows, optCols, CV_32F);
            precomputedMask.create(optRows, optCols, CV_32F);

            
            // Elevamos los radios al cuadrado
            float p1Sq = static_cast<float>(p1 * p1);
            float p2Sq = static_cast<float>(p2 * p2);

            // En lugar de dividir por (2 * p^2) adentro, multiplicamos por (-1 / (2 * p^2))
            float invDenom1 = (p1 > 0.0) ? -1.0f / (2.0f * p1Sq) : 0.0f;
            float invDenom2 = (p2 > 0.0) ? -1.0f / (2.0f * p2Sq) : 0.0f;

            for (int y = 0; y < optRows; y++)
            {
                int v = (y <= optRows / 2) ? y : (optRows - y);
                int vSq = v * v; // Elevamos 'v' al cuadrado una sola vez por fila

                float *m = precomputedMask.ptr<float>(y);

                for (int x = 0; x < optCols; x++)
                {
                    int u = (x <= optCols / 2) ? x : (optCols - x);

                    //Distancia al cuadrado puramente entera. Sin sqrt.
                    int distSq = u * u + vSq;

                    switch (type)
                    {
                    case FilterType::IdealLow:
                        m[x] = (distSq <= p1Sq) ? 1.0f : 0.0f;
                        break;
                    case FilterType::IdealHigh:
                        m[x] = (distSq <= p1Sq) ? 0.0f : 1.0f;
                        break;
                    case FilterType::GaussLow:
                        m[x] = std::exp(distSq * invDenom1);
                        break;
                    case FilterType::GaussHigh:
                        m[x] = 1.0f - std::exp(distSq * invDenom1);
                        break;
                    case FilterType::BandPass:
                    {
                        float gHigh = std::exp(distSq * invDenom2);
                        float gLow = std::exp(distSq * invDenom1);
                        m[x] = gHigh - gLow;
                        break;
                    }
                    case FilterType::BandReject:
                    {
                        float gHigh = std::exp(distSq * invDenom2);
                        float gLow = std::exp(distSq * invDenom1);
                        m[x] = 1.0f - (gHigh - gLow);
                        break;
                    }
                    }
                }
            }
        }

        cv::Mat run(const cv::Mat &gray)
        {
            gray.convertTo(grayFloat, CV_32F);

            cv::copyMakeBorder(grayFloat, padded, 0, optRows - gray.rows, 0, optCols - gray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

            // DFT Real-to-Real (CCS Packed): Devuelve 1 solo canal.
            // Se ejecuta en la mitad del tiempo porque explota la simetría internamente.
            cv::dft(padded, dftBuffer, 0);

            // Multiplicación escalar directa elemento a elemento (1 canal contra 1 canal)
            // Es infinitamente más rápida que una multiplicación compleja de matrices.
            cv::multiply(dftBuffer, precomputedMask, dftBuffer);

            // IDFT Real-to-Real: Reconstruye la imagen usando el empaquetado simétrico
            cv::idft(dftBuffer, idftOutput, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

            cv::Mat cropped = idftOutput(cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Mat output;
            cropped.convertTo(output, CV_8U);
            return output;
        }
    };

    // =====================================================================
    // API Pública
    // =====================================================================

    cv::Mat idealLowPass(const cv::Mat &gray, int cutoffRadius)
    {
        static SuperChargedFilterEngine engine;
        static cv::Size lastSize(0, 0);
        static int lastCutoff = -1;
        if (gray.size() != lastSize || cutoffRadius != lastCutoff)
        {
            engine.updateMask(gray.size(), FilterType::IdealLow, cutoffRadius);
            lastSize = gray.size();
            lastCutoff = cutoffRadius;
        }
        return engine.run(gray);
    }

    cv::Mat idealHighPass(const cv::Mat &gray, int cutoffRadius)
    {
        static SuperChargedFilterEngine engine;
        static cv::Size lastSize(0, 0);
        static int lastCutoff = -1;
        if (gray.size() != lastSize || cutoffRadius != lastCutoff)
        {
            engine.updateMask(gray.size(), FilterType::IdealHigh, cutoffRadius);
            lastSize = gray.size();
            lastCutoff = cutoffRadius;
        }
        return engine.run(gray);
    }

    cv::Mat gaussianLowPass(const cv::Mat &gray, double cutoffD0)
    {
        static SuperChargedFilterEngine engine;
        static cv::Size lastSize(0, 0);
        static double lastD0 = -1.0;
        if (gray.size() != lastSize || cutoffD0 != lastD0)
        {
            engine.updateMask(gray.size(), FilterType::GaussLow, cutoffD0);
            lastSize = gray.size();
            lastD0 = cutoffD0;
        }
        return engine.run(gray);
    }

    cv::Mat gaussianHighPass(const cv::Mat &gray, double cutoffD0)
    {
        static SuperChargedFilterEngine engine;
        static cv::Size lastSize(0, 0);
        static double lastD0 = -1.0;
        if (gray.size() != lastSize || cutoffD0 != lastD0)
        {
            engine.updateMask(gray.size(), FilterType::GaussHigh, cutoffD0);
            lastSize = gray.size();
            lastD0 = cutoffD0;
        }
        return engine.run(gray);
    }

    cv::Mat bandPass(const cv::Mat &gray, double D0low, double D0high)
    {
        CV_Assert(D0low < D0high);
        static SuperChargedFilterEngine engine;
        static cv::Size lastSize(0, 0);
        static double lastLow = -1.0, lastHigh = -1.0;
        if (gray.size() != lastSize || D0low != lastLow || D0high != lastHigh)
        {
            engine.updateMask(gray.size(), FilterType::BandPass, D0low, D0high);
            lastSize = gray.size();
            lastLow = D0low;
            lastHigh = D0high;
        }
        return engine.run(gray);
    }

    cv::Mat bandReject(const cv::Mat &gray, double D0low, double D0high)
    {
        CV_Assert(D0low < D0high);
        static SuperChargedFilterEngine engine;
        static cv::Size lastSize(0, 0);
        static double lastLow = -1.0, lastHigh = -1.0;
        if (gray.size() != lastSize || D0low != lastLow || D0high != lastHigh)
        {
            engine.updateMask(gray.size(), FilterType::BandReject, D0low, D0high);
            lastSize = gray.size();
            lastLow = D0low;
            lastHigh = D0high;
        }
        return engine.run(gray);
    }

    cv::Mat spectrumMagnitude(const cv::Mat &gray)
    {
        int optRows = cv::getOptimalDFTSize(gray.rows);
        int optCols = cv::getOptimalDFTSize(gray.cols);
        cv::Mat grayFloat, padded;
        gray.convertTo(grayFloat, CV_32F);
        cv::copyMakeBorder(grayFloat, padded, 0, optRows - gray.rows, 0, optCols - gray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
        cv::Mat planes[] = {padded, cv::Mat::zeros(padded.size(), CV_32F)};
        cv::Mat complexImg;
        cv::merge(planes, 2, complexImg);
        cv::dft(complexImg, complexImg);

        int cx = complexImg.cols / 2;
        int cy = complexImg.rows / 2;
        cv::Mat q0(complexImg, cv::Rect(0, 0, cx, cy)), q1(complexImg, cv::Rect(cx, 0, cx, cy));
        cv::Mat q2(complexImg, cv::Rect(0, cy, cx, cy)), q3(complexImg, cv::Rect(cx, cy, cx, cy));
        cv::Mat temp;
        q0.copyTo(temp);
        q3.copyTo(q0);
        temp.copyTo(q3);
        q1.copyTo(temp);
        q2.copyTo(q1);
        temp.copyTo(q2);

        cv::split(complexImg, planes);
        cv::Mat magnitude;
        cv::magnitude(planes[0], planes[1], magnitude);
        magnitude += cv::Scalar::all(1);
        cv::log(magnitude, magnitude);
        cv::normalize(magnitude, magnitude, 0, 255, cv::NORM_MINMAX);
        cv::Mat cropped = magnitude(cv::Rect(0, 0, gray.cols, gray.rows));
        cv::Mat output;
        cropped.convertTo(output, CV_8U);
        return output;
    }

}