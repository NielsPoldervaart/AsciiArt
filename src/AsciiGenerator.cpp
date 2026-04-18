#include "AsciiGenerator.h"
#include "EdgeProcessor.h"
#include <string_view>
#include <algorithm>

namespace
{
    struct HSV
    {
        float h, s, v;
    };

    HSV RGBtoHSV(const float r, const float g, const float b)
    {
        const float max = std::max({r, g, b});
        const float min = std::min({r, g, b});
        const float chroma = max - min;

        HSV hsv{0.0f, 0.0f, max};

        if (hsv.v != 0.0f)
            hsv.s = chroma / hsv.v;

        if (chroma != 0.0f)
        {
            if (max == r)
                hsv.h = std::fmod((g - b) / chroma, 6.0f);
            else if (max == g)
                hsv.h = (b - r) / chroma + 2.0f;
            else
                hsv.h = (r - g) / chroma + 4.0f;

            hsv.h *= 60.0f;

            if (hsv.h < 0.0f)
                hsv.h += 360.0f;
        }
        return hsv;
    }

    void HSVtoRGB(const HSV& hsv, int& r, int& g, int& b)
    {
        const float c = hsv.v * hsv.s;
        const float x = c * (1.0f - std::abs(std::fmod(hsv.h / 60.0f, 2.0f) - 1.0f));
        const float m = hsv.v - c;

        float rf = 0, gf = 0, bf = 0;
        if (hsv.h >= 0 && hsv.h < 60)
        {
            rf = c;
            gf = x;
            bf = 0;
        }
        else if (hsv.h >= 60 && hsv.h < 120)
        {
            rf = x;
            gf = c;
            bf = 0;
        }
        else if (hsv.h >= 120 && hsv.h < 180)
        {
            rf = 0;
            gf = c;
            bf = x;
        }
        else if (hsv.h >= 180 && hsv.h < 240)
        {
            rf = 0;
            gf = x;
            bf = c;
        }
        else if (hsv.h >= 240 && hsv.h < 300)
        {
            rf = x;
            gf = 0;
            bf = c;
        }
        else
        {
            rf = c;
            gf = 0;
            bf = x;
        }

        r = static_cast<int>((rf + m) * 255.0f);
        g = static_cast<int>((gf + m) * 255.0f);
        b = static_cast<int>((bf + m) * 255.0f);
    }
}

AsciiFrame AsciiGenerator::GenerateStandard(const Image& img, const std::vector<char>& edgeMap, const float contrast,
                                            const bool retroColors, const float saturation, const float gamma,
                                            const bool dither)
{
    constexpr std::string_view asciiChars = " .:-=+*#%@";
    constexpr size_t numChars = asciiChars.length();

    AsciiFrame frame;
    frame.width = img.width;
    frame.height = img.height;
    frame.pixels.reserve(img.width * img.height);

    std::vector lumError(img.width * img.height, 0.0f);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int rawR = img.pixelData[index];
            const int rawG = img.pixelData[index + 1];
            const int rawB = img.pixelData[index + 2];

            float lum = (0.2126f * static_cast<float>(rawR) +
                0.7152f * static_cast<float>(rawG) +
                0.0722f * static_cast<float>(rawB)) / 255.0f;

            if (!retroColors && lum > 0.0f)
                lum = std::pow(lum, gamma);

            lum = std::clamp((lum - 0.5f) * contrast + 0.5f, 0.0f, 1.0f);

            if (dither)
                lum = std::clamp(lum + lumError[y * img.width + x], 0.0f, 1.0f);

            const auto charIndex = static_cast<size_t>(lum * (numChars - 1));
            char c = asciiChars[charIndex];

            if (dither)
            {
                const float quantizedLum = static_cast<float>(charIndex) / static_cast<float>(numChars - 1);
                const float error = lum - quantizedLum;

                if (x + 1 < img.width)
                    lumError[y * img.width + (x + 1)] += error * (7.0f / 16.0f);
                if (x - 1 >= 0 && y + 1 < img.height)
                    lumError[(y + 1) * img.width + (x - 1)] += error * (3.0f / 16.0f);
                if (y + 1 < img.height)
                    lumError[(y + 1) * img.width + x] += error * (5.0f / 16.0f);
                if (x + 1 < img.width && y + 1 < img.height)
                    lumError[(y + 1) * img.width + (x + 1)] += error * (1.0f / 16.0f);
            }

            if (const char edgeChar = edgeMap[y * img.width + x]; edgeChar != ' ')
                c = edgeChar;

            HSV hsv = RGBtoHSV(static_cast<float>(rawR) / 255.0f,
                               static_cast<float>(rawG) / 255.0f,
                               static_cast<float>(rawB) / 255.0f);

            if (retroColors)
            {
                hsv.v = 1.0f;
                hsv.h = std::round(hsv.h / 60.0f) * 60.0f;
                if (hsv.h >= 360.0f) hsv.h = 0.0f;
                hsv.s = (hsv.s < 0.25f) ? 0.0f : 1.0f;
            }
            else
            {
                if (hsv.v > 0.0f)
                    hsv.v = std::pow(hsv.v, gamma);

                hsv.s = std::clamp(hsv.s * saturation, 0.0f, 1.0f);
            }

            int finalR, finalG, finalB;
            HSVtoRGB(hsv, finalR, finalG, finalB);

            frame.pixels.push_back({c, finalR, finalG, finalB});
        }
    }
    return frame;
}

AsciiFrame AsciiGenerator::GenerateWordArt(const Image& img, const std::string& targetWord,
                                           const std::vector<char>& edgeMap, const float contrast,
                                           const bool retroColors, const float saturation, const float gamma,
                                           const bool dither)
{
    constexpr std::string_view shadingChars = " .:-=+*";
    constexpr size_t numShading = shadingChars.length();
    size_t wordIndex = 0;

    AsciiFrame frame;
    frame.width = img.width;
    frame.height = img.height;
    frame.pixels.reserve(img.width * img.height);

    std::vector lumError(img.width * img.height, 0.0f);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int index = (y * img.width + x) * img.channels;
            const int rawR = img.pixelData[index];
            const int rawG = img.pixelData[index + 1];
            const int rawB = img.pixelData[index + 2];

            float lum = (0.2126f * static_cast<float>(rawR) +
                0.7152f * static_cast<float>(rawG) +
                0.0722f * static_cast<float>(rawB)) / 255.0f;

            if (!retroColors && lum > 0.0f)
                lum = std::pow(lum, gamma);

            lum = std::clamp((lum - 0.5f) * contrast + 0.5f, 0.0f, 1.0f);

            if (dither)
                lum = std::clamp(lum + lumError[y * img.width + x], 0.0f, 1.0f);

            char c;
            float error = 0.0f;

            if (lum > 0.5f)
            {
                c = targetWord[wordIndex];
                wordIndex = (wordIndex + 1) % targetWord.length();

                if (dither)
                    error = lum - 1.0f;
            }
            else
            {
                const float shadowLum = std::clamp(lum * 2.0f, 0.0f, 1.0f);
                const auto charIndex = static_cast<size_t>(shadowLum * (numShading - 1));
                c = shadingChars[charIndex];

                if (dither)
                {
                    const float quantizedLum = static_cast<float>(charIndex) / static_cast<float>(numShading - 1);
                    error = lum - (quantizedLum / 2.0f); // Scale it back down to the 0.0 - 0.5 range
                }
            }

            if (dither && error != 0.0f)
            {
                if (x + 1 < img.width)
                    lumError[y * img.width + (x + 1)] += error * (7.0f / 16.0f);
                if (x - 1 >= 0 && y + 1 < img.height)
                    lumError[(y + 1) * img.width + (x - 1)] += error * (3.0f / 16.0f);
                if (y + 1 < img.height)
                    lumError[(y + 1) * img.width + x] += error * (5.0f / 16.0f);
                if (x + 1 < img.width && y + 1 < img.height)
                    lumError[(y + 1) * img.width + (x + 1)] += error * (1.0f / 16.0f);
            }

            if (const char edgeChar = edgeMap[y * img.width + x]; edgeChar != ' ')
                c = edgeChar;

            HSV hsv = RGBtoHSV(static_cast<float>(rawR) / 255.0f,
                               static_cast<float>(rawG) / 255.0f,
                               static_cast<float>(rawB) / 255.0f);

            if (retroColors)
            {
                hsv.v = 1.0f;
                hsv.h = std::round(hsv.h / 60.0f) * 60.0f;

                if (hsv.h >= 360.0f)
                    hsv.h = 0.0f;
                hsv.s = (hsv.s < 0.25f) ? 0.0f : 1.0f;
            }
            else
            {
                if (hsv.v > 0.0f)
                    hsv.v = std::pow(hsv.v, gamma);

                hsv.s = std::clamp(hsv.s * saturation, 0.0f, 1.0f);
            }

            int finalR, finalG, finalB;
            HSVtoRGB(hsv, finalR, finalG, finalB);

            frame.pixels.push_back({c, finalR, finalG, finalB});
        }
    }
    return frame;
}