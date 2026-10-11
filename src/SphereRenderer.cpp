#include "SphereRenderer.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

void generate_sphere(std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices, int stacks, int sectors)
{
    for (int i = 0; i <= stacks; ++i)
    {
        const float theta = std::numbers::pi_v<float>
            * static_cast<float>(i) / static_cast<float>(stacks);
        for (int j = 0; j <= sectors; ++j)
        {
            const float phi = 2.0f * std::numbers::pi_v<float>
                * static_cast<float>(j) / static_cast<float>(sectors);
            const glm::vec3 normal(std::sin(theta) * std::cos(phi),
                std::cos(theta), std::sin(theta) * std::sin(phi));
            vertices.push_back({normal, normal});
        }
    }
    for (int i = 0; i < stacks; ++i)
        for (int j = 0; j < sectors; ++j)
        {
            const auto a = static_cast<unsigned int>(i * (sectors + 1) + j);
            const auto b = a + static_cast<unsigned int>(sectors + 1);
            indices.insert(indices.end(), {a, a + 1, b, a + 1, b + 1, b});
        }
}
}

SphereRenderer::SphereRenderer()
    : shader_("shaders/sphere.vert", "shaders/sphere.frag"),
      model_location_(shader_.uniform_location("model")),
      view_location_(shader_.uniform_location("view")),
      projection_location_(shader_.uniform_location("projection"))
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    generate_sphere(vertices, indices, 4, 8);
    index_count_ = static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1, &vertex_array_);
    glBindVertexArray(vertex_array_);
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
        indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

SphereRenderer::~SphereRenderer()
{
    glDeleteVertexArrays(1, &vertex_array_);
    glDeleteBuffers(1, &vertex_buffer_);
    glDeleteBuffers(1, &index_buffer_);
}

void SphereRenderer::set_points(const std::vector<PlotPoint>& points)
{
    std::vector<glm::mat4> models;
    models.reserve(points.size());
    for (const PlotPoint& marker : points)
    {
        if (!std::isfinite(marker.radius) || marker.radius <= 0.0f ||
            !std::isfinite(marker.position.x) ||
            !std::isfinite(marker.position.y) || !std::isfinite(marker.position.z))
            throw std::invalid_argument("Sphere position/radius must be finite; radius must be positive.");
        models.push_back(glm::scale(
            glm::translate(glm::mat4(1.0f), marker.position),
            glm::vec3(marker.radius)));
    }
    models_.swap(models);
}

void SphereRenderer::draw(const glm::mat4& view, const glm::mat4& projection) const
{
    shader_.use();
    shader_.set_matrix(view_location_, view);
    shader_.set_matrix(projection_location_, projection);
    glBindVertexArray(vertex_array_);
    for (const glm::mat4& model : models_)
    {
        shader_.set_matrix(model_location_, model);
        glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}
