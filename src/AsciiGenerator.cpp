#include "AsciiGenerator.h"
#include <fstream>

void AsciiGenerator::GenerateStandard(const Image& img, const std::string& outputPath)
{
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) return;

    constexpr std::string asciiChars = " .:-=+*#%@";
    constexpr size_t numChars = asciiChars.length();

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int r = img.pixelData[index];
            const int g = img.pixelData[index + 1];
            const int b = img.pixelData[index + 2];
            const int brightness = (r + g + b) / 3;

            const size_t charIndex = (brightness * (numChars - 1)) / 255;

            outFile << asciiChars[charIndex] << " ";
        }
        outFile << "\n";
    }
    outFile.close();
}

void AsciiGenerator::GenerateWordArt(const Image& img, const std::string& outputPath, const std::string& targetWord)
{
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) return;

    constexpr std::string_view shadingChars = " .:-=+*sN";
    constexpr size_t numShading = shadingChars.length();
    size_t wordIndex = 0;

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int r = img.pixelData[index];
            const int g = img.pixelData[index + 1];
            const int b = img.pixelData[index + 2];

            if (const int brightness = (r + g + b) / 3; brightness > 100)
            {
                outFile << targetWord[wordIndex];
                wordIndex = (wordIndex + 1) % targetWord.length();

                outFile << targetWord[wordIndex];
                wordIndex = (wordIndex + 1) % targetWord.length();
            }
            else
            {
                const size_t charIndex = (brightness * (numShading - 1)) / 100;
                outFile << shadingChars[charIndex] << shadingChars[charIndex];
            }
        }
        outFile << "\n";
    }
    outFile.close();
}