#pragma once
#include "AsciiGenerator.h"
#include <string>

class ImageExporter
{
public:
    static void ExportToPng(const AsciiFrame& frame, const std::string& fontPath, const std::string& outputPath,
                            bool useColor, int fontSize = 16);
};