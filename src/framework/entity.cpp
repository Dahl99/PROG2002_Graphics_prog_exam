#include "entity.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace framework 
{
	Entity::Entity(glm::vec3 position, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) 
		: position(position), rotationAxis(glm::vec3(0.f, 1.f, 0.f)), scale(glm::vec3(1.f)), mvSpeed(5.0f), rotation(0.f)
	{
		vao = std::make_unique<VertexArray>();			// Initializing vao
		vbo = std::make_unique<VertexBuffer>(vertices); // Initializing vbo
		ibo = std::make_unique<IndexBuffer>(indices);	// Initializing ibo

		framework::VertexBufferLayout vbl;          // Create a vertex buffer layout
		vbl.Push<GLfloat>(3);                       // Adding position floats to layout
		vbl.Push<GLfloat>(3);                       // Adding normal floats to layout
		vbl.Push<GLfloat>(2);                       // Adding tex coords floats to layout

		vao->AddBuffer(*vbo, vbl);					// Populating the vertex buffer
	}

	Entity::Entity(glm::vec3 position, const std::string& modelpath) 
		: position(position), rotationAxis(0.f, 1.f, 0.f), scale(1.f, 1.f, 1.f), mvSpeed(5.0f), rotation(0.f)
	{
		model = std::make_unique<Model>(modelpath);					// Initializing model

		vao = std::make_unique<VertexArray>();						// Initializing vao
		vbo = std::make_unique<VertexBuffer>(model->GetVertices()); // Initializing vbo
		ibo = std::make_unique<IndexBuffer>(model->GetIndices());	// Initializing ibo

		framework::VertexBufferLayout vbl;          // Create a vertex buffer layout
		vbl.Push<GLfloat>(3);                       // Adding position floats to layout
		vbl.Push<GLfloat>(3);                       // Adding normal floats to layout
		vbl.Push<GLfloat>(2);                       // Adding tex coords floats to layout

		vao->AddBuffer(*vbo, vbl);					// Populating the vertex buffer
	}

	void Entity::Draw(Shader& shader, glm::mat4& view, glm::mat4& proj) const
	{
		shader.Bind();
		vao->Bind();
		vbo->Bind();
		ibo->Bind();

		// Setting the model matrix
		auto model = glm::translate(glm::mat4(1.f), position);
		model = glm::rotate(model, glm::radians(rotation), rotationAxis);
		model = glm::scale(model, scale);

		// Passing Model, View and Projection matrices to shader
		shader.SetUniformMat4f("u_Model", model);
		shader.SetUniformMat4f("u_View", view);
		shader.SetUniformMat4f("u_Projection", proj);

		glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Entity::Move(const GLfloat& dt, const Direction& direction)
	{
		// Updates position based on direction
		switch (direction)
		{
		case Direction::FORWARD:
			position.z -= dt * mvSpeed;
			break;
		case Direction::RIGHT:
			position.x += dt * mvSpeed;
			break;
		case Direction::BACK:
			position.z += dt * mvSpeed;
			break;
		case Direction::LEFT:
			position.x -= dt * mvSpeed;
			break;
		default:
			break;
		}
	}
}