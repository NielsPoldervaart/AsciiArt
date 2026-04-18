#pragma once
#include "Image.h"
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
    static AsciiFrame GenerateStandard(const Image& img, float contrast = 1.0f, float edgeThreshold = 100.0f,
                                       bool retroColors = false, float saturation = 1.0f, float gamma = 1.0f,
                                       bool dither = false);
    static AsciiFrame GenerateWordArt(const Image& img, const std::string& targetWord, float contrast = 1.0f,
                                      float edgeThreshold = 100.0f, bool retroColors = false, float saturation = 1.0f,
                                      float gamma = 1.0f, bool dither = false);
};