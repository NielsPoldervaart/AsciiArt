#pragma once
#include "Image.h"
#include <string>

class AsciiGenerator
{
public:
    static void GenerateStandard(const Image& img, bool useColor = true, float contrast = 1.0f);
    static void GenerateWordArt(const Image& img, const std::string& targetWord, bool useColor = true,
                                float contrast = 1.0f);
};