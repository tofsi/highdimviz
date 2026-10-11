#include "Camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

void Camera::move_local(const glm::vec3& displacement)
{
    position_ += orientation_ * displacement;
}

void Camera::rotate_local(float yaw, float pitch, float roll)
{
    orientation_ = glm::normalize(orientation_
        * glm::angleAxis(yaw, glm::vec3(0, 1, 0))
        * glm::angleAxis(pitch, glm::vec3(1, 0, 0))
        * glm::angleAxis(roll, glm::vec3(0, 0, 1)));
}

glm::mat4 Camera::view_matrix() const
{
    const glm::vec3 forward = orientation_ * glm::vec3(0, 0, -1);
    const glm::vec3 up = orientation_ * glm::vec3(0, 1, 0);
    return glm::lookAt(position_, position_ + forward, up);
}

void Camera::update(GLFWwindow* window, float delta_time)
{
    const auto key = [window](int code) -> float {
        return glfwGetKey(window, code) == GLFW_PRESS ? 1.0f : 0.0f;
    };
    const bool focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE;
    if (!focused || key(GLFW_KEY_ESCAPE))
    {
        if (captured_)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        captured_ = false;
        first_mouse_ = true;
    }
    else if (!captured_ &&
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        captured_ = true;
        first_mouse_ = true;
    }
    if (!captured_)
        return;

    double x = 0.0, y = 0.0;
    glfwGetCursorPos(window, &x, &y);
    const float dx = first_mouse_ ? 0.0f : static_cast<float>(x - previous_x_);
    const float dy = first_mouse_ ? 0.0f : static_cast<float>(y - previous_y_);
    previous_x_ = x;
    previous_y_ = y;
    first_mouse_ = false;
    rotate_local(-dx * mouse_sensitivity_, -dy * mouse_sensitivity_,
        (key(GLFW_KEY_Q) - key(GLFW_KEY_E)) * roll_speed_ * delta_time);

    const glm::vec3 movement(
        key(GLFW_KEY_D) - key(GLFW_KEY_A),
        key(GLFW_KEY_SPACE) - key(GLFW_KEY_LEFT_CONTROL),
        key(GLFW_KEY_S) - key(GLFW_KEY_W));
    if (glm::dot(movement, movement) > 0.0f)
        move_local(glm::normalize(movement) * movement_speed_ * delta_time);
}
