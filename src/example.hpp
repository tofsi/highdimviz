#pragma once

#include <string>
#include <vector>
#include "Dataset.hpp"
#include "Projection.hpp"

inline const DataSet exampleData = {
    {"a", "b", "c", "d"},
    {
        {2.0, 10.0, 4.0, 8.0},
        {3.0, 5.0, 6.0, 6.0}
    }};

const auto projected =
    project_columns(exampleData.points.at(0), {2, 0, 3});

std::vector<ProjectedPoint> projected_points;
    