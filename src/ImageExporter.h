#pragma once
#include "AsciiGenerator.h"
#include <string>

class ImageExporter
{
public:
    static float GetFontRatio(const std::string& fontPath, int fontSize = 16);

    static void ExportToPng(const AsciiFrame& frame, const std::string& fontPath, const std::string& outputPath,
                            bool useColor, int fontSize = 16);
};