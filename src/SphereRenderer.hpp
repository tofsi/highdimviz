#pragma once

#include "PlotPoint.hpp"
#include "ShaderProgram.hpp"
#include <vector>

class SphereRenderer
{
public:
    SphereRenderer();
    ~SphereRenderer();
    SphereRenderer(const SphereRenderer&) = delete;
    SphereRenderer& operator=(const SphereRenderer&) = delete;

    void set_points(const std::vector<PlotPoint>& points);
    void draw(const glm::mat4& view, const glm::mat4& projection) const;

private:
    ShaderProgram shader_;
    GLint model_location_, view_location_, projection_location_;
    GLuint vertex_array_ = 0;
    GLuint vertex_buffer_ = 0;
    GLuint index_buffer_ = 0;
    GLsizei index_count_ = 0;
    std::vector<glm::mat4> models_;
};
