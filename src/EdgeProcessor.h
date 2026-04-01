#pragma once
#include "Image.h"
#include <vector>

class EdgeProcessor
{
public:
    static std::vector<char> GenerateEdgeMap(const Image& img, float edgeThreshold);
};