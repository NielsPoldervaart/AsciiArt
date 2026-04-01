#pragma once
#include <string>

class Image
{
public:
    std::string filePath;
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixelData = nullptr;

    explicit Image(const std::string& path);
    ~Image();

    void PrintInfo() const;
    [[nodiscard]] bool IsValid() const;
    void Resize(int targetWidth, float fontRatio = 2.0f);
};