#include "EdgeProcessor.h"
#include <cmath>
#include <vector>
#include <future>
#include <thread>
#include <algorithm>

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

        const int numThreads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
        const int rowsPerThread = height / numThreads;
        std::vector<std::future<void>> futures;

        for (int t = 0; t < numThreads; ++t)
        {
            const int startRow = std::max(1, t * rowsPerThread);
            const int endRow = (t == numThreads - 1) ? height - 1 : (t + 1) * rowsPerThread;

            if (startRow >= endRow) continue;

            futures.push_back(std::async(std::launch::async, [=, &input, &output, &kernel]()
            {
                for (int y = startRow; y < endRow; ++y)
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
            }));
        }

        for (auto& f : futures) f.wait();

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

std::vector<char> EdgeProcessor::GeneratePooledEdgeMap(const Image& highResImg, const int targetWidth,
                                                       const int targetHeight, const float edgeThreshold)
{
    const std::vector<float> gray = GetGrayscaleMap(highResImg);

    constexpr float gaussianKernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    const std::vector<float> blur1 = Apply3x3Kernel(gray, highResImg.width, highResImg.height, gaussianKernel);
    const std::vector<float> blur2 = Apply3x3Kernel(blur1, highResImg.width, highResImg.height, gaussianKernel);

    std::vector pooledMap(targetWidth * targetHeight, ' ');
    const float boxWidth = static_cast<float>(highResImg.width) / static_cast<float>(targetWidth);
    const float boxHeight = static_cast<float>(highResImg.height) / static_cast<float>(targetHeight);

    const int numThreads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
    const int rowsPerThread = targetHeight / numThreads;
    std::vector<std::future<void>> futures;

    for (int t = 0; t < numThreads; ++t)
    {
        const int startRow = t * rowsPerThread;
        const int endRow = (t == numThreads - 1) ? targetHeight : startRow + rowsPerThread;

        futures.push_back(std::async(std::launch::async, [=, &pooledMap, &blur1, &blur2]()
        {
            for (int ty = startRow; ty < endRow; ++ty)
            {
                for (int tx = 0; tx < targetWidth; ++tx)
                {
                    const int startX = static_cast<int>(static_cast<float>(tx) * boxWidth);
                    const int endX = static_cast<int>(static_cast<float>((tx + 1)) * boxWidth);
                    const int startY = static_cast<int>(static_cast<float>(ty) * boxHeight);
                    const int endY = static_cast<int>(static_cast<float>((ty + 1)) * boxHeight);

                    int votes[4] = {0, 0, 0, 0}; // 0='/', 1='\', 2='_', 3='|'
                    int totalEdges = 0;

                    for (int y = startY; y < endY; ++y)
                    {
                        for (int x = startX; x < endX; ++x)
                        {
                            if (x == 0 || y == 0 || x >= highResImg.width - 1 || y >= highResImg.height - 1)
                                continue;

                            const int idx = y * highResImg.width + x;

                            if (const float dogMag = std::abs((blur1[idx] - blur2[idx]) * 10.0f); dogMag >
                                edgeThreshold)
                            {
                                const float tl = blur1[(y - 1) * highResImg.width + (x - 1)];
                                const float tc = blur1[(y - 1) * highResImg.width + x];
                                const float tr = blur1[(y - 1) * highResImg.width + (x + 1)];
                                const float ml = blur1[y * highResImg.width + (x - 1)];
                                const float mr = blur1[y * highResImg.width + (x + 1)];
                                const float bl = blur1[(y + 1) * highResImg.width + (x - 1)];
                                const float bc = blur1[(y + 1) * highResImg.width + x];
                                const float br = blur1[(y + 1) * highResImg.width + (x + 1)];

                                const float gx = -tl + tr - 2.0f * ml + 2.0f * mr - bl + br;
                                const float gy = tl + 2.0f * tc + tr - bl - 2.0f * bc - br;

                                const char edgeChar = GetSobelAngleChar(gx, gy);
                                if (edgeChar == '/') votes[0]++;
                                else if (edgeChar == '\\') votes[1]++;
                                else if (edgeChar == '_') votes[2]++;
                                else votes[3]++;

                                totalEdges++;
                            }
                        }
                    }

                    const int requiredEdges = static_cast<int>(std::max(boxWidth, boxHeight) * 0.5f);
                    if (totalEdges < requiredEdges)
                    {
                        continue;
                    }

                    int maxVotes = 0;
                    int winnerIdx = -1;
                    for (int i = 0; i < 4; i++)
                    {
                        if (votes[i] > maxVotes)
                        {
                            maxVotes = votes[i];
                            winnerIdx = i;
                        }
                    }

                    if (winnerIdx == 0) pooledMap[ty * targetWidth + tx] = '/';
                    else if (winnerIdx == 1) pooledMap[ty * targetWidth + tx] = '\\';
                    else if (winnerIdx == 2) pooledMap[ty * targetWidth + tx] = '_';
                    else if (winnerIdx == 3) pooledMap[ty * targetWidth + tx] = '|';
                }
            }
        }));
    }

    for (auto& f : futures) f.wait();

    return pooledMap;
}