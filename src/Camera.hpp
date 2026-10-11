#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct GLFWwindow;

class Camera
{
public:
    void update(GLFWwindow* window, float delta_time);
    void move_local(const glm::vec3& displacement);
    void rotate_local(float yaw, float pitch, float roll);
    glm::mat4 view_matrix() const;

private:
    glm::vec3 position_{0.0f, 0.0f, 3.0f};
    glm::quat orientation_ = glm::angleAxis(
        glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    float movement_speed_ = 1.0f;
    float mouse_sensitivity_ = 0.002f;
    float roll_speed_ = glm::radians(90.0f);
    bool captured_ = false;
    bool first_mouse_ = true;
    double previous_x_ = 0.0;
    double previous_y_ = 0.0;
};
