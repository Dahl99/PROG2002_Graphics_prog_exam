#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

namespace framework
{
	//-------------------------------------------------------------------------------------
	//									Constants
	//-------------------------------------------------------------------------------------

	const int WINDOWSIZEX = 1280, WINDOWSIZEY = 720;

	const std::string HEIGHTMAPPATH = "../res/HeightMaps/Snaasa_HeightMap.png";

	const std::string SUNMODELPATH = "../res/models/sun/sphere.obj";
	const std::string SUNTEXTUREPATH = "../res/models/sun/sun.png";

	const std::string VERTLIGHTSRCSHADERPATH = "../res/shaders/lightsrc.vert";
	const std::string FRAGLIGHTSRCSHADERPATH = "../res/shaders/lightsrc.frag";
	
	const std::string VERTSHADERPATH = "../res/shaders/shader.vert";
	const std::string FRAGSHADERPATH = "../res/shaders/shader.frag";

	
	//-------------------------------------------------------------------------------------
	//								Structs and Enums
	//-------------------------------------------------------------------------------------
	
	struct Vertex	// Struct used for vertices in 3D space
	{
		glm::vec3 pos;
		glm::vec3 nor;
		glm::vec2 tex;
	
		bool operator==(const Vertex& other) const
		{
			return (pos == other.pos
				&& nor == other.nor
				&& tex == other.tex);
		}
	};

	enum class Direction	// Enum class used for direction
	{
		FORWARD = 0,
		BACK = 2,
		RIGHT = 1,
		LEFT = 3
	};
}
