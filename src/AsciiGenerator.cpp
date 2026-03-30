#include "AsciiGenerator.h"
#include <iostream>

void AsciiGenerator::GenerateStandard(const Image& img)
{
    constexpr std::string_view asciiChars = " .:-=+*#%@";
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

            std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m"
                << asciiChars[charIndex] << " ";
        }
        std::cout << "\x1b[0m\n";
    }
}

void AsciiGenerator::GenerateWordArt(const Image& img, const std::string& targetWord)
{
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

            std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m";

            if (const int brightness = (r + g + b) / 3; brightness > 100)
            {
                std::cout << targetWord[wordIndex];
                wordIndex = (wordIndex + 1) % targetWord.length();

                std::cout << targetWord[wordIndex];
                wordIndex = (wordIndex + 1) % targetWord.length();
            }
            else
            {
                const size_t charIndex = (brightness * (numShading - 1)) / 100;
                std::cout << shadingChars[charIndex] << shadingChars[charIndex];
            }
        }
        std::cout << "\x1b[0m\n";
    }
}