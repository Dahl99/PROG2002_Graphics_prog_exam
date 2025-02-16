#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <irrKlang.h>
#include <glm/trigonometric.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "framework/glutil.hpp"
#include "framework/texture.hpp"
#include "framework/renderer.hpp"
#include "framework/entity.hpp"
#include "framework/heightmap.hpp"
#include "framework/skybox.hpp"

void ProcessInput(GLFWwindow* window, const float dt);
void UpdateLightingUniforms(framework::Shader& shader, framework::Entity& sun, framework::Entity& moon, 
                            float& dt, float& daynightCycle, float& allTime, int& day);

void PlaneMovement(framework::Entity& plane, framework::Direction& lastDir, const float& dt, bool& yUp);
void AnimalMovement(framework::Entity& animal, framework::Direction& lastDir, const float& dt);

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
    renderer.EnableMultisampling();
    renderer.SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

    // Initializing music
    static irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
    static irrklang::ISound* music = soundEngine->play2D(framework::SOUNDTRACKPATH.c_str(), GL_TRUE, GL_FALSE, GL_TRUE);
    music->setVolume(framework::MUSICVOLUME);


    // Variables used to find delta time as well as variables for day/night cycle
    static GLfloat dt, curTime, lastTime, allTime, daynightCycle;
    dt = curTime = lastTime = allTime = daynightCycle = 0.0f;

    static int day = 0;

    // Initializing Camera object
    framework::camera = std::make_unique<framework::Camera>(glm::vec3(540.f, 70.f, 540.f));
    

    /**
     * Loading heightmap and fetching the vertices and indices from it
     */
    framework::Heightmap map(framework::HEIGHTMAPPATH);

    framework::VertexArray vao;			                // Initializing vao
    framework::VertexBuffer vbo(map.GetVertices());     // Initializing vbo
    framework::IndexBuffer ibo(map.GetIndices());       // Initializing ibo

    framework::VertexBufferLayout vbl;                  // Create a vertex buffer layout
    vbl.Push<GLfloat>(3);                               // Adding position floats to layout
    vbl.Push<GLfloat>(3);                               // Adding normal floats to layout
    vbl.Push<GLfloat>(2);                               // Adding tex coords floats to layout

    vao.AddBuffer(vbo, vbl);					        // Populating the vertex buffer

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

    for (const auto& pos : framework::TREEPOSITIONS)
    {
            auto temp = std::make_shared<framework::Entity>(pos, pinetreeModel->GetVertices(), pinetreeModel->GetIndices());
            temp->SetScale(glm::vec3(0.2f));
            temp->SetRotationAxis(glm::vec3(1.f, 0.f, 0.f));
            temp->SetRotation(270.f);
            pinetrees.push_back(temp);
    }

    // Creating plane entity
    framework::Entity plane(glm::vec3(540.f, 300.f, 200.f), framework::PLANEMODELPATH);
    plane.SetScale(glm::vec3(0.3f));
    plane.SetSpeed(80.f);
    bool yUp = true;
    framework::Direction planeLastDirection = framework::Direction::BACK;

    framework::Texture deerTexture(framework::DEERTEXTUREPATH);
    framework::Texture mooseTexture(framework::MOONTEXTUREPATH);

    std::vector<std::shared_ptr<framework::Entity>> animals;
    framework::Direction animalDirs[6] = {
        framework::Direction::BACK,
        framework::Direction::BACK,
        framework::Direction::BACK,
        framework::Direction::BACK,
        framework::Direction::BACK,
        framework::Direction::BACK
    };

    std::unique_ptr<framework::Model> deerModel = std::make_unique<framework::Model>(framework::DEERMODELPATH);
    for (int i = 0; i < 3; i++)
    {
        auto temp = std::make_shared<framework::Entity>(glm::vec3(300.f + (float)i*100.f, 70.f, 300.f + (float)i * 100.f), 
                                                        deerModel->GetVertices(), deerModel->GetIndices());
        temp->SetRotationAxis(glm::vec3(1.f, 0.f, 0.f));
        temp->SetRotation(270.f);
        animals.push_back(temp);
    }

    std::unique_ptr<framework::Model> mooseModel = std::make_unique<framework::Model>(framework::MOOSEMODELPATH);
    for (int i = 3; i < 6; i++)
    {
        auto temp = std::make_shared<framework::Entity>(glm::vec3(100.f + (float)i*100.f, 70.f, 100.f + (float)i * 50.f),
            mooseModel->GetVertices(), mooseModel->GetIndices());
        temp->SetRotationAxis(glm::vec3(1.f, 0.f, 0.f));
        temp->SetRotation(270.f);
        temp->SetScale(glm::vec3(0.3f));
        animals.push_back(temp);
    }


    // Model matrix for heightmap terrain as well as projection matrix
    auto terrainModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1.f));
    auto proj = glm::perspective(glm::radians(65.f), (float)framework::WINDOWSIZEX / (float)framework::WINDOWSIZEY, 0.1f, 1200.f);

    // Creating shaders: 1 for light sources, 1 for terrain and entities and 1 for skybox
    framework::Shader lightSrcShader(framework::LIGHTSRCVERTSHADERPATH, framework::LIGHTSRCFRAGSHADERPATH);
    framework::Shader shader(framework::VERTSHADERPATH, framework::FRAGSHADERPATH);
    framework::Shader skyboxShader(framework::SKYBOXVERTSHADERPATH, framework::SKYBOXFRAGSHADERPATH);

    framework::Skybox skybox(framework::SKYBOXTEXTURES);

    for (int i = 0; i < 2; i++)
    {
        shader.Bind();
        shader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].linear", 0.0014f);
        shader.SetUniform1f("u_PointLights[" + std::to_string(i) + "].quadratic", 0.000007f);
    }

    // ImGui setup
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;


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

        // Creating ImGui textbox for displaying current day and time of day
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Score", NULL, window_flags);
        ImGui::Text("Day: %d time: %.1f", day, daynightCycle);
        ImGui::End();


        // Processing input, updating plane movement and lighting uniforms
        ProcessInput(window, dt);
        PlaneMovement(plane, planeLastDirection, dt, yUp);
        UpdateLightingUniforms(shader, sun, moon, dt, daynightCycle, allTime, day);

        // Draw calls
        sunTexture.Bind();
        sun.Draw(lightSrcShader, framework::camera->GetViewMatrix(), proj);
        
        moonTexture.Bind();
        moon.Draw(lightSrcShader, framework::camera->GetViewMatrix(), proj);

        pinetreeTexture.Bind();
        shader.Bind();
        shader.SetUniform1i("u_isTextured", 1);
        for (auto& pinetree : pinetrees)
            pinetree->Draw(shader, framework::camera->GetViewMatrix(), proj);
        pinetreeTexture.Unbind();

        shader.SetUniform1i("u_isTextured", 0);
        plane.Draw(shader, framework::camera->GetViewMatrix(), proj);

        for (int i = 0; i < 6; i++)
        {
            animals.at(i)->ApplyGravity(dt);
            AnimalMovement(*animals.at(i), animalDirs[i],dt);
            glm::vec3 temp = map.CheckCollision(animals.at(i)->GetPosition());
            animals.at(i)->SetPosition(temp);
        }

        deerTexture.Bind();
        shader.Bind();
        shader.SetUniform1i("u_isTextured", 1);
        for (int i = 0; i < 3; i++)
            animals.at(i)->Draw(shader, framework::camera->GetViewMatrix(), proj);
        
        mooseTexture.Bind();
        for (int i = 3; i < 6; i++)
            animals.at(i)->Draw(shader, framework::camera->GetViewMatrix(), proj);

        shader.SetUniform1i("u_isTextured", 0);
        shader.SetUniformMat4f("u_Model", terrainModelMatrix);
        shader.SetUniformMat4f("u_Projection", proj);
        renderer.Draw(vao, ibo, shader);

        skybox.Draw(skyboxShader, framework::camera->GetViewMatrix(), proj);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // Exit the loop if escape is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
    }

    // Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

void UpdateLightingUniforms(framework::Shader& shader, framework::Entity& sun, 
                          framework::Entity& moon, float& dt, float& daynightCycle,
                          float& allTime, int& day)
{
    // Updating sun and moon position, making them rotate around the map
    sun.SetPosition(glm::vec3(glm::cos(allTime) * 700.f + 540.f, glm::sin(allTime) * 700.f, 540.f));
    moon.SetPosition(glm::vec3((glm::cos(allTime + 3.2f) * 700.f + 540.f), glm::sin(allTime + 3.2f) * 700.f, 540.f));

    shader.Bind();
    shader.SetUniformMat4f("u_View", framework::camera->GetViewMatrix());
    shader.SetUniform3fv("u_ViewPos", framework::camera->GetViewPosition());
    shader.SetUniform3fv("u_PointLights[0].position", sun.GetPosition());
    shader.SetUniform3fv("u_PointLights[1].position", moon.GetPosition());

    daynightCycle += dt;

    // dawn   - morning   - midday - evening   - dusk   - night
    // orange - yellowish - white  - yellowish - orange - dark blue

    if (daynightCycle <= 12.f)          // Setting sun color for dawn
    {
        shader.Bind();
        shader.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.25f, 0.1f));
        shader.SetUniform3fv("u_PointLights[1].color", glm::vec3(0.f));
    }
    else if (daynightCycle <= 24.f)     // Setting sun color for morning
    {
        shader.Bind();
        shader.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.87f, 0.2f));
    }
    else if (daynightCycle <= 36.f)     // Setting sun color for midday
    {
        shader.Bind();
        shader.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f));
    }
    else if (daynightCycle <= 48.f)     // Setting sun color for evening
    {
        shader.Bind();
        shader.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.87f, 0.2f));
    }
    else if (daynightCycle <= 60.f)     // Setting sun color for dusk
    {
        shader.Bind();
        shader.SetUniform3fv("u_PointLights[0].color", glm::vec3(1.0f, 0.25f, 0.1f));
    }
    else if (daynightCycle <= 120.f)    // Setting sun and moon color for night
    {
        shader.Bind();
        shader.SetUniform3fv("u_PointLights[0].color", glm::vec3(0.f));
        shader.SetUniform3fv("u_PointLights[1].color", glm::vec3(0.f, 0.f, 0.6f));
    }
    else
    {
        day++;
        daynightCycle = 0.0f;
    }
}

void PlaneMovement(framework::Entity& plane, framework::Direction& lastDir, const float& dt, bool& yUp)
{
    plane.Move(dt, lastDir);

    // Getting random number used for changing direction and position of plane
    int temp = framework::randInt(framework::gen);
    const auto planePos = plane.GetPosition();

    // Plane will go slowly up and down between y = 200 and y = 300
    if (planePos.y < 200.f) 
        yUp = true;

    if (planePos.y > 300.f)
        yUp = false;

    if (yUp)
        plane.SetPosition(glm::vec3(planePos.x, planePos.y + (plane.GetSpeed() * dt)/4, planePos.z));
    else if (!yUp)
        plane.SetPosition(glm::vec3(planePos.x, planePos.y - (plane.GetSpeed() * dt)/4, planePos.z));
    
    /**
     *  If x is < 100 or > 1000 plane will randomly change direction either forward or backwards
     *  The same goes for z except the randomly chosen direction will be either left or right
     */
    if (plane.GetPosition().x > 1000.f && lastDir == framework::Direction::RIGHT)
    {
        if (temp == 0) 
        {
            lastDir = framework::Direction::FORWARD;
            plane.SetRotation(180.f);
        }
        else
        {
            lastDir = framework::Direction::BACK;
            plane.SetRotation(0.f);
        }
    }
    else if (plane.GetPosition().z > 1000.f && lastDir == framework::Direction::BACK)
    {
        if (temp == 0)
        {
            lastDir = framework::Direction::LEFT;
            plane.SetRotation(270.f);
        }
        else
        {
            lastDir = framework::Direction::RIGHT;
            plane.SetRotation(90.f);
        }
    }
    else if (plane.GetPosition().x < 100.f && lastDir == framework::Direction::LEFT)
    {
        if (temp == 0)
        {
            lastDir = framework::Direction::FORWARD;
            plane.SetRotation(180.f);
        }
        else
        {
            lastDir = framework::Direction::BACK;
            plane.SetRotation(0.f);
        }
    }
    else if (plane.GetPosition().z < 100.f && lastDir == framework::Direction::FORWARD)
    {
        if (temp == 0)
        {
            lastDir = framework::Direction::LEFT;
            plane.SetRotation(270.f);
        }
        else
        {
            lastDir = framework::Direction::RIGHT;
            plane.SetRotation(90.f);
        }
    }
}

void AnimalMovement(framework::Entity& animal, framework::Direction& lastDir, const float& dt)
{
    animal.Move(dt, lastDir);

    // Getting random number used for changing direction and position of plane
    int temp = framework::randInt(framework::gen);
    const auto animalPos = animal.GetPosition();

    /**
     *  If x is < 300 or > 900 animals will randomly change direction either forward or backwards
     *  The same goes for z except the randomly chosen direction will be either left or right
     */
    if (animal.GetPosition().x > 900.f && lastDir == framework::Direction::RIGHT)
    {
        if (temp == 0)
            lastDir = framework::Direction::FORWARD;
        else
            lastDir = framework::Direction::BACK;
    }
    else if (animal.GetPosition().z > 900.f && lastDir == framework::Direction::BACK)
    {
        if (temp == 0)
            lastDir = framework::Direction::LEFT;
        else
            lastDir = framework::Direction::RIGHT;
    }
    else if (animal.GetPosition().x < 300.f && lastDir == framework::Direction::LEFT)
    {
        if (temp == 0)
            lastDir = framework::Direction::FORWARD;
        else
            lastDir = framework::Direction::BACK;
    }
    else if (animal.GetPosition().z < 300.f && lastDir == framework::Direction::FORWARD)
    {
        if (temp == 0)
            lastDir = framework::Direction::LEFT;
        else
            lastDir = framework::Direction::RIGHT;
    }
}
