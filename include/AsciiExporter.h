#pragma once
#include <string>
#include "AsciiGenerator.h"

class AsciiExporter
{
public:
    AsciiExporter() = delete;

    static bool SaveTxt(const std::string& filepath, const AsciiFrame& frame);
    static bool SaveHtml(const std::string& filepath, const AsciiFrame& frame, const std::string& fontPath);
};