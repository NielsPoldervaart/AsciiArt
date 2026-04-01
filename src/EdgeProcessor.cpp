#include "EdgeProcessor.h"
#include <cmath>
#include <vector>

namespace
{
    std::vector<float> GetGrayscaleMap(const Image& img)
    {
        std::vector<float> gray(img.width * img.height);
        for (int y = 0; y < img.height; ++y)
        {
            for (int x = 0; x < img.width; ++x)
            {
                const int idx = (y * img.width + x) * img.channels;
                gray[y * img.width + x] =
                    0.2126f * static_cast<float>(img.pixelData[idx]) +
                    0.7152f * static_cast<float>(img.pixelData[idx + 1]) +
                    0.0722f * static_cast<float>(img.pixelData[idx + 2]);
            }
        }
        return gray;
    }

    std::vector<float> Apply3x3Kernel(const std::vector<float>& input, const int width, const int height,
                                      const float kernel[9])
    {
        std::vector<float> output = input;
        for (int y = 1; y < height - 1; ++y)
        {
            for (int x = 1; x < width - 1; ++x)
            {
                float sum = 0.0f;
                sum += input[(y - 1) * width + (x - 1)] * kernel[0];
                sum += input[(y - 1) * width + x] * kernel[1];
                sum += input[(y - 1) * width + (x + 1)] * kernel[2];
                sum += input[y * width + (x - 1)] * kernel[3];
                sum += input[y * width + x] * kernel[4];
                sum += input[y * width + (x + 1)] * kernel[5];
                sum += input[(y + 1) * width + (x - 1)] * kernel[6];
                sum += input[(y + 1) * width + x] * kernel[7];
                sum += input[(y + 1) * width + (x + 1)] * kernel[8];
                output[y * width + x] = sum;
            }
        }
        return output;
    }

    char GetSobelAngleChar(const float gx, const float gy)
    {
        const float angle = std::atan2(gy, gx) * (180.0f / 3.14159265f);

        if ((22.5f <= angle && angle <= 67.5f) || (-157.5f <= angle && angle <= -112.5f)) return '\\';
        if ((67.5f <= angle && angle <= 112.5f) || (-112.5f <= angle && angle <= -67.5f)) return '_';
        if ((112.5f <= angle && angle <= 157.5f) || (-67.5f <= angle && angle <= -22.5f)) return '/';
        return '|';
    }
}

std::vector<char> EdgeProcessor::GenerateEdgeMap(const Image& img, const float edgeThreshold)
{
    std::vector<char> edgeMap(img.width * img.height, ' ');

    const std::vector<float> gray = GetGrayscaleMap(img);

    constexpr float gaussianKernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    const std::vector<float> blur1 = Apply3x3Kernel(gray, img.width, img.height, gaussianKernel);

    const std::vector<float> blur2 = Apply3x3Kernel(blur1, img.width, img.height, gaussianKernel);

    for (int y = 1; y < img.height - 1; ++y)
    {
        for (int x = 1; x < img.width - 1; ++x)
        {
            const int idx = y * img.width + x;

            if (const float dogMagnitude = (blur1[idx] - blur2[idx]) * 10.0f; std::abs(dogMagnitude) > edgeThreshold)
            {
                const float tl = blur1[(y - 1) * img.width + (x - 1)];
                const float tc = blur1[(y - 1) * img.width + x];
                const float tr = blur1[(y - 1) * img.width + (x + 1)];
                const float ml = blur1[y * img.width + (x - 1)];
                const float mr = blur1[y * img.width + (x + 1)];
                const float bl = blur1[(y + 1) * img.width + (x - 1)];
                const float bc = blur1[(y + 1) * img.width + x];
                const float br = blur1[(y + 1) * img.width + (x + 1)];

                const float gx = -tl + tr - 2.0f * ml + 2.0f * mr - bl + br;
                const float gy = tl + 2.0f * tc + tr - bl - 2.0f * bc - br;

                edgeMap[idx] = GetSobelAngleChar(gx, gy);
            }
        }
    }

    return edgeMap;
}