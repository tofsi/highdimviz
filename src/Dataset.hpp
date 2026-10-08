// Definitions of data points and data set.
#pragma once

#include <string>
#include <variant>
#include <vector>

using DataValue = std::variant<double, std::string>; // Continuous or categoricaL
using DataPoint = std::vector<DataValue>;

struct DataSet
{
    std::vector<std::string> variable_names;
    std::vector<DataPoint> points;
};

DataSet load_csv(const std::string& path); //TODO: Define in cpp