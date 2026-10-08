
#include "Dataset.hpp"
#include <csv.hpp>
#include <cmath>
#include <stdexcept>
#include <utility>

DataSet load_csv(const std::string& path)
{   
    // Define the csv format
    // TODO: Add support for no header, custom delimiters
    csv::CSVFormat format;
    format.delimiter(',');
    format.header_row(0);
    format.trim({' ', '\t'});
    format.variable_columns(csv::VariableColumnPolicy::THROW);

    csv::CSVReader reader(path, format);
    DataSet result;
    result.variable_names = reader.get_col_names();
    if (result.variable_names.empty())
        throw std::runtime_error("CSV has no column names");
    
    std::size_t data_row = 0;
    for (auto& row : reader)
    {
        ++data_row;
        DataPoint point;
        point.reserve(result.variable_names.size());

        for (std::size_t column = 0;
            column < result.variable_names.size(); ++column)
        {
            try
            {
                const double value = row[column].get<double>();
                if (!std::isfinite(value))
                    throw std::runtime_error("Value is not finite.");
                point.emplace_back(value);
            }
            catch(const std::exception& e)
            {
                throw std::runtime_error(
                    "CSV data row " + std::to_string(data_row) +
                    ", column '" + result.variable_names[column] +
                    "': " + e.what());
            }
        }
        result.points.push_back(std::move(point));
    }
    if (result.points.empty())
        throw std::runtime_error("CSV contains no data rows");
    return result;
}