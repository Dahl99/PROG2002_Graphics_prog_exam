#pragma once

#include <memory>
#include <vector>
#include <string>
#include <GL/glew.h>

#include "vao.hpp"
#include "vbo.hpp"
#include "shader.hpp"

namespace framework
{
	class Skybox
	{
	private:
		GLuint m_RendererID;

		std::unique_ptr<VertexArray> vao;
		std::unique_ptr<VertexBuffer> vbo;

	public:
		Skybox(std::vector<std::string> texturePaths);

		void Draw(Shader& shader, glm::mat4& view, glm::mat4& proj);
	};
}