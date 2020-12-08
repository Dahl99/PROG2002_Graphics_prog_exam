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
#include "framework/heightmap.hpp"

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
    framework::camera = std::make_unique<framework::Camera>(glm::vec3(0.f, 15.f, 100.f));
    

    /**
     * Loading heightmap and fetching the vertices and indices from it
     */
    framework::Heightmap map(framework::HEIGHTMAPPATH);

    framework::VertexArray vao;			                // Initializing vao
    framework::VertexBuffer vbo(map.GetVertices());     // Initializing vbo
    framework::IndexBuffer ibo(map.GetIndices());       // Initializing ibo

    framework::VertexBufferLayout vbl;          // Create a vertex buffer layout
    vbl.Push<GLfloat>(3);                       // Adding position floats to layout
    vbl.Push<GLfloat>(3);                       // Adding normal floats to layout
    vbl.Push<GLfloat>(2);                       // Adding tex coords floats to layout

    vao.AddBuffer(vbo, vbl);					// Populating the vertex buffer

    // Creating sun and moon entity and loading their texture
    framework::Texture sunTexture(framework::SUNTEXTUREPATH);
    framework::Entity sun(glm::vec3(0.f, 50.f, 540.f), framework::SUNMODELPATH);
    sun.SetScale(glm::vec3(0.01f));

    framework::Texture moonTexture(framework::MOONTEXTUREPATH);
    framework::Entity moon(glm::vec3(1081.f, 500.f, 540.f), framework::MOONMODELPATH);
    moon.SetScale(glm::vec3(0.01f));

    // Model matrix for heightmap terrain as well as projection matrix
    auto terrainModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1.f));
    auto proj = glm::perspective(glm::radians(65.f), (float)framework::WINDOWSIZEX / (float)framework::WINDOWSIZEY, 0.1f, 1200.f);

    // Loading shader for light sources
    framework::Shader lightSrcShader(framework::LIGHTSRCVERTSHADERPATH, framework::LIGHTSRCFRAGSHADERPATH);
    framework::Shader terrainShader(framework::TERRAINVERTSHADERPATH, framework::TERRAINFRAGSHADERPATH);

    terrainShader.Bind();
    for (int i = 0; i < 2; i++)
    {
        terrainShader.SetUniform3fv("u_PointLights[" + std::to_string(i) + "].color", glm::vec3(1.0f));
        terrainShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].constant", 1.0f);
        terrainShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].linear", 0.0014f);
        terrainShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].quadratic", 0.000007f);
    }

    terrainShader.SetUniformMat4f("u_Model", terrainModelMatrix);
    terrainShader.SetUniformMat4f("u_Projection", proj);



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

        /**
         * Draw calls for terrain and sun
         */
        sunTexture.Bind();
        sun.Draw(lightSrcShader, framework::camera->GetViewMatrix(), proj);
        
        moonTexture.Bind();
        moon.Draw(lightSrcShader, framework::camera->GetViewMatrix(), proj);

        terrainShader.Bind();
        terrainShader.SetUniformMat4f("u_View", framework::camera->GetViewMatrix());
        terrainShader.SetUniform3fv("u_ViewPos", framework::camera->GetViewPosition());
        terrainShader.SetUniform3fv("u_PointLights[0].position", sun.GetPosition());
        terrainShader.SetUniform3fv("u_PointLights[1].position", moon.GetPosition());

        renderer.Draw(vao, ibo, terrainShader);

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
