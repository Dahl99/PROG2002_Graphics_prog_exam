#include "skybox.hpp"
#include <stb_image.h>
#include <iostream>
#include "consts.hpp"

namespace framework
{
	Skybox::Skybox(std::vector<std::string> texturePaths)
	{
		vao = std::make_unique<VertexArray>();									 // Initializing vao
		vbo = std::make_unique<VertexBuffer>(framework::SKYBOXVERTICES, 
											 sizeof(framework::SKYBOXVERTICES)); // Initializing vbo

		framework::VertexBufferLayout vbl;          // Create a vertex buffer layout
		vbl.Push<GLfloat>(3);                       // Adding position floats to layout

		vao->AddBuffer(*vbo, vbl);					// Populating the vertex buffer

		// Loading textures for all faces
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		stbi_set_flip_vertically_on_load(false);

		int w, h, numChannels;
		for (int i = 0; i < texturePaths.size(); i++)
		{
			unsigned char* image = stbi_load(texturePaths.at(i).c_str(), &w, &h, &numChannels, 0);
			if (image)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			}
			else
			{
				std::cout << "Failed to load cubemap texture at path: " << texturePaths.at(i) << std::endl;
			}
			
			stbi_image_free(image);	// Freeing image from memory
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void Skybox::Draw(Shader& shader, glm::mat4& view, glm::mat4& proj)
	{
		glDepthFunc(GL_LEQUAL);

		shader.Bind();
		vao->Bind();
		vbo->Bind();

		// Removing translation from view matrix and setting uniforms
		glm::mat4 temp = glm::mat4(glm::mat3(view));
		shader.SetUniformMat4f("u_View", temp);
		shader.SetUniformMat4f("u_Projection", proj);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS); // set depth function back to default
	}
}