solution "cee"
    configurations {"Debug", "Release"}
        language "C++"

        defines {
            "GLFW_STATIC",
            "GLEW_STATIC"
        }

        libdirs {
            os.findlib("glfw3"),
            os.findlib("glew")
        }

    configuration {"gmake"}
        buildoptions {
            "-Wall",
            "-std=c++11"
        }

    configuration {"macosx"}
        links {
            "glfw3",
            "GLEW",
            "Cocoa.framework",
            "OpenGL.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

    configuration {"linux"}
        links {
            "glfw3",
            "GLEW",
            "GL",
            "X11",
            "Xxf86vm",
            "Xrandr",
            "Xinerama",
            "Xi",
            "Xcursor",
            "pthread"
        }

    configuration "Release"
        defines {"NDEBUG"}
        objdir "obj/release"
        targetdir "bin/release"
        kind "WindowedApp"
        flags {"Optimize"}

    configuration "Debug"
        defines {"DEBUG"}
        objdir "obj/debug"
        targetdir "bin/debug"
        kind "ConsoleApp"
        flags {"Symbols"}

    project "cee"
        location "build"
        files {
            "src/**.cpp",
            "src/**.hpp"
        }
