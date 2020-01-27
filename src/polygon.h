#pragma once
#include <vector>
#include "vertex.h"

// Simple abstraction of a polygon with a variable number of faces.
class Polygon
{
    public:
        std::vector<Vertex> vertices;
};
