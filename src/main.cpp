#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <array>

#include "chip8.hpp"

static GLFWwindow *
setupWindow(int width, int height, const char * title);

static std::vector<uint8_t>
readAllBytes(const char * path);

static std::array<uint8_t, 16>
getKeyStates(GLFWwindow * window);

int main(int argc, char ** argv)
{
    auto window = setupWindow(800, 600, "Chip8 Emulator");

    cee::Chip8 chip;
    chip.loadProgram(readAllBytes("data/PONG"));

    glfwShowWindow(window);
    while (! glfwWindowShouldClose(window))
    {
        chip.updateKeys(getKeyStates(window));
        chip.updateCycle();

        // Clear back buffer and background color.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    glfwTerminate();
    return 0;
}

GLFWwindow *
setupWindow(int width, int height, const char * title)
{
    glfwSetErrorCallback([](int err, const char * desc)
    {
        std::cerr << "GLFW Error: " << desc << "\n";
    });

    if (! glfwInit())
    {
        std::cerr << "GLEW Error: Could not initialise GLFW\n";
        std::exit(-1);
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // Callback Parameters.
    // k: Key
    // s: Scan Code
    // a: Action
    // m: Key Mode
    glfwSetKeyCallback(window, [](GLFWwindow * w, int k, int s, int a, int m)
    {
        if (k == GLFW_KEY_ESCAPE && a == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(w, GL_TRUE);
        }
    });

    // Load GL extensions
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW Error: Could not initialise GLEW\n";
        std::exit(-1);
    }

    // Sync NDC Coordinates with window resolution
    {
        int w, h; // width, height
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
    }

    return window;
}

std::array<uint8_t, 16>
getKeyStates(GLFWwindow * window)
{
    return {};
}

std::vector<uint8_t>
readAllBytes(const char * path)
{
    try
    {
        std::ifstream file;
        file.exceptions(std::ios::badbit);
        file.open(path, std::ios::binary|std::ios::ate);
        auto length = file.tellg();
        std::vector<uint8_t> result(length);
        file.seekg(0, std::ios::beg);
        file.read((char*)&result[0], length);
        file.close();
        return result;
    }
    catch (std::ios_base::failure & err)
    {
        std::cerr << "File Error: Can't open file with path: " << path << "\n";
    }
    catch (std::bad_alloc & err)
    {
        std::cerr << "File Error: Can't open file with path: " << path << "\n";
    }

    return {};
}
