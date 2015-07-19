// Third party library dependencies
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// STL Library
#include <iostream>

int main(int argc, char ** argv)
{
    glfwSetErrorCallback([](int err, const char * desc)
    {
        std::cerr << "GLFW Error: " << desc << "\n";
    });

    if (! glfwInit())
    {
        std::cerr << "GLEW Error: Could not initialise GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(800, 600, "Chip8 Emulator", nullptr, nullptr);

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
        return -1;
    }

    // Sync NDC Coordinates with window resolution
    {
        int w, h; // width, height
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
    }

    glfwShowWindow(window);
    while (! glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Clear back buffer and background color.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glfwSwapBuffers(window);
    }

    // Cleanup resources
    glfwTerminate();
    return 0;
}
