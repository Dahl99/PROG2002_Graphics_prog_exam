#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <irrKlang.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "framework/glutil.hpp"
#include "framework/texture.hpp"
#include "framework/renderer.hpp"
#include "framework/entity.hpp"

void ProcessInput(GLFWwindow* window, const float dt);

//------------------------------------------------------------------------------------
//                                     Main
//------------------------------------------------------------------------------------
int main() 
{
    glfwSetErrorCallback(framework::GLFWErrorCallback);

    auto window = framework::initWindow("Graphics Exam");
    if (window == nullptr)
    {
        glfwTerminate();
        std::cin.get();
        return EXIT_FAILURE;
    }

    // Enable capture of debug output.
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(framework::MessageCallback, 0);

    glfwSetCursorPosCallback(window, framework::mouse_callback);
    framework::SetInputModeMouse(window);

    static framework::Renderer renderer;
    renderer.EnableBlending();
    renderer.EnableDepthTesting();
    renderer.SetClearColor(glm::vec4(0.3f, 0.0f, 0.3f, 1.0f));

    // Variables used to find delta time
    static GLfloat dt, curTime, lastTime;
    dt = curTime = lastTime = 0.0f;

    // Initializing Camera object
    framework::camera = std::make_unique<framework::Camera>(glm::vec3(0.f, 0.f, 5.f));

    framework::Entity cube(glm::vec3(0.f), "../res/models/cube/cube.obj");
    framework::Texture cubeTexture("../res/models/cube/cubetexture.png");
    framework::Shader shader(framework::VERTLIGHTSRCSHADERPATH, framework::FRAGLIGHTSRCSHADERPATH);

    auto proj = glm::perspective(glm::radians(65.f), framework::WINDOWSIZEX / framework::WINDOWSIZEY, 0.1f, 100.f);


//------------------------------------------------------------------------------------
//                                  Game loop
//------------------------------------------------------------------------------------
    
    while (!glfwWindowShouldClose(window))
    {
        //                   Preparation
        framework::updateDeltaTime(dt, curTime, lastTime);
        glfwPollEvents();

        renderer.Clear();   // Clearing screen

        ProcessInput(window, dt);
       
        cubeTexture.Bind();
        cube.Draw(shader, framework::camera->GetViewMatrix(), proj);

        glfwSwapBuffers(window);

        // Exit the loop if escape is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

void ProcessInput(GLFWwindow* window, const float dt)
{
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        framework::camera->ProcessInputKeyboard(framework::Direction::FORWARD, dt);
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        framework::camera->ProcessInputKeyboard(framework::Direction::BACK, dt);
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        framework::camera->ProcessInputKeyboard(framework::Direction::RIGHT, dt);
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        framework::camera->ProcessInputKeyboard(framework::Direction::LEFT, dt);
    }
}
