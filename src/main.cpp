#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SFML/Audio.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cassert>

#include <map>
#include <string>
#include <iostream>
#include <fstream>

#include "chip8.hpp"
#include "keys.hpp"

static GLFWwindow *
setupWindow(int width, int height, const char * title);

static std::vector<uint8_t>
readAllBytes(const char * path);

static std::string
readAllChars(const char * path);

static cee::Keys
getKeyStates(GLFWwindow * window);

static GLuint
makeShader(GLenum type, const std::string & src);

static GLuint
makeProgram(std::vector<GLuint> shaders);

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

static inline float
mapRangeWidth(float index);

static inline float
mapRangeHeight(float index);

static std::map<GLFWwindow *, uint8_t>
lastKeyPressed;

static constexpr int
WIDTH = 800;

static constexpr int
HEIGHT = 400;

static constexpr const char *
TITLE = "Chip8 Emulator";

static constexpr float
PX_WIDTH = (1.0f / WIDTH) * (WIDTH / 64.0f);

static constexpr float
PX_HEIGHT = (1.0f / HEIGHT) * (HEIGHT / 32.0f);

int main(int argc, char ** argv)
{
    auto pathToRom = std::string();

    if (argc == 2)
    {
        pathToRom = argv[1];
    }
    else
    {
        printf("Chip8 Error: Wrong number of arguments\n");
        return -1;
    }

    sf::SoundBuffer beepSnd;
    if (! beepSnd.loadFromFile("data/sounds/beep.wav"))
    {
        printf("Chip8 Error: Can't load the beeping sound.\n");
        return -1;
    }

    sf::Sound sndSrc;
    sndSrc.setBuffer(beepSnd);
    sndSrc.setLoop(false);

    auto window = setupWindow(WIDTH, HEIGHT, TITLE);

    cee::Chip8 chip;
    chip.loadProgram(readAllBytes(pathToRom.c_str()));

    constexpr GLfloat pxVerts[] =
    {
        -1.0f,  1.0f, 0.0f, // Top Left
         1.0f,  1.0f, 0.0f, // Top Right
        -1.0f, -1.0f, 0.0f, // Bottom Left
         1.0f, -1.0f, 0.0f  // Bottom Right
    };

    constexpr GLuint pxIndices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    // Initialize the VAO and other buffers associated
    // with drawing an emulated pixel.
    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    {
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pxVerts), &pxVerts, GL_STATIC_DRAW);

        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pxIndices), &pxIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    // Current Vertex Shader
    const auto pxVertexSrc = readAllChars("data/shaders/px_vertex.glsl");
    const auto pxVertex = makeShader(GL_VERTEX_SHADER, pxVertexSrc);

    // Current Fragment Shader
    const auto pxFragmentSrc = readAllChars("data/shaders/px_fragment.glsl");
    const auto pxFragment = makeShader(GL_FRAGMENT_SHADER, pxFragmentSrc);

    // Current Shader Program
    const auto pxProgram = makeProgram({pxVertex, pxFragment});
    glUseProgram(pxProgram);

    glfwShowWindow(window);
    while (! glfwWindowShouldClose(window))
    {
        chip.updateKeys(getKeyStates(window));
        chip.updateCycle();

        if (chip.isBeeping() && sndSrc.getStatus() != sf::SoundSource::Playing)
            sndSrc.play();

        // Clear back buffer and background color.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glBindVertexArray(vao);
        const auto gfx = chip.getGfx();
        for (int i = 0; i < 32; ++i)
        {
            // Maps the width resolution [0-HEIGHT] to [-1.0-1.0]
            auto y = - mapRangeHeight(i);
            auto l = i * 64;

            for (int j = 0; j < 64; ++j)
            {
                if (gfx[l + j] == 1)
                {
                    // Maps the width resolution [0-WIDTH] to [-1.0-1.0]
                    auto x = mapRangeWidth(j);
                    auto ident = glGetUniformLocation(pxProgram, "PxModel");
                    auto model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(x, y, 0.0f));
                    model = glm::scale(model, glm::vec3(PX_WIDTH, PX_HEIGHT, 1.0f));
                    glUniformMatrix4fv(ident, 1, GL_FALSE, glm::value_ptr(model));
                    glDrawElements(GL_TRIANGLES, sizeof(pxIndices), GL_UNSIGNED_INT, nullptr);
                }
            }
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    glDeleteProgram(pxProgram);
    glDeleteShader(pxVertex);
    glDeleteShader(pxFragment);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
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

        if (keyboardLayout.count(k) && a == GLFW_PRESS)
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

cee::Keys
getKeyStates(GLFWwindow * window)
{
    cee::Keys keys;

    for (auto & pair : keyboardLayout)
    {
        auto state = pair.first;
        auto index = pair.second;
        auto offset = (1 << index);
        auto result = (glfwGetKey(window, state) == GLFW_PRESS ? offset : 0);
        keys.keysPressed |= result;
    }

    keys.lastKeyPressed = lastKeyPressed[window];

    return keys;
}

std::vector<uint8_t>
readAllBytes(const char * path)
{
    try
    {
        std::ifstream file;
        file.exceptions(std::ios::failbit);
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

std::string
readAllChars(const char * path)
{
    try
    {
        std::ifstream file;
        file.exceptions(std::ios::failbit);
        file.open(path, std::ios::ate);
        auto length = file.tellg();
        std::string result(length, ' ');
        file.seekg(0, std::ios::beg);
        file.read(&result[0], length);
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

    return "";
}

GLuint
makeShader(GLenum type, const std::string & src)
{
    const auto id = glCreateShader(type);
    const auto cp = src.c_str();
    glShaderSource(id, 1, &cp, nullptr);
    glCompileShader(id);
    return id;
}

GLuint
makeProgram(std::vector<GLuint> shaders)
{
    const auto id = glCreateProgram();

    for (const auto s : shaders)
        glAttachShader(id, s);

    glLinkProgram(id);
    return id;
}


float
mapRangeWidth(float index)
{
    constexpr auto h = static_cast<float>(WIDTH);
    constexpr auto x = 2.0f / h;
    constexpr auto s = h / 64.0f;
    return (x * index * s) - 0.98f;
}

float
mapRangeHeight(float index)
{
    constexpr auto h = static_cast<float>(HEIGHT);
    constexpr auto x = 2.0f / h;
    constexpr auto s = h / 32.0f;
    return (x * index * s) - 0.96f;
}

