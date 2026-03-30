#include <iostream>
#include <string>
#include "Image.h"
#include "AsciiGenerator.h"

int main(const int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./AsciiArt <image_path> [--width <number>] [--word <custom_word>]\n";
        return 1;
    }

    const std::string imagePath = argv[1];

    int targetWidth = 100;
    std::string customWord;
    bool useColor = true;

    for (int i = 2; i < argc; ++i)
    {
        if (std::string arg = argv[i]; arg == "--width" && i + 1 < argc)
        {
            targetWidth = std::stoi(argv[++i]);
        }
        else if (arg == "--word" && i + 1 < argc)
        {
            customWord = argv[++i];
        }
        else if (arg == "--no-color")
        {
            useColor = false;
        }
    }

    Image myImage(imagePath);

    if (!myImage.IsValid())
    {
        std::cout << "Failed to load image. Double-check your path!\n";
        return 1;
    }

    myImage.Resize(targetWidth);

    if (!customWord.empty())
    {
        AsciiGenerator::GenerateWordArt(myImage, customWord, useColor);
    }
    else
    {
        AsciiGenerator::GenerateStandard(myImage, useColor);
    }
    std::cout << "\x1b[0m";
    return 0;
}