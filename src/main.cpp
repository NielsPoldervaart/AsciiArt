#include <iostream>
#include <string>
#include "Image.h"
#include "AsciiGenerator.h"
#include "ImageExporter.h"

struct AppConfig
{
    std::string imagePath;
    int targetWidth = 100;
    std::string customWord;
    bool useColor = true;
    bool showHelp = false;
    float contrast = 1.0f;
    std::string fontPath = "fonts/VT323.ttf";
    std::string outputPath = "ascii.png";
    float edgeThreshold = 100.0f;
    bool retroColors = false;
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
        else if (arg == "--font" && i + 1 < argc)
        {
            config.fontPath = argv[++i];
        }
        else if (arg == "--out" && i + 1 < argc)
        {
            config.outputPath = argv[++i];
        }
        else if (arg == "--edgeThreshold" || arg == "-et" && i + 1 < argc)
        {
            config.edgeThreshold = std::stof(argv[++i]);
        }
        else if (arg == "--retro")
        {
            config.retroColors = true;
        }
    }

    return config;
}

int main(const int argc, char* argv[])
{
    const auto [imagePath, targetWidth, customWord, useColor, showHelp, contrast, fontPath, outputPath, edgeThreshold,
            retroColors] =
        ParseArguments(argc, argv);

    if (showHelp || imagePath.empty())
    {
        std::cout << "=== AsciiArt Generator ===\n";
        std::cout << "Usage: ./AsciiArt <image_path> [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --width <num>      Set the output width in characters (default: 100)\n";
        std::cout << "  --word <text>      Use a custom word for bright areas\n";
        std::cout << "  --no-color         Disable ANSI colors and export monochrome PNG\n";
        std::cout << "  --contrast <num>   Adjust image contrast multiplier (default: 1.0)\n";
        std::cout << "  --font <path>      Path to a monospace .ttf font (default: fonts/VT323.ttf)\n";
        std::cout << "  --threshold <num>  Set Sobel edge detection threshold (default: 100.0)\n";
        std::cout << "  --out <path>       Path to save the generated PNG (default: output.png)\n";
        std::cout << "  --help, -h         Show this help menu\n";
        return 0;
    }

    Image myImage(imagePath);

    if (!myImage.IsValid())
    {
        std::cout << "Failed to load image. Double-check your path!\n";
        return 1;
    }

    const float exactFontRatio = ImageExporter::GetFontRatio(fontPath, 16);
    myImage.Resize(targetWidth, exactFontRatio);

    AsciiFrame frame;
    if (!customWord.empty())
        frame = AsciiGenerator::GenerateWordArt(myImage, customWord, contrast, edgeThreshold, retroColors);
    else
        frame = AsciiGenerator::GenerateStandard(myImage, contrast, edgeThreshold, retroColors);

    std::cout << "\nRendering PNG...\n";
    ImageExporter::ExportToPng(frame, fontPath, outputPath, useColor);

    return 0;
}