#pragma once
#include "Image.h"
#include "AppConfig.h"
#include <string>
#include <vector>

struct AsciiPixel
{
    char c;
    int r, g, b;
};

struct AsciiFrame
{
    int width = 0;
    int height = 0;
    std::vector<AsciiPixel> pixels;
};

class AsciiGenerator
{
public:
    AsciiGenerator() = delete;

    static AsciiFrame GenerateStandard(const Image& img, const std::vector<char>& edgeMap, const AppConfig& config);
    static AsciiFrame GenerateWordArt(const Image& img, const std::vector<char>& edgeMap, const AppConfig& config);
};