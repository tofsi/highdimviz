#pragma once
#include <cstddef>
#include <glm/glm.hpp>

struct PlotPoint
{
    std::size_t source_row{};
    glm::vec3 position{};
    float radius = 0.005f;
};