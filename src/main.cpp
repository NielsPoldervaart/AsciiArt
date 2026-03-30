#include <iostream>
#include <string>
#include "Image.h"
#include "AsciiGenerator.h"

struct AppConfig
{
    std::string imagePath;
    int targetWidth = 100;
    std::string customWord;
    bool useColor = true;
    bool showHelp = false;
    float contrast = 1.0f;
};

AppConfig ParseArguments(const int argc, char* argv[])
{
    AppConfig config;

    if (argc < 2)
    {
        config.showHelp = true;
        return config;
    }

    const std::string firstArg = argv[1];
    if (firstArg == "--help" || firstArg == "-h")
    {
        config.showHelp = true;
        return config;
    }

    config.imagePath = firstArg;

    for (int i = 2; i < argc; ++i)
    {
        if (std::string arg = argv[i]; arg == "--width" && i + 1 < argc)
        {
            config.targetWidth = std::stoi(argv[++i]);
        }
        else if (arg == "--word" && i + 1 < argc)
        {
            config.customWord = argv[++i];
        }
        else if (arg == "--no-color")
        {
            config.useColor = false;
        }
        else if (arg == "--help" || arg == "-h")
        {
            config.showHelp = true;
        }
        else if (arg == "--contrast" && i + 1 < argc)
        {
            config.contrast = std::stof(argv[++i]);
        }
    }

    return config;
}

int main(const int argc, char* argv[])
{
    const auto [imagePath, targetWidth, customWord, useColor, showHelp, contrast] = ParseArguments(argc, argv);

    if (showHelp || imagePath.empty())
    {
        std::cout << "=== AsciiArt Generator ===\n";
        std::cout << "Usage: ./AsciiArt <image_path> [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --width <num>    Set the output width (default: 100)\n";
        std::cout << "  --word <text>    Use a custom word for bright areas\n";
        std::cout << "  --no-color       Disable ANSI terminal colors\n";
        std::cout << "  --help, -h       Show this help menu\n";
        return 0;
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
        AsciiGenerator::GenerateWordArt(myImage, customWord, useColor, contrast);
    }
    else
    {
        AsciiGenerator::GenerateStandard(myImage, useColor, contrast);
    }

    if (useColor) std::cout << "\x1b[0m";
    return 0;
}