#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Camera.hpp"
#include "Dataset.hpp"
#include "Projection.hpp"
#include "SceneFit.hpp"
#include "SphereRenderer.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
constexpr glm::vec3 background(0.8f, 0.0f, 0.8f);

void error_callback(int code, const char* description)
{
    std::cerr << "GLFW error " << code << ": " << description << '\n';
}

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::vector<PlotPoint> make_plot_points(const DataSet& data)
{
    std::vector<ProjectedPoint> projected;
    projected.reserve(data.points.size());
    for (const DataPoint& point : data.points)
        projected.push_back(project_columns(point, {2, 0, 3}));
    const SceneFit fit = calculate_scene_fit(projected);

    std::vector<PlotPoint> markers;
    markers.reserve(projected.size());
    for (std::size_t row = 0; row < projected.size(); ++row)
    {
        const ProjectedPoint scene = apply_scene_fit(projected[row], fit);
        PlotPoint marker;
        marker.source_row = row;
        marker.position = glm::vec3(static_cast<float>(scene[0]),
            static_cast<float>(scene[1]), static_cast<float>(scene[2]));
        markers.push_back(marker);
    }
    return markers;
}

void check_rendering(int width, int height)
{
    unsigned char corner[4]{}, center[4]{};
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, corner);
    glReadPixels(width / 2, height / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, center);
    bool background_matches = true;
    bool sphere_visible = false;
    for (int channel = 0; channel < 3; ++channel)
    {
        const int expected = static_cast<int>(std::lround(background[channel] * 255));
        background_matches &= std::abs(static_cast<int>(corner[channel]) - expected) <= 2;
        sphere_visible |= std::abs(static_cast<int>(center[channel]) - expected) > 10;
    }
    if (glGetError() != GL_NO_ERROR || !background_matches || !sphere_visible)
        throw std::runtime_error("OpenGL rendering check failed.");
    std::cout << "Context, shaders, sphere drawing, and pixel checks passed\n";
}

int run(GLFWwindow* window, bool smoke_test)
{
    const DataSet data = load_csv("data/gaussian_example.csv");
    const std::vector<PlotPoint> points = make_plot_points(data);
    std::cout << "Loaded " << points.size() << " plot points\n";
    SphereRenderer renderer;
    renderer.set_points(points);
    Camera camera;

    // A large centered marker makes the GPU smoke test deterministic.
    if (smoke_test)
        renderer.set_points({PlotPoint{0, glm::vec3(0.0f), 0.5f}});

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(smoke_test ? 0 : 1);
    double previous_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        const double current_time = glfwGetTime();
        const float delta_time = static_cast<float>(
            std::min(current_time - previous_time, 0.1));
        previous_time = current_time;
        camera.update(window, delta_time);

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (width > 0 && height > 0)
        {
            glViewport(0, 0, width, height);
            glClearColor(background.x, background.y, background.z, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            const glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
            renderer.draw(camera.view_matrix(), projection);
            if (smoke_test)
                check_rendering(width, height);
            glfwSwapBuffers(window);
        }
        else if (smoke_test)
            throw std::runtime_error("Smoke test framebuffer is empty.");
        if (smoke_test)
            break;
    }
    return 0;
}
}

int main(int argc, char* argv[])
{
    const bool smoke_test = argc == 2 && std::string_view(argv[1]) == "--smoke-test";
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    struct GlfwLifetime { ~GlfwLifetime() { glfwTerminate(); } } lifetime;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, smoke_test ? GLFW_FALSE : GLFW_TRUE);
    const std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window(
        glfwCreateWindow(1280, 720, "highdimviz", nullptr, nullptr), glfwDestroyWindow);
    if (!window)
        return 1;
    glfwMakeContextCurrent(window.get());
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to load OpenGL with GLAD\n";
        return 1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n'
              << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    try
    {
        // All GPU owners inside run() are destroyed before the window/context.
        return run(window.get(), smoke_test);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
