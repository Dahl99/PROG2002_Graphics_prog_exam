#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "framework/consts.hpp"

namespace framework {

	class Camera
	{
	private:

        // Camera attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        // Euler angles
        float Yaw;
        float Pitch;

        // Camera options
        float MovementSpeed;
        float MouseSensitivity;

    public:
        Camera(glm::vec3 Position = glm::vec3(0.f), glm::vec3 Up = glm::vec3(0.f, 1.0f, 0.f), float Yaw = -90.f, float Pitch = 0.f);

        // Methods for processing both keyboard and mouse input
        void ProcessInputKeyboard(framework::Direction dir, float dt);
        void ProcessInputMouse(float xoffset, float yoffset, bool constrainPitch = true);

        // Getter for view matrix
        inline glm::mat4 GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }

        // Getter for view position
        inline glm::vec3 GetViewPosition() const { return Position; }

    private:
        void updateCameraVectors();
	};
}