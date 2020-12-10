#pragma once

#include <string>
#include <vector>

#include "stb_image.h"
#include "consts.hpp"

namespace framework
{
	class Heightmap
	{
	private:
		stbi_uc* image;
		int w, h;

		std::vector<framework::Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

	public:
		Heightmap(const std::string& filepath);

		inline auto GetVertices() const { return m_Vertices; }	// Getter for vertices
		inline auto GetIndices() const { return m_Indices; }	// Getter for indices

		glm::vec3 CheckCollision(const glm::vec3 entity);

	private:
		void calculateNormals();
		void setIndices();
	};
}