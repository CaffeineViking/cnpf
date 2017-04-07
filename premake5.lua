---- premake5
name = "cnpf"
workspace (name)
    language "C++"
    flags {"C++11"}
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
        links {"OpenCL", "glew32", "glfw3dll", "opengl32"}
        buildoptions { " -static -static-libgcc -static-libstdc++", "-mwindows", "-mconsole" }
    filter {"system:linux or bsd or macosx"}
        links {"OpenCL", "glfw", "GLEW", "GL"}
        defines {"UNIX"}

-- ------ Library
-- project (name)
--     targetdir "lib"
--     files {"src/**.cc"}
--     includedirs {"include"}
--     -- links {"library"}
--     filter {"platforms:Static"}
--         defines {"STATIC"}
--         kind "StaticLib"
--     filter {"platforms:Shared"}
--         defines {"SHARED"}
--         kind "SharedLib"

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
        links {"OpenCL", "glew32", "glfw3dll", "opengl32"}
        buildoptions { " -static -static-libgcc -static-libstdc++", "-mwindows", "-mconsole"}
    filter {"system:linux or bsd or macosx"}
        links {"OpenCL", "glfw", "GLEW", "GL"}
        defines {"UNIX"}
