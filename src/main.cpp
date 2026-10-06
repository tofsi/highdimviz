#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <string_view>

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

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

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
        glClear(GL_COLOR_BUFFER_BIT);

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

    glfwDestroyWindow(window);
    glfwTerminate();
    return exit_code;
}
