#include <iostream>
#include <string>
#include "Image.h"
#include "AsciiGenerator.h"
#include "EdgeProcessor.h"
#include "ImageExporter.h"
#include "AsciiExporter.h"
#include "UpdateChecker.h"

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
    float saturation = 1.0f;
    float gamma = 1.0f;
    bool dither = false;
    std::string txtPath;
    std::string htmlPath;
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
        else if ((arg == "--contrast" || arg == "-c") && i + 1 < argc)
        {
            config.contrast = std::stof(argv[++i]);
        }
        else if ((arg == "--saturation" || arg == "-s") && i + 1 < argc)
        {
            config.saturation = std::stof(argv[++i]);
        }
        else if ((arg == "--gamma" || arg == "-g") && i + 1 < argc)
        {
            config.gamma = std::stof(argv[++i]);
        }
        else if (arg == "--font" && i + 1 < argc)
        {
            config.fontPath = argv[++i];
        }
        else if (arg == "--out" && i + 1 < argc)
        {
            config.outputPath = argv[++i];
        }
        else if ((arg == "--edgeThreshold" || arg == "-et") && i + 1 < argc)
        {
            config.edgeThreshold = std::stof(argv[++i]);
        }
        else if (arg == "--retro")
        {
            config.retroColors = true;
        }
        else if (arg == "--dither")
        {
            config.dither = true;
        }
        else if (arg == "--txt" && i + 1 < argc)
        {
            config.txtPath = argv[++i];
        }
        else if (arg == "--html" && i + 1 < argc)
        {
            config.htmlPath = argv[++i];
        }
    }

    return config;
}

int main(const int argc, char* argv[])
{
    if constexpr (ENABLE_UPDATER)
    {
        std::cout << "Checking for updates...\n";

        if (auto [updateAvailable, latestVersion, releaseUrl] = UpdateChecker::CheckForUpdates(
            REPO_OWNER, REPO_NAME, APP_VERSION); updateAvailable)
        {
            std::cout << "\n======================================================\n";
            std::cout << " NEW UPDATE AVAILABLE: " << latestVersion << " (Current: " << APP_VERSION << ")\n";
            std::cout << " Download here: " << releaseUrl << "\n";
            std::cout << "======================================================\n\n";
        }
    }

    const AppConfig config = ParseArguments(argc, argv);

    if (config.showHelp || config.imagePath.empty())
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
        return 0;
    }

    Image myImage(config.imagePath);

    if (!myImage.IsValid())
    {
        std::cout << "Failed to load image. Double-check your path!\n";
        return 1;
    }

    const float exactFontRatio = ImageExporter::GetFontRatio(config.fontPath, 16);

    const float ratio = static_cast<float>(myImage.height) / static_cast<float>(myImage.width);
    const int targetHeight = static_cast<int>((static_cast<float>(config.targetWidth) * ratio) / exactFontRatio);

    std::cout << "Analyzing High-Resolution Edges. Might take a moment...\n";
    const std::vector<char> pooledEdges = EdgeProcessor::GeneratePooledEdgeMap(
        myImage, config.targetWidth, targetHeight, config.edgeThreshold);

    std::cout << "Downscaling image for shading and colors...\n";
    myImage.Resize(config.targetWidth, exactFontRatio);

    AsciiFrame frame;
    if (!config.customWord.empty())
        frame = AsciiGenerator::GenerateWordArt(myImage, config.customWord, pooledEdges, config.contrast,
                                                config.retroColors, config.saturation,
                                                config.gamma, config.dither);
    else
        frame = AsciiGenerator::GenerateStandard(myImage, pooledEdges, config.contrast, config.retroColors,
                                                 config.saturation, config.gamma,
                                                 config.dither);

    std::cout << "Rendering PNG...\n";
    ImageExporter::ExportToPng(frame, config.fontPath, config.outputPath, config.useColor);

    if (!config.txtPath.empty())
    {
        std::cout << "Exporting TXT...\n";
        if (AsciiExporter::SaveTxt(config.txtPath, frame))
            std::cout << "Saved text export to " << config.txtPath << "\n";
    }

    if (!config.htmlPath.empty())
    {
        std::cout << "Exporting HTML...\n";
        if (AsciiExporter::SaveHtml(config.htmlPath, frame, config.fontPath))
            std::cout << "Saved HTML export to " << config.htmlPath << "\n";
    }

    return 0;
}