---- premake5
name = "cnpf"
workspace (name)
    language "C++"
    cppdialect "C++11"
    location "build"
    warnings "Extra"
    -- platforms {"Static", "Shared"}
    configurations {"Debug", "Release"}
    filter {"configurations:Debug"}
        defines {"DEBUG"}
        optimize "Off"
        symbols "On"
    filter {"configurations:Release"}
        defines {"RELEASE"}
        optimize "Speed"
        symbols "Off"

------ Program
project (name)
    targetdir "bin"
    kind "WindowedApp"
    files {"src/**.cc"}
    files {"src/**.cpp"}
    files {"src/**.c"}
    includedirs {"include"}
    filter {"system:windows"} -- Inconsistent...
        defines {"WINDOWS"}
        libdirs {"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.0/lib/x64"}
        libdirs {"bin/"}
        links {"OpenCL", "glew32", "glfw3", "opengl32", "AntTweakBar64"}
        buildoptions { " -static -static-libgcc -static-libstdc++", "-mwindows", "-mconsole" }
    filter {"system:linux or bsd or macosx"}
        links {"OpenCL", "glfw", "GLEW", "AntTweakBar", "GL"}
        defines {"UNIX"}

---------------- Testing
project (name.."-tests")
    targetdir "bin"
    kind "ConsoleApp"
    files {"src/**.cpp"}
    files {"src/**.c"}
    files {"tests/**.cpp"}
    removefiles {"src/main.cpp"}
    includedirs {"include"}
    filter {"system:windows"} -- Inconsistent...
        defines {"WINDOWS"}
        libdirs {"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.0/lib/x64"}
        libdirs {"bin/"}
        links {"OpenCL", "glew32", "glfw3", "opengl32", "AntTweakBar64"}
        buildoptions { " -static -static-libgcc -static-libstdc++", "-mwindows", "-mconsole"}
    filter {"system:linux or bsd or macosx"}
        links {"OpenCL", "glfw", "GLEW", "AntTweakBar", "GL"}
        defines {"UNIX"}
