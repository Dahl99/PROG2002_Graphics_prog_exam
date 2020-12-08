#include "heightmap.hpp"
#include "texture.hpp"
#include "stb_image.h"

#include <iostream>

framework::Heightmap::Heightmap(const std::string& filepath) : image(nullptr), w(0), h(0)
{
	// Loading heightmap image using stbi library
	stbi_set_flip_vertically_on_load(true);
	image = stbi_load(filepath.c_str(), &w, &h, NULL, 1);

	/**
	 * Looping through all pixels in heightmap 
	 */

	for (int x = 0; x < w; x++)
	{
		for (int z = 0; z < h; z++)
		{
			stbi_uc pixel = image[w * z + x];	// Getting pixel

			framework::Vertex temp{};			// Using x, pixel and z to set vertex coordinate
			temp.pos = { (float)x,
						 ((float)pixel),
						 (float)z };

			m_Vertices.push_back(temp);			// Adding vertex to vector containing all vertices
		}
	}
	
	Heightmap::calculateNormals();
	Heightmap::setIndices();

	stbi_image_free(image);	// Freeing image from memory
}

void framework::Heightmap::calculateNormals()
{
	// Looping through all vertices
	for (int x = 0; x < w; x++)
	{
		for (int z = 0; z < h; z++)
		{
			// Variables for each surrounding vertex's height
			float heightL, heightR, heightD, heightU;

			// Calculating x and z coordinate to be able to find surrounding vertices
			const int leftXPos = x - 1;
			const int rightXPos = x + 1;

			const int downZPos = z - 1;
			const int upZPos = z + 1;

			// If x or z isn't withing bounds of heightmap the height will be 0
			if (leftXPos < 0)
				heightL = 0.f;
			else
				heightL = m_Vertices.at(w * z + leftXPos).pos.y;

			if (rightXPos >= h)
				heightR = 0.f;
			else
				heightR = m_Vertices.at(w * z + rightXPos).pos.y;

			if (downZPos < 0)
				heightD = 0.f;
			else
				heightD = m_Vertices.at(w * downZPos + x).pos.y;

			if (upZPos >= h)
				heightU = 0.f;
			else
				heightU = m_Vertices.at(w * upZPos + x).pos.y;

			// Calculating the normal vector and normalizing it
			glm::vec3 normal(heightL - heightR, 2.f, heightD - heightU);
			normal = glm::normalize(normal);

			// Updating the normal
			m_Vertices.at(w * z + x).nor = normal;
		}
	}
}

void framework::Heightmap::setIndices()
{
	/**
	 * Looping through all vertices except last row and column
	 * to set the indices for the heightmap
	 */
	for (int z = 0; z < h - 1; z++)
	{
		for (int x = 0; x < h - 1; x++)
		{
			const unsigned int topLeft = (z * h) + x;
			const unsigned int topRight = topLeft + 1;
			const unsigned int bottomLeft = ((z + 1) * h) + x;
			const unsigned int bottomRight = bottomLeft + 1;

			m_Indices.push_back(topLeft);
			m_Indices.push_back(bottomLeft);
			m_Indices.push_back(topRight);
			m_Indices.push_back(topRight);
			m_Indices.push_back(bottomLeft);
			m_Indices.push_back(bottomRight);
		}
	}
}
