#include "heightmap.hpp"
#include "texture.hpp"
#include "stb_image.h"

#include <iostream>

framework::Heightmap::Heightmap(const std::string& filepath) : image(nullptr), w(0), h(0)
{
	stbi_set_flip_vertically_on_load(true);
	image = stbi_load(filepath.c_str(), &w, &h, NULL, 1);

	int highest = 0, lowest = 1000;
	for (int x = 0; x < w; x++)
	{
		for (int z = 0; z < h; z++)
		{
			stbi_uc pixel = image[w * z + x];


			if (pixel < lowest)
				lowest = pixel;

			if (pixel > highest)
				highest = pixel;




			framework::Vertex temp{};
			temp.pos = { (float)x,
						 ((float)pixel/10.f),
						 (float)z };

			/*temp.nor = { attrib.normals[3 * index.normal_index + 0],
							  attrib.normals[3 * index.normal_index + 1],
							  attrib.normals[3 * index.normal_index + 2] };*/

			m_Vertices.push_back(temp);
		}
	}
	
	std::cout << lowest << '\t' << highest << '\t' << m_Vertices.size() << std::endl;

	for (int z = 0; z < h-1; z++)
	{
		for (int x = 0; x < h-1; x++)
		{
			const unsigned int topLeft = (z * h) + x;
			const unsigned int topRight = topLeft + 1;
			const unsigned int bottomLeft = ((z+1) * h) + x;
			const unsigned int bottomRight = bottomLeft + 1;

			m_Indices.push_back(topLeft);
			m_Indices.push_back(bottomLeft);
			m_Indices.push_back(topRight);
			m_Indices.push_back(topRight);
			m_Indices.push_back(bottomLeft);
			m_Indices.push_back(bottomRight);

			m_Vertices.at(bottomLeft).tex.x = 0.f;
			m_Vertices.at(bottomLeft).tex.y = 0.f;

			m_Vertices.at(bottomRight).tex.x = 1.f;
			m_Vertices.at(bottomRight).tex.y = 0.f;

			m_Vertices.at(topLeft).tex.x = 0.f;
			m_Vertices.at(topLeft).tex.y = 1.f;

			m_Vertices.at(topRight).tex.x = 1.f;
			m_Vertices.at(topRight).tex.y = 1.f;
		}
	}

	stbi_image_free(image);
}
