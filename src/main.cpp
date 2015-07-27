#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib>

#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <array>

#include "chip8.hpp"

static GLFWwindow *
setupWindow(int width, int height, const char * title);

static std::vector<uint8_t>
readAllBytes(const char * path);

std::array<uint8_t, 17>
getKeyStates(GLFWwindow * window);

static std::map<int, uint8_t>
keyboardLayout
{
    {GLFW_KEY_X, 0},
    {GLFW_KEY_1, 1},
    {GLFW_KEY_2, 2},
    {GLFW_KEY_3, 3},
    {GLFW_KEY_Q, 4},
    {GLFW_KEY_W, 5},
    {GLFW_KEY_E, 6},
    {GLFW_KEY_A, 7},
    {GLFW_KEY_S, 8},
    {GLFW_KEY_D, 9},
    {GLFW_KEY_Z, 0xA},
    {GLFW_KEY_C, 0xB},
    {GLFW_KEY_4, 0xC},
    {GLFW_KEY_R, 0xD},
    {GLFW_KEY_F, 0xE},
    {GLFW_KEY_V, 0xF}
};

static std::map<GLFWwindow *, uint8_t>
lastKeyPressed;

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

    // Register window to its own last key press state.
    lastKeyPressed.emplace(window, GLFW_KEY_UNKNOWN);

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

        if (keyboardLayout.count(k))
        {
            lastKeyPressed[w] = keyboardLayout[k];
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

std::array<uint8_t, 17>
getKeyStates(GLFWwindow * window)
{
    std::array<uint8_t, 17> keys;

    for (auto & pair : keyboardLayout)
    {
        auto state = pair.first;
        auto index = pair.second;
        keys[index] = (glfwGetKey(window, state) == GLFW_PRESS ? 1 : 0);
    }

    // Last item of the array should store the most recent key press.
    keys[0x10] = lastKeyPressed[window];

    return keys;
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
