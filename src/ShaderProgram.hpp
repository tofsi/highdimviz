#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

// GPU resources must be created and destroyed with a current OpenGL context.
class ShaderProgram
{
public:
    ShaderProgram(const std::string& vertex_path, const std::string& fragment_path);
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;
    GLint uniform_location(const char* name) const;
    void set_matrix(GLint location, const glm::mat4& matrix) const;

private:
    GLuint program_ = 0;
};
