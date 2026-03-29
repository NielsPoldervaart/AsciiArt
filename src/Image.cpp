#include "Image.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

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

void Image::Resize(const int targetWidth)
{
    if (!IsValid()) return;

    const float ratio = static_cast<float>(height)/ static_cast<float>(width);
    const int targetHeight = static_cast<int>(static_cast<float>(targetWidth) * ratio);

    const auto resizedData = static_cast<unsigned char*>(malloc(targetWidth * targetHeight * channels));

    stbir_resize_uint8_linear(pixelData, width, height, 0, resizedData, targetWidth, targetHeight, 0, (stbir_pixel_layout)channels);

    stbi_image_free(pixelData);

    pixelData = resizedData;
    width = targetWidth;
    height = targetHeight;
}