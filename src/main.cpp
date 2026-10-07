#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstddef>

#include <cstdlib>
#include <iostream>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <numbers>

namespace
{
void error_callback(int code, const char* description)
{
    std::cerr << "GLFW error " << code << ": " << description << '\n';
}

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLuint compile_shader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* source_text = source.c_str();
    glShaderSource(shader, 1, &source_text, nullptr);
    glCompileShader(shader);

    // Check compile success
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    // report failures
    if (success == GL_FALSE)
    {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(log_length > 0? log_length : 1, '\0');
        glGetShaderInfoLog(shader, log_length, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed:\n" + log);
    }

    return shader;
}

// Geometry helpers
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

void generate_sphere(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    int stacks, int sectors, float radius)
{
    vertices.clear();
    indices.clear();
    // generate vertices
    for (int i=0;i<=stacks;i++){
        for (int j=0;j<=sectors;j++){
            float theta = std::numbers::pi_v<float>
            * static_cast<float>(i) / static_cast<float>(stacks);
            float phi = 2.0f * std::numbers::pi_v<float>
            * static_cast<float>(j) / static_cast<float>(sectors);
            glm::vec3 normal(
                std::sin(theta) * std::cos(phi),
                std::cos(theta),
                std::sin(theta)* std::sin(phi));
            vertices.push_back({radius*normal, normal});
        }
    }
    // generate triangle indices
    for (int i=0;i<stacks;i++){
        for (int j=0;j<sectors;j++){
            unsigned int a = static_cast<unsigned int>(i * (sectors + 1) + j);
            unsigned int b = a + static_cast<unsigned int>(sectors + 1);

            indices.insert(indices.end(),{
                a, a + 1, b,
                a + 1, b + 1, b
            });
        }
    }
}
}

std::string read_shader_file(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Could not open shader file: " + path);
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

int main(int argc, char* argv[])
{
    const bool smoke_test = argc == 2 && std::string_view(argv[1]) == "--smoke-test";

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, smoke_test ? GLFW_FALSE : GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "highdimviz", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to load OpenGL with GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    GLuint program = 0;
    // Shaders!
    try
    {
        // Read shaders
        const std::string vertex_source =
            read_shader_file("shaders/sphere.vert");
        const std::string fragment_source =
            read_shader_file("shaders/sphere.frag");
        // Compile shaders
        GLuint vertex_shader =
            compile_shader(GL_VERTEX_SHADER, vertex_source);
        GLuint fragment_shader =
            compile_shader(GL_FRAGMENT_SHADER, fragment_source);
        std::cout << "both shaders compiled successfully\n";
        // Attach shaders
        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        // Check link status
        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        // Throw error
        if (success == GL_FALSE)
        {
            GLint log_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length > 0 ? log_length : 1, '\0');
            glGetProgramInfoLog(program, log_length, nullptr, log.data());
            glDeleteProgram(program);
            throw std::runtime_error("Shader linking failed:\n" + log);
        }
        std::cout << "Shader program linked successfully\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    // allocate sphere data to buffer
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    generate_sphere(vertices, indices, 32, 64, 1.0f);
    GLuint vertex_buffer = 0;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    const GLsizeiptr vertex_bytes =
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex));
    glBufferData(
        GL_ARRAY_BUFFER,
        vertex_bytes,
        vertices.data(),
        GL_STATIC_DRAW
    );
    // Check allocation
    //GLint uploaded_bytes = 0;
    //glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &uploaded_bytes);
    //std::cout << "Vertex buffer size: " << uploaded_bytes << " bytes\n";
    // VAO setup
    GLuint vertex_array = 0;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    GLuint index_buffer = 0;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    const GLsizeiptr index_bytes = 
        static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int));
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        index_bytes,
        indices.data(),
        GL_STATIC_DRAW);
    // Check allocation
    /* GLint uploaded_index_bytes = 0;
    glGetBufferParameteriv(
        GL_ELEMENT_ARRAY_BUFFER,
        GL_BUFFER_SIZE,
        &uploaded_index_bytes);

    std::cout << "Index buffer size: "
            << uploaded_index_bytes << " bytes\n"; */
    
    glBindVertexArray(0); // Unbind vertex_array
    
    // Create matrices for vertex shader
    glEnable(GL_DEPTH_TEST);
    const GLint model_location = glGetUniformLocation(program, "model");
    const GLint view_location = glGetUniformLocation(program, "view");
    const GLint projection_location = glGetUniformLocation(program, "projection");
    const glm::mat4 model(1.0f);
    const glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Set window sizes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(smoke_test ? 0 : 1);

    int exit_code = 0;
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glClearColor(0.10f, 0.18f, 0.22f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glfwGetFramebufferSize(window, &width, &height);
        if (width > 0 && height > 0)
        {
            const glm::mat4 projection = glm::perspective(
                glm::radians(45.0f),
                static_cast<float>(width) / static_cast<float>(height),
                0.1f, 100.0f);
            glUseProgram(program);
            glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
            glBindVertexArray(vertex_array);
            glDrawElements(
                GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        if (smoke_test)
        {
            // Reading a pixel verifies that OpenGL rendered the expected clear color.
            unsigned char pixel[4]{};
            glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
            const bool color_matches = std::abs(static_cast<int>(pixel[0]) - 26) <= 2
                && std::abs(static_cast<int>(pixel[1]) - 46) <= 2
                && std::abs(static_cast<int>(pixel[2]) - 56) <= 2;
            if (glGetError() != GL_NO_ERROR || !color_matches)
            {
                std::cerr << "OpenGL rendering check failed\n";
                exit_code = 1;
            }
            else
            {
                std::cout << "GLFW context, GLAD loading, and rendering check passed\n";
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        if (smoke_test)
        {
            break;
        }
    }
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &index_buffer);
    glfwDestroyWindow(window);
    glfwTerminate();
    return exit_code;
}
