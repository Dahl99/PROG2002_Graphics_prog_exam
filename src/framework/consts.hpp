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

	const glm::vec3 TREEPOSITIONS[] = {
		glm::vec3(295.f, 68.5f, 100.f),
		glm::vec3(280.f, 58.f, 500.f),
		glm::vec3(400.f, 67.5f, 205.f),
		glm::vec3(330.f, 66.f, 300.f),
		glm::vec3(500.f, 58.f, 400.f),
		glm::vec3(460.f, 71.f, 600.f),
		glm::vec3(460.f, 66.5f, 900.f),
		glm::vec3(550.f, 67.3f, 970.f),
		glm::vec3(950.f, 44.5f, 1000.f),
		glm::vec3(1000.f, 48.f, 880.f),
		glm::vec3(1050.f, 65.f, 730.f),
		glm::vec3(750.f, 75.5f, 200.f),
		glm::vec3(650.f, 65.5f, 300.f)
	};

	const std::string HEIGHTMAPPATH = "../res/HeightMaps/Gjovik_Height Map.png";

	const std::string SPHEREMODELPATH = "../res/models/sun_moon/sphere.obj";
	const std::string SUNTEXTUREPATH = "../res/models/sun_moon/sun.png";
	const std::string MOONTEXTUREPATH = "../res/models/sun_moon/moon.png";

	const std::string PINETREEMODELPATH = "../res/Objects/PineTree2/10447_Pine_Tree_v1_L3b.obj";
	const std::string PINETREETEXTUREPATH = "../res/Objects/PineTree2/10447_Pine_Tree_v1_Diffuse.png";

	const std::string LIGHTSRCVERTSHADERPATH = "../res/shaders/lightsrc.vert";
	const std::string LIGHTSRCFRAGSHADERPATH = "../res/shaders/lightsrc.frag";
	
	const std::string TERRAINVERTSHADERPATH = "../res/shaders/terrain.vert";
	const std::string TERRAINFRAGSHADERPATH = "../res/shaders/terrain.frag";
	
	const std::string ENTITIESVERTSHADERPATH = "../res/shaders/entities.vert";
	const std::string ENTITIESFRAGSHADERPATH = "../res/shaders/entities.frag";

	
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
