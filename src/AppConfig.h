#pragma once
#include <string>

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

AppConfig ParseArguments(int argc, char* argv[]);

void PrintHelpMenu();