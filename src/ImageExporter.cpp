#include "ImageExporter.h"
#include <iostream>
#include <fstream>
#include <vector>

// gives deprecated sprintf warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#pragma clang diagnostic pop

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

void ImageExporter::ExportToPng(const AsciiFrame& frame, const std::string& fontPath, const std::string& outputPath,
                                bool useColor, int fontSize)
{
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cout << "Error: Could not open font file: " << fontPath << "\n";
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> ttf_buffer(size);
    file.read(reinterpret_cast<char*>(ttf_buffer.data()), size);

    std::vector<unsigned char> atlas(512 * 512);
    stbtt_bakedchar charData[96];
    stbtt_BakeFontBitmap(ttf_buffer.data(), 0, static_cast<float>(fontSize), atlas.data(), 512, 512, 32, 96, charData);

    int charWidth = static_cast<int>(charData['A' - 32].xadvance);
    int charHeight = fontSize;

    int imgWidth = frame.width * charWidth;
    int imgHeight = frame.height * charHeight;

    std::vector<unsigned char> canvas(imgWidth * imgHeight * 3, 0);

    for (int y = 0; y < frame.height; ++y)
    {
        for (int x = 0; x < frame.width; ++x)
        {
            const auto& [c, r, g, b] = frame.pixels[y * frame.width + x];

            const int paintR = useColor ? r : 255;
            const int paintG = useColor ? g : 255;
            const int paintB = useColor ? b : 255;

            auto drawChar = [&](const unsigned char c, const int startX, const int startY)
            {
                if (c < 32 || c >= 128) return;

                const stbtt_bakedchar* bData = &charData[c - 32];
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

            int screenX = x * charWidth;
            int screenY = y * charHeight;

            drawChar(c, screenX, screenY);
        }
    }

    stbi_write_png(outputPath.c_str(), imgWidth, imgHeight, 3, canvas.data(), imgWidth * 3);
    std::cout << "Successfully exported PNG to: " << outputPath << "\n";
}