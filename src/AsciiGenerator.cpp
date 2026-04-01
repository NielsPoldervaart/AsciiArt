#include "AsciiGenerator.h"
#include "EdgeProcessor.h"
#include <string_view>
#include <algorithm>

AsciiFrame AsciiGenerator::GenerateStandard(const Image& img, const float contrast, const float edgeThreshold)
{
    constexpr std::string_view asciiChars = " .:-=+*#%@";
    constexpr size_t numChars = asciiChars.length();

    AsciiFrame frame;
    frame.width = img.width;
    frame.height = img.height;
    frame.pixels.reserve(img.width * img.height);

    const std::vector<char> edgeMap = EdgeProcessor::GenerateEdgeMap(img, edgeThreshold);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int r = img.pixelData[index];
            const int g = img.pixelData[index + 1];
            const int b = img.pixelData[index + 2];

            int brightness = static_cast<int>(
                0.2126f * static_cast<float>(r) +
                0.7152f * static_cast<float>(g) +
                0.0722f * static_cast<float>(b)
            );
            brightness = std::clamp(static_cast<int>(static_cast<float>(brightness - 128) * contrast + 128.0f), 0, 255);
            const size_t charIndex = (brightness * (numChars - 1)) / 255;

            char c = asciiChars[charIndex];

            if (const char edgeChar = edgeMap[y * img.width + x]; edgeChar != ' ')
                c = edgeChar;

            frame.pixels.push_back({c, r, g, b});
        }
    }
    return frame;
}

AsciiFrame AsciiGenerator::GenerateWordArt(const Image& img, const std::string& targetWord, const float contrast,
                                           const float edgeThreshold)
{
    constexpr std::string_view shadingChars = " .:-=+*";
    constexpr size_t numShading = shadingChars.length();
    size_t wordIndex = 0;

    AsciiFrame frame;
    frame.width = img.width;
    frame.height = img.height;
    frame.pixels.reserve(img.width * img.height);

    const std::vector<char> edgeMap = EdgeProcessor::GenerateEdgeMap(img, edgeThreshold);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int r = img.pixelData[index];
            const int g = img.pixelData[index + 1];
            const int b = img.pixelData[index + 2];

            int brightness = static_cast<int>(
                0.2126f * static_cast<float>(r) +
                0.7152f * static_cast<float>(g) +
                0.0722f * static_cast<float>(b)
            );
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

            if (const char edgeChar = edgeMap[y * img.width + x]; edgeChar != ' ')
                c = edgeChar;

            frame.pixels.push_back({c, r, g, b});
        }
    }
    return frame;
}