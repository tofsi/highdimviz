#pragma once

#include "Dataset.hpp"
#include <array>
#include <cstddef>

using ProjectedPoint = std::array<double, 3>;

ProjectedPoint project_columns(
    const DataPoint& point,
    const std::array<std::size_t, 3>& columns);