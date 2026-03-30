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
        std::cout << "Converting to Word Art using '" << customWord << "'...\n";
        AsciiGenerator::GenerateWordArt(myImage, "ascii.txt", customWord);
    }
    else
    {
        std::cout << "Converting to Standard ASCII...\n";
        AsciiGenerator::GenerateStandard(myImage, "ascii.txt");
    }

    std::cout << "Success! Saved as 'ascii.txt'.\n";
    return 0;
}