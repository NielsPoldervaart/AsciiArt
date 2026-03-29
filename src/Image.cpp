#include "Image.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(const std::string& path)
{
    filePath = path;

    pixelData = stbi_load(filePath.c_str(), &width, &height, &channels, 3);

    if (pixelData != nullptr)
    {
        channels = 3;
    }
}

Image::~Image()
{
    if (pixelData != nullptr)
    {
        stbi_image_free(pixelData);
    }
}

void Image::PrintInfo() const
{
    std::cout << "Path: " << filePath << std::endl;

    if (IsValid())
    {
        std::cout << "Dimensions: " << width << "x" << height << std::endl;
    }
    else
    {
        std::cout << "Error: Not a valid image file or file not found!" << std::endl;
    }
}

bool Image::IsValid() const
{
    return pixelData != nullptr;
}