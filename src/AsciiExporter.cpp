#include "AsciiExporter.h"
#include <fstream>
#include <iostream>

namespace
{
    std::string EscapeHTML(char c)
    {
        switch (c)
        {
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '&': return "&amp;";
        case '"': return "&quot;";
        default: return {1, c};
        }
    }
}

bool AsciiExporter::SaveTxt(const std::string& filepath, const AsciiFrame& frame)
{
    std::ofstream outFile(filepath);

    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open " << filepath << " for writing.\n";
        return false;
    }

    for (int y = 0; y < frame.height; ++y)
    {
        for (int x = 0; x < frame.width; ++x)
        {
            const int index = y * frame.width + x;
            outFile << frame.pixels[index].c;
        }
        outFile << "\n";
    }

    outFile.close();
    return true;
}

bool AsciiExporter::SaveHtml(const std::string& filepath, const AsciiFrame& frame, const std::string& fontPath)
{
    std::ofstream outFile(filepath);

    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open " << filepath << " for writing.\n";
        return false;
    }

    outFile << "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n";
    outFile << "<style>\n";

    outFile << "  @font-face {\n";
    outFile << "      font-family: 'AsciiCustomFont';\n";
    outFile << "      src: url('" << fontPath << "');\n";
    outFile << "  }\n";
    outFile << "  body { background-color: #1e1e1e; padding: 20px; }\n";
    outFile << "  .terminal { \n";
    outFile << "      font-family: 'AsciiCustomFont', monospace; \n";
    outFile << "      font-size: 10px; \n";
    outFile << "      line-height: 8px; \n";
    outFile << "      letter-spacing: 2px; \n";
    outFile << "      white-space: pre; \n";
    outFile << "  }\n";
    outFile << "</style>\n";
    outFile << "</head>\n<body>\n";
    outFile << "<div class='terminal'>\n";

    for (int y = 0; y < frame.height; ++y)
    {
        for (int x = 0; x < frame.width; ++x)
        {
            const int index = y * frame.width + x;
            const auto& [symbol, r, g, b] = frame.pixels[index];

            outFile << "<span style=\"color:rgb(" << r << "," << g << "," << b << ");\">"
                << EscapeHTML(symbol) << "</span>";
        }
        outFile << "\n";
    }

    outFile << "</div>\n</body>\n</html>";

    outFile.close();
    return true;
}