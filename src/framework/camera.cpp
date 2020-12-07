#include "camera.hpp"

namespace framework {
	Camera::Camera(glm::vec3 Position, glm::vec3 Up, float Yaw, float Pitch)
		: Front(glm::vec3(0.f, 0.f, -1.f)), MovementSpeed(50.f), MouseSensitivity(0.1f)
	{
		this->Position = Position;
		this->WorldUp = Up;
		this->Yaw = Yaw;
		this->Pitch = Pitch;

		Camera::updateCameraVectors();
	}

	void Camera::ProcessInputKeyboard(framework::Direction dir, float dt)
	{
		const float vel = MovementSpeed * dt;

		switch (dir)
		{
		case Direction::FORWARD:
			Position += Front * vel;
			break;
		case Direction::BACK:
			Position -= Front * vel;
			break;
		case Direction::RIGHT:
			Position += Right * vel;
			break;
		case Direction::LEFT:
			Position -= Right * vel;
			break;
		}
	}

	void Camera::ProcessInputMouse(float xoffset, float yoffset, bool constrainPitch)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		Camera::updateCameraVectors();
	}

	void Camera::updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		// Re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
}
