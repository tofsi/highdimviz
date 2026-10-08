#pragma once
#include "Projection.hpp"
#include <stdexcept>

ProjectedPoint project_columns(
    const DataPoint& point,
    const std::array<std::size_t, 3>& columns)
{
    ProjectedPoint result{};
    for (std::size_t axis = 0; axis < 3; ++axis){
        const DataValue& value = point.at(columns[axis]);
        if (!std::holds_alternative<double>(value))
        {
            throw std::runtime_error(
            "Projection requires numerical column");
        }
        result[axis] = std::get<double>(value);
    }

    return result;
}