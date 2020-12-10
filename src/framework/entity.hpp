#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "vao.hpp"
#include "vbo.hpp"
#include "ibo.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "consts.hpp"

namespace framework {

	class Entity 
	{
	protected:
		glm::vec3 position, rotationAxis, scale;
		GLfloat mvSpeed, rotation;

		std::unique_ptr<VertexArray> vao;
		std::unique_ptr<VertexBuffer> vbo;
		std::unique_ptr<IndexBuffer> ibo;
	
	public:
		Entity(glm::vec3 pos, std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
		Entity(glm::vec3 pos, const std::string& modelpath);

		// Setter for rotation axis vector
		inline void SetRotationAxis(glm::vec3 rotationAxis) { this->rotationAxis = rotationAxis; }	

		inline GLfloat GetRotation() const { return rotation; }						// Getter for rotation in degrees
		inline void SetRotation(GLfloat rotation) { this->rotation = rotation; }	// Setter for rotation in degrees					

		inline void SetScale(glm::vec3 scale) { this->scale = scale; }				// Setter for scale vector				

		inline glm::vec3 GetPosition() const { return position; };					// Getter for position vector
		inline void SetPosition(glm::vec3 position) { this->position = position; }	// Setter for position vector

		inline GLfloat GetSpeed() const { return mvSpeed; }							// Getter for movement speed
		inline void SetSpeed(const GLfloat mvSpeed) { this->mvSpeed = mvSpeed; }	// Setter for movement speed

		// Updates entity position based on direction using delta time
		void Move(const GLfloat& dt, const Direction& direction);

		void ApplyGravity(const float& dt);											// Applies gravity

		void Draw(Shader& shader, glm::mat4& view, glm::mat4& proj) const;			// Draws entity onto screen
	};
}