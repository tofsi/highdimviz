#include "Camera.hpp"

#include <iostream>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
void expect(const glm::vec3& actual, const glm::vec3& expected)
{
    if (glm::length(actual - expected) > 0.0001f)
        throw std::runtime_error("Camera coordinate check failed.");
}

glm::vec3 eye(const Camera& camera)
{
    return glm::vec3(glm::inverse(camera.view_matrix())[3]);
}
}

int main()
{
    try
    {
        Camera camera;
        expect(eye(camera), glm::vec3(0, 0, 3));
        expect(glm::vec3(camera.view_matrix() * glm::vec4(0, 0, 0, 1)),
            glm::vec3(0, 0, -3));
        camera.move_local(glm::vec3(0, 0, -1));
        expect(eye(camera), glm::vec3(0, 0, 2));

        Camera rolled;
        rolled.rotate_local(0, 0, glm::radians(90.0f));
        rolled.move_local(glm::vec3(0, 1, 0));
        expect(eye(rolled), glm::vec3(0, 1, 3));

        Camera loop;
        for (int i = 0; i < 360; ++i)
            loop.rotate_local(0, glm::radians(1.0f), 0);
        loop.move_local(glm::vec3(0, 0, -1));
        expect(eye(loop), glm::vec3(0, 0, 2));
        std::cout << "Initial orientation, local movement, roll, and full-loop checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
