cmake_minimum_required(VERSION 3.12)

project(GraphicsExam)

find_package(OpenGL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(glm_DIR external/glm/cmake/glm)
find_package(glm REQUIRED)


add_subdirectory(external/glfw-3.3.2)
add_subdirectory(external/glew-cmake-2.1.0)
add_subdirectory(external/stb)
add_subdirectory(external/tiny_obj_loader)

add_library(irrKlang SHARED IMPORTED)
if (WIN32)
  set_target_properties(irrKlang PROPERTIES LINKER_LANGUAGE CXX
                                  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/bin/winx64-visualStudio/irrKlang.dll"
                                  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/bin/winx64-visualStudio/ikpMP3.dll"
                                  IMPORTED_IMPLIB   "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/lib/Winx64-visualStudio/irrKlang.lib"
                                  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/include")
endif (WIN32)
if (UNIX)
  set_target_properties(irrKlang PROPERTIES LINKER_LANGUAGE CXX
                                  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/bin/linux-gcc/irrKlang.so"
                                  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/bin/linux-gcc/ikpMP3.so"
                                  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/include")
endif (UNIX)

set_target_properties(irrKlang PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(irrKlang PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


add_executable(GraphicsExam
	

	src/framework/shader.hpp
	src/framework/shader.cpp

	src/framework/texture.hpp
	src/framework/texture.cpp

	src/framework/vao.hpp
	src/framework/vao.cpp
	
	src/framework/vbo.hpp
	src/framework/vbo.cpp
	
	src/framework/ibo.hpp
	src/framework/ibo.cpp
	
	src/framework/vbl.hpp

	src/framework/renderer.hpp
	src/framework/renderer.cpp

	src/framework/model.hpp
	src/framework/model.cpp

	src/framework/entity.hpp
	src/framework/entity.cpp

	src/framework/camera.hpp
	src/framework/camera.cpp
	
	src/framework/heightmap.hpp
	src/framework/heightmap.cpp
	
	src/framework/skybox.hpp
	src/framework/skybox.cpp
	
	src/framework/consts.hpp
	src/framework/glutil.hpp

	src/main.cpp

	external/imgui/imconfig.h
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui.h
    external/imgui/imgui_impl_glfw.cpp
    external/imgui/imgui_impl_glfw.h
    external/imgui/imgui_impl_opengl3.cpp
    external/imgui/imgui_impl_opengl3.h
    external/imgui/imgui_internal.h
    external/imgui/imgui_widgets.cpp)


target_compile_definitions(GraphicsExam PRIVATE GLEW_STATIC)

# Set include directories
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/external/imgui
	${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/bin/winx64-visualStudio
    ${CMAKE_CURRENT_LIST_DIR}/external/irrKlang/include/

)


target_link_libraries(GraphicsExam
	libglew_static
	glfw
	glm::glm
	stbimage::stbimage
	tinyobj::tinyobj
	irrKlang
	OpenGL::GL)

# Copy resources to binary directory
add_custom_target(
    copy_res ALL
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
    COMMAND ${CMAKE_COMMAND} -E echo "Copied resource directory to binary directory ${CMAKE_CURRENT_BINARY_DIR}"
)

file(COPY "external/irrKlang/bin/winx64-visualStudio/irrKlang.dll"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})
	
file(COPY "external/irrKlang/bin/winx64-visualStudio/ikpMP3.dll"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})
	
file(COPY "external/irrKlang/bin/linux-gcc/ikpMP3.so"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})

file(COPY "external/irrKlang/bin/linux-gcc/libIrrKlang.so"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})
