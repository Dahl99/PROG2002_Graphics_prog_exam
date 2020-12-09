#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <irrKlang.h>
#include <glm/trigonometric.hpp>
#include <random>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "framework/glutil.hpp"
#include "framework/texture.hpp"
#include "framework/renderer.hpp"
#include "framework/entity.hpp"
#include "framework/heightmap.hpp"

void ProcessInput(GLFWwindow* window, const float dt);
void UpdateLightingUniforms(framework::Shader& terrain, framework::Shader& entities, framework::Entity& sun, 
                            framework::Entity& moon, float& dt, float& daynightCycle, float& allTime);

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

    // Variables used to find delta time as well as variables for day/night cycle
    static GLfloat dt, curTime, lastTime, allTime,daynightCycle;
    dt = curTime = lastTime = allTime = daynightCycle = 0.0f;

    // Initializing Camera object
    framework::camera = std::make_unique<framework::Camera>(glm::vec3(540.f, 70.f, 540.f));
    

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
    std::unique_ptr<framework::Model> sphereModel = std::make_unique<framework::Model>(framework::SPHEREMODELPATH);

    framework::Texture sunTexture(framework::SUNTEXTUREPATH);
    framework::Entity sun(glm::vec3(0.f, 50.f, 540.f), sphereModel->GetVertices(), sphereModel->GetIndices());
    sun.SetScale(glm::vec3(0.01f));

    framework::Texture moonTexture(framework::MOONTEXTUREPATH);
    framework::Entity moon(glm::vec3(1081.f, 500.f, 540.f), sphereModel->GetVertices(), sphereModel->GetIndices());
    moon.SetScale(glm::vec3(0.01f));

    // Creating pine trees and loading pine tree texture
    framework::Texture pinetreeTexture(framework::PINETREETEXTUREPATH);
    std::vector<std::shared_ptr<framework::Entity>> pinetrees;
    auto pinetreeModel = std::make_unique<framework::Model>(framework::PINETREEMODELPATH);

    framework::Entity plane(glm::vec3(540.f, 300.f, 200.f), framework::PLANEMODELPATH);
    plane.SetScale(glm::vec3(0.3f));

    for (const auto& pos : framework::TREEPOSITIONS)
    {
            auto temp = std::make_shared<framework::Entity>(pos, pinetreeModel->GetVertices(), pinetreeModel->GetIndices());
            temp->SetScale(glm::vec3(0.2f));
            temp->SetRotationAxis(glm::vec3(1.f, 0.f, 0.f));
            temp->SetRotation(270.f);
            pinetrees.push_back(temp);
    }

    // Model matrix for heightmap terrain as well as projection matrix
    auto terrainModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1.f));
    auto proj = glm::perspective(glm::radians(65.f), (float)framework::WINDOWSIZEX / (float)framework::WINDOWSIZEY, 0.1f, 1200.f);

    // Creating shaders: 1 for light sources, 1 for terrain and 1 for textured entities
    framework::Shader lightSrcShader(framework::LIGHTSRCVERTSHADERPATH, framework::LIGHTSRCFRAGSHADERPATH);
    framework::Shader terrainShader(framework::TERRAINVERTSHADERPATH, framework::TERRAINFRAGSHADERPATH);
    framework::Shader entitiesShader(framework::ENTITIESVERTSHADERPATH, framework::ENTITIESFRAGSHADERPATH);

    terrainShader.Bind();
    for (int i = 0; i < 2; i++)
    {
        terrainShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].constant", 1.0f);
        terrainShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].linear", 0.0014f);
        terrainShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].quadratic", 0.000007f);
    }

    terrainShader.SetUniformMat4f("u_Model", terrainModelMatrix);
    terrainShader.SetUniformMat4f("u_Projection", proj);
    
    entitiesShader.Bind();
    for (int i = 0; i < 2; i++)
    {
        entitiesShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].constant", 1.0f);
        entitiesShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].linear", 0.0014f);
        entitiesShader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].quadratic", 0.000007f);
    }
    entitiesShader.SetUniformMat4f("u_Projection", proj);



//------------------------------------------------------------------------------------
//                                  Game loop
//------------------------------------------------------------------------------------
    
    while (!glfwWindowShouldClose(window))
    {
        //                   Preparation
        framework::updateDeltaTime(dt, curTime, lastTime);
        allTime += dt * 0.05f;

        glfwPollEvents();

        renderer.Clear();   // Clearing screen

        ProcessInput(window, dt);
        UpdateLightingUniforms(terrainShader, entitiesShader, sun, moon, dt, daynightCycle, allTime);

        // Draw calls
        sunTexture.Bind();
        sun.Draw(lightSrcShader, framework::camera->GetViewMatrix(), proj);
        
        moonTexture.Bind();
        moon.Draw(lightSrcShader, framework::camera->GetViewMatrix(), proj);

        pinetreeTexture.Bind();
        for (auto& pinetree : pinetrees)
            pinetree->Draw(entitiesShader, framework::camera->GetViewMatrix(), proj);

        plane.Draw(entitiesShader, framework::camera->GetViewMatrix(), proj);

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

void UpdateLightingUniforms(framework::Shader& terrain, framework::Shader& entities, framework::Entity& sun, 
                          framework::Entity& moon, float& dt, float& daynightCycle,
                          float& allTime)
{
    // Updating sun and moon position, making them rotate around the map
    sun.SetPosition(glm::vec3(glm::cos(allTime) * 700.f + 540.f, glm::sin(allTime) * 700.f, 540.f));
    moon.SetPosition(glm::vec3((glm::cos(allTime + 3.2f) * 700.f + 540.f), glm::sin(allTime + 3.2f) * 700.f, 540.f));

    terrain.Bind();
    terrain.SetUniformMat4f("u_View", framework::camera->GetViewMatrix());
    terrain.SetUniform3fv("u_ViewPos", framework::camera->GetViewPosition());
    terrain.SetUniform3fv("u_PointLights[0].position", sun.GetPosition());
    terrain.SetUniform3fv("u_PointLights[1].position", moon.GetPosition());

    entities.Bind();
    entities.SetUniform3fv("u_ViewPos", framework::camera->GetViewPosition());
    entities.SetUniform3fv("u_PointLights[0].position", sun.GetPosition());
    entities.SetUniform3fv("u_PointLights[1].position", moon.GetPosition());

    daynightCycle += dt;

    // dawn   - morning   - midday - evening   - dusk   - night
    // orange - yellowish - white  - yellowish - orange - dark blue

    if (daynightCycle <= 12.f)          // Setting sun color for dawn
    {
        terrain.Bind();
        terrain.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.25f, 0.1f));
        terrain.SetUniform3fv("u_PointLights[1].color", glm::vec3(0.f));

        entities.Bind();
        entities.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.25f, 0.1f));
        entities.SetUniform3fv("u_PointLights[1].color", glm::vec3(0.f));
    }
    else if (daynightCycle <= 24.f)     // Setting sun color for morning
    {
        terrain.Bind();
        terrain.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.87f, 0.2f));

        entities.Bind();
        entities.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.87f, 0.2f));
    }
    else if (daynightCycle <= 36.f)     // Setting sun color for midday
    {
        terrain.Bind();
        terrain.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f));
        
        entities.Bind();
        entities.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f));
    }
    else if (daynightCycle <= 48.f)     // Setting sun color for evening
    {
        terrain.Bind();
        terrain.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.87f, 0.2f));

        entities.Bind();
        entities.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.87f, 0.2f));

    }
    else if (daynightCycle <= 60.f)     // Setting sun color for dusk
    {
        terrain.Bind();
        terrain.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.25f, 0.1f));

        entities.Bind();
        entities.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.25f, 0.1f));
    }
    else if (daynightCycle <= 120.f)    // Setting sun and moon color for night
    {
        terrain.Bind();
        terrain.SetUniform3fv("u_PointLights[0].color", glm::vec3(0.f));
        terrain.SetUniform3fv("u_PointLights[1].color", glm::vec3(0.f, 0.f, 0.6f));
        
        entities.Bind();
        entities.SetUniform3fv("u_PointLights[0].color", glm::vec3(0.f));
        entities.SetUniform3fv("u_PointLights[1].color", glm::vec3(0.f, 0.f, 0.6f));
    }
    else daynightCycle = 0.0f;
}
