#pragma once
#include "Projection.hpp"
#include <vector>

struct SceneFit
{
    ProjectedPoint center{};
    double scale = 1.0;
};

SceneFit calculate_scene_fit(const std::vector<ProjectedPoint>& points);
ProjectedPoint apply_scene_fit(const ProjectedPoint& point, const SceneFit& fit);
