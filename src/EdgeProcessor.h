#pragma once
#include "Image.h"
#include <vector>

class EdgeProcessor
{
public:
    static std::vector<char> GeneratePooledEdgeMap(const Image& highResImg, int targetWidth, int targetHeight,
                                                   float edgeThreshold);
};