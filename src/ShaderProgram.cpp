#include "ShaderProgram.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

namespace
{
std::string read_file(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Could not open shader file: " + path);
    std::ostringstream contents;
    contents << file.rdbuf();
    if (file.bad())
        throw std::runtime_error("Could not read shader file: " + path);
    return contents.str();
}

GLuint compile_shader(GLenum type, const std::string& source)
{
    const GLuint shader = glCreateShader(type);
    if (shader == 0)
        throw std::runtime_error("Could not create shader.");
    const char* text = source.c_str();
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, length, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed:\n" + log);
    }
    return shader;
}
}

ShaderProgram::ShaderProgram(
    const std::string& vertex_path, const std::string& fragment_path)
{
    const std::string vertex_source = read_file(vertex_path);
    const std::string fragment_source = read_file(fragment_path);
    GLuint vertex = 0, fragment = 0;
    try
    {
        vertex = compile_shader(GL_VERTEX_SHADER, vertex_source);
        fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
        program_ = glCreateProgram();
        if (program_ == 0)
            throw std::runtime_error("Could not create shader program.");
        glAttachShader(program_, vertex);
        glAttachShader(program_, fragment);
        glLinkProgram(program_);
        GLint success = GL_FALSE;
        glGetProgramiv(program_, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint length = 0;
            glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
            std::string log(length > 0 ? length : 1, '\0');
            glGetProgramInfoLog(program_, length, nullptr, log.data());
            throw std::runtime_error("Shader linking failed:\n" + log);
        }
    }
    catch (...)
    {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program_);
        throw;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(program_); }
void ShaderProgram::use() const { glUseProgram(program_); }

GLint ShaderProgram::uniform_location(const char* name) const
{
    const GLint location = glGetUniformLocation(program_, name);
    if (location < 0)
        throw std::runtime_error(std::string("Missing shader uniform: ") + name);
    return location;
}

void ShaderProgram::set_matrix(GLint location, const glm::mat4& matrix) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
