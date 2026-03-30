#pragma once
#include "Image.h"
#include <string>

class AsciiGenerator
{
public:
    static void GenerateStandard(const Image& img);
    static void GenerateWordArt(const Image& img, const std::string& targetWord);
};