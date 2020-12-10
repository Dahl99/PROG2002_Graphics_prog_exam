#pragma once

#include <memory>
#include <vector>
#include <string>
#include <GL/glew.h>

#include "vao.hpp"
#include "vbo.hpp"
#include "shader.hpp"
//#include "ibo.hpp"

namespace framework
{
	class Skybox
	{
	private:
		GLuint m_RendererID;

		std::unique_ptr<VertexArray> vao;
		std::unique_ptr<VertexBuffer> vbo;
		//std::unique_ptr<IndexBuffer> ibo;

	public:
		Skybox(std::vector<std::string> texturePaths);

		void Draw(Shader& shader, glm::mat4& view, glm::mat4& proj);
	};
}