#include "ImageExporter.h"
#include <iostream>
#include <fstream>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#pragma clang diagnostic pop

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace
{
    struct BakedFont
    {
        std::vector<unsigned char> atlas;
        stbtt_bakedchar charData[96]{};
        int charWidth = 0;
        bool success = false;
    };

    BakedFont LoadAndBakeFont(const std::string& fontPath, const int fontSize)
    {
        BakedFont result;
        std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::cout << "Error: Could not open font file: " << fontPath << "\n";
            return result;
        }

        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> ttf_buffer(size);
        file.read(reinterpret_cast<char*>(ttf_buffer.data()), size);

        result.atlas.resize(512 * 512);
        stbtt_BakeFontBitmap(ttf_buffer.data(), 0, static_cast<float>(fontSize), result.atlas.data(), 512, 512, 32, 96,
                             result.charData);

        result.charWidth = static_cast<int>(result.charData['A' - 32].xadvance);
        result.success = true;

        return result;
    }
}

float ImageExporter::GetFontRatio(const std::string& fontPath, const int fontSize)
{
    const BakedFont font = LoadAndBakeFont(fontPath, fontSize);

    if (!font.success || font.charWidth == 0) return 2.0f;

    return static_cast<float>(fontSize) / static_cast<float>(font.charWidth);
}

void ImageExporter::ExportToPng(const AsciiFrame& frame, const std::string& fontPath, const std::string& outputPath,
                                const bool useColor, const int fontSize)
{
    const auto [atlas, charData, charWidth, success] = LoadAndBakeFont(fontPath, fontSize);
    if (!success) return;

    const int charHeight = fontSize;

    const int imgWidth = frame.width * charWidth;
    const int imgHeight = frame.height * charHeight;

    std::vector<unsigned char> canvas(imgWidth * imgHeight * 3, 0);

    for (int y = 0; y < frame.height; ++y)
    {
        for (int x = 0; x < frame.width; ++x)
        {
            const auto& [c, r, g, b] = frame.pixels[y * frame.width + x];

            const int paintR = useColor ? r : 255;
            const int paintG = useColor ? g : 255;
            const int paintB = useColor ? b : 255;

            auto drawChar = [&](const unsigned char ch, const int startX, const int startY)
            {
                if (ch < 32 || ch >= 128) return;

                const stbtt_bakedchar* bData = &charData[ch - 32];
                const int w = bData->x1 - bData->x0;
                const int h = bData->y1 - bData->y0;

                for (int cy = 0; cy < h; ++cy)
                {
                    for (int cx = 0; cx < w; ++cx)
                    {
                        const int destX = startX + static_cast<int>(bData->xoff) + cx;
                        const int destY = startY + static_cast<int>(bData->yoff) + cy + fontSize;

                        if (destX >= 0 && destX < imgWidth && destY >= 0 && destY < imgHeight)
                        {
                            if (const unsigned char alpha = atlas[(bData->y0 + cy) * 512 + (bData->x0 + cx)]; alpha > 0)
                            {
                                const int idx = (destY * imgWidth + destX) * 3;
                                canvas[idx] = (paintR * alpha) / 255;
                                canvas[idx + 1] = (paintG * alpha) / 255;
                                canvas[idx + 2] = (paintB * alpha) / 255;
                            }
                        }
                    }
                }
            };

            const int screenX = x * charWidth;
            const int screenY = y * charHeight;

            drawChar(c, screenX, screenY);
        }
    }

    stbi_write_png(outputPath.c_str(), imgWidth, imgHeight, 3, canvas.data(), imgWidth * 3);
    std::cout << "Successfully exported PNG to: " << outputPath << "\n";
}