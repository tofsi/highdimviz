#pragma once
#include "SceneFit.hpp"
#include <stdexcept>
#include <cmath>
#include <numeric>

SceneFit calculate_scene_fit(const std::vector<ProjectedPoint>& points)
{
    if (points.empty())
    {
        throw std::invalid_argument("Point vector empty");
    }
    ProjectedPoint minimum = points.front();
    ProjectedPoint maximum = points.front();

    for (const ProjectedPoint& point : points)
    {
        for (std::size_t axis=0; axis<3; axis++)
        {
            if (!std::isfinite(point[axis]))
            {
                throw std::invalid_argument(
                    "Scene fitting requires finite coordinates"
                );
            }
            // Maximization/min algorithm
            minimum[axis] = std::min(minimum[axis], point[axis]);
            maximum[axis] = std::max(maximum[axis], point[axis]);
        }
    }
    // Calculate scene fit
    SceneFit fit;
    double largest_extent = 0.0;
    for (std::size_t axis = 0; axis < 3; ++axis)
    {
        fit.center[axis] = std::midpoint(minimum[axis], maximum[axis]);
        largest_extent = std::max(
            largest_extent,  maximum[axis] - minimum[axis]);
    }
    fit.scale = largest_extent > 0.0 ? 1.0 / largest_extent : 1.0;
    return fit;
}


ProjectedPoint apply_scene_fit(const ProjectedPoint& point, const SceneFit& fit)
{
    ProjectedPoint result;
    for (std::size_t axis = 0; axis < 3; ++axis){
        result[axis] = (point[axis] - fit.center[axis]) * fit.scale;
    }
    return result;
}
