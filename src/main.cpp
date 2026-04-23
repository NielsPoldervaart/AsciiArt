#include <iostream>
#include <string>
#include "Image.h"
#include "AsciiGenerator.h"
#include "EdgeProcessor.h"
#include "ImageExporter.h"
#include "AsciiExporter.h"
#include "UpdateChecker.h"
#include "AppConfig.h"

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
        PrintHelpMenu();
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
        frame = AsciiGenerator::GenerateWordArt(myImage, pooledEdges, config);
    else
        frame = AsciiGenerator::GenerateStandard(myImage, pooledEdges, config);

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