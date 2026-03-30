#pragma once
#include "Image.h"
#include <string>

class AsciiGenerator {
public:
    static void GenerateStandard(const Image& img, const std::string& outputPath);

    static void GenerateWordArt(const Image& img, const std::string& outputPath, const std::string& targetWord);
};