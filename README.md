# Assignment 2: 3D Pac-Man

## About

This project contains my work on the PROG2002 Graphics programming exam autumn 2020. The program reads a given heightmap and renders it with vertices having different color
depending on their height. Brown is the lowest having height < 42, green is in the middle being 42 < height > 84 and white having height > 84. The scene also contains some
textured trees loaded from a model that are scattered around the scene. There's also a plane that flies around randomly in all 3 dimensions, as well as there being 3 deer
and 3 moose roaming randomly around that are affected by gravity and collides with the map.

The map is also being lit up using the blinn-phong model by both a sun and a
moon that are rotating around the map, essentially creating a day/night cycle with the sunlight varying depending on time of day. The current day number and time of day is
being displayed in the upper left corner as well. The scene also has a skybox and can be navigated around in 1st person while listening to a soothing song.

## Built With

This exam project was built using the following libraries:

* OpenGL ver. 4.3
* GLEW ver. 2.1.0
* GLFW ver. 3.3.2
* GLM ver. 0.9.9
* ImGui
* IrrKlang
* STB
* tinyobjloader

## Getting Started

### Windows

This is an example on how to set up the project locally using Visual Studio 2019 with Cmake addon.

#### Prerequisites

Before you begin make sure you have the following installed:

* Visual Studio 2019
* CMake addon for Visual Studio 2019

#### Installation

1. Clone the repository using git
2. Right click on the root folder containing all files in repository
3. Click on "Open in Visual Studio"
4. The project will now be open in Visual Studio and Cmake should automatically generate the project
5. The project should now be set up locally and is ready to build

### Linux

As I developed the project on Windows; I have no idea how to set it up on Linux. But it should be possible to just use CMake as well.

## Creators

This project was developed by:

* Daniel Dahl

## Acknowledgements

* Collision between animals themselves or trees is not implemented. Only collision with ground is
  
* I tried rotating the animal models so they would look in the direction they were going, but was unable to figure out how to rotate along two axes

* I also at some point changed the folder name of some folders in the "res" directory, but for some reason they weren't updated in the repository and the program is still functioning properly even though the path constants aren't 100% accurate
