#include "AppConfig.h"
#include "iostream"

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
            config.targetWidth = std::stoi(argv[++i]);
        else if (arg == "--word" && i + 1 < argc)
            config.customWord = argv[++i];
        else if (arg == "--no-color")
            config.useColor = false;
        else if (arg == "--help" || arg == "-h")
            config.showHelp = true;
        else if ((arg == "--contrast" || arg == "-c") && i + 1 < argc)
            config.contrast = std::stof(argv[++i]);
        else if ((arg == "--saturation" || arg == "-s") && i + 1 < argc)
            config.saturation = std::stof(argv[++i]);
        else if ((arg == "--gamma" || arg == "-g") && i + 1 < argc)
            config.gamma = std::stof(argv[++i]);
        else if (arg == "--font" && i + 1 < argc)
            config.fontPath = argv[++i];
        else if (arg == "--out" && i + 1 < argc)
            config.outputPath = argv[++i];
        else if ((arg == "--edgeThreshold" || arg == "-et") && i + 1 < argc)
            config.edgeThreshold = std::stof(argv[++i]);
        else if (arg == "--retro")
            config.retroColors = true;
        else if (arg == "--dither")
            config.dither = true;
        else if (arg == "--txt" && i + 1 < argc)
            config.txtPath = argv[++i];
        else if (arg == "--html" && i + 1 < argc)
            config.htmlPath = argv[++i];
    }

    return config;
}

void PrintHelpMenu()
{
    std::cout << "=== AsciiArt Generator ===\n";
    std::cout << "Usage: ./AsciiArt <image_path> [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --width <num>          Set the output width in characters (default: 100)\n";
    std::cout << "  --word <text>          Use a custom word for bright areas\n";
    std::cout << "  --no-color             Disable colors and export monochrome PNG\n";
    std::cout << "  --contrast, -c <num>   Adjust image contrast multiplier (default: 1.0)\n";
    std::cout << "  --saturation, -s <num> Adjust color saturation multiplier (default: 1.0)\n";
    std::cout << "  --gamma, -g <num>      Adjust gamma curve (default: 1.0, < 1.0 is brighter, > 1.0 is darker)\n";
    std::cout << "  --retro                Use classic 8-color retro palette with max brightness\n";
    std::cout << "  --dither               Apply dithering for smooth shading gradients\n";
    std::cout << "  --threshold, -et <num> Set Sobel edge detection threshold (default: 100.0)\n";
    std::cout << "  --font <path>          Path to a monospace .ttf font (default: fonts/VT323.ttf)\n";
    std::cout << "  --out <path>           Path to save the generated PNG (default: ascii.png)\n";
    std::cout << "  --txt <path>           Path to save raw text ASCII (e.g., output.txt)\n";
    std::cout << "  --html <path>          Path to save dark-mode HTML (e.g., output.html)\n";
    std::cout << "  --help, -h             Show this help menu\n";
}