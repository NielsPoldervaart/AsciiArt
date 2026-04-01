#include "AsciiGenerator.h"
#include <string_view>
#include <algorithm>
#include <cmath>

namespace
{
    float GetLuminance(const Image& img, const int x, const int y)
    {
        const int index = (y * img.width + x) * img.channels;
        return 0.2126f * static_cast<float>(img.pixelData[index]) +
            0.7152f * static_cast<float>(img.pixelData[index + 1]) +
            0.0722f * static_cast<float>(img.pixelData[index + 2]);
    }

    char GetSobelAngleChar(const float gx, const float gy)
    {
        const float angle = std::atan2(gy, gx) * (180.0f / 3.14159265f);

        if ((22.5f <= angle && angle <= 67.5f) || (-157.5f <= angle && angle <= -112.5f))
            return '\\';
        if ((67.5f <= angle && angle <= 112.5f) || (-112.5f <= angle && angle <= -67.5f))
            return '_';
        if ((112.5f <= angle && angle <= 157.5f) || (-67.5f <= angle && angle <= -22.5f))
            return '/';

        return '|';
    }
}

AsciiFrame AsciiGenerator::GenerateStandard(const Image& img, const float contrast)
{
    constexpr std::string_view asciiChars = " .:-=+*#%@";
    constexpr size_t numChars = asciiChars.length();

    constexpr float edgeThreshold = 100.0f;

    AsciiFrame frame;
    frame.width = img.width;
    frame.height = img.height;
    frame.pixels.reserve(img.width * img.height);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int r = img.pixelData[index];
            const int g = img.pixelData[index + 1];
            const int b = img.pixelData[index + 2];

            int brightness = static_cast<int>(GetLuminance(img, x, y));
            brightness = std::clamp(static_cast<int>(static_cast<float>(brightness - 128) * contrast + 128.0f), 0, 255);
            const size_t charIndex = (brightness * (numChars - 1)) / 255;

            char c = asciiChars[charIndex];

            if (x > 0 && x < img.width - 1 && y > 0 && y < img.height - 1)
            {
                const float tl = GetLuminance(img, x - 1, y - 1);
                const float tc = GetLuminance(img, x, y - 1);
                const float tr = GetLuminance(img, x + 1, y - 1);
                const float ml = GetLuminance(img, x - 1, y);
                const float mr = GetLuminance(img, x + 1, y);
                const float bl = GetLuminance(img, x - 1, y + 1);
                const float bc = GetLuminance(img, x, y + 1);
                const float br = GetLuminance(img, x + 1, y + 1);

                const float gx = -tl + tr - 2.0f * ml + 2.0f * mr - bl + br;
                const float gy = tl + 2.0f * tc + tr - bl - 2.0f * bc - br;

                if (const float magnitude = std::sqrt(gx * gx + gy * gy); magnitude > edgeThreshold)
                {
                    c = GetSobelAngleChar(gx, gy);
                }
            }

            frame.pixels.push_back({c, r, g, b});
        }
    }
    return frame;
}

AsciiFrame AsciiGenerator::GenerateWordArt(const Image& img, const std::string& targetWord, const float contrast)
{
    constexpr std::string_view shadingChars = " .:-=+*";
    constexpr size_t numShading = shadingChars.length();
    size_t wordIndex = 0;

    constexpr float edgeThreshold = 100.0f;

    AsciiFrame frame;
    frame.width = img.width;
    frame.height = img.height;
    frame.pixels.reserve(img.width * img.height);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int r = img.pixelData[index];
            const int g = img.pixelData[index + 1];
            const int b = img.pixelData[index + 2];

            int brightness = static_cast<int>(GetLuminance(img, x, y));
            brightness = std::clamp(static_cast<int>(static_cast<float>(brightness - 128) * contrast + 128.0f), 0, 255);

            char c;
            if (brightness > 128)
            {
                c = targetWord[wordIndex];
                wordIndex = (wordIndex + 1) % targetWord.length();
            }
            else
            {
                const size_t charIndex = (brightness * (numShading - 1)) / 128;
                c = shadingChars[charIndex];
            }

            if (x > 0 && x < img.width - 1 && y > 0 && y < img.height - 1)
            {
                const float tl = GetLuminance(img, x - 1, y - 1);
                const float tc = GetLuminance(img, x, y - 1);
                const float tr = GetLuminance(img, x + 1, y - 1);
                const float ml = GetLuminance(img, x - 1, y);
                const float mr = GetLuminance(img, x + 1, y);
                const float bl = GetLuminance(img, x - 1, y + 1);
                const float bc = GetLuminance(img, x, y + 1);
                const float br = GetLuminance(img, x + 1, y + 1);

                const float gx = -tl + tr - 2.0f * ml + 2.0f * mr - bl + br;
                const float gy = tl + 2.0f * tc + tr - bl - 2.0f * bc - br;

                if (const float magnitude = std::sqrt(gx * gx + gy * gy); magnitude > edgeThreshold)
                {
                    c = GetSobelAngleChar(gx, gy);
                }
            }

            frame.pixels.push_back({c, r, g, b});
        }
    }
    return frame;
}