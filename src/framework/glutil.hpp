#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <random>
#include <iostream>

#include "consts.hpp"
#include "camera.hpp"


namespace framework
{
    // Variables for first person camera
    std::unique_ptr<Camera> camera;
    float lastX = WINDOWSIZEX / 2.0f;
    float lastY = WINDOWSIZEY / 2.0f;
    bool firstMouse = true;

    std::random_device rd;		                        // Obtaining a "true" random seed
    std::mt19937 gen(rd());		                        // Seeding the Mersenne twister
    std::uniform_int_distribution<> randInt(0, 3);	    // Generates random integers in range [0, 3]

    // GLFW and window initialization
    GLFWwindow* initWindow(const std::string& title)
    {
        if (!glfwInit())
        {
            return nullptr;
        }

        // Window hints
        glfwWindowHint(GLFW_RESIZABLE, false);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        // Open a window and create its OpenGL context
        GLFWwindow* window = glfwCreateWindow(WINDOWSIZEX, WINDOWSIZEY, title.c_str(), nullptr, nullptr);
        if (window == nullptr)
        {
            glfwTerminate();
            return nullptr;
        }

        glfwMakeContextCurrent(window);

        GLenum error = glewInit();
        if (error != GLEW_OK)
        {
            glfwTerminate();
            return nullptr;
        }

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSwapInterval(1);
        return window;
    }

    void GLFWErrorCallback(int code, const char* description)
    {
        std::cerr << "Error " << "0x" << std::hex << code << ':' << description << "\n";
    }

    void GLAPIENTRY
        MessageCallback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam)
    {
        std::cerr << "GL CALLBACK:" << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
            "type = 0x" << type <<
            ", severity = 0x" << severity <<
            ", message =" << message << "\n";
    }

    void SetInputModeMouse(GLFWwindow* window)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    // Callback function for whenever mouse is moved
    void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera->ProcessInputMouse(xoffset, yoffset);
    }

    // Updates delta time
    void updateDeltaTime(GLfloat& dt, GLfloat& ct, GLfloat& lt)
    {
        ct = static_cast<GLfloat>(glfwGetTime());
        dt = ct - lt;
        lt = ct;
    }
}
