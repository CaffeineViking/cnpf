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
    includedirs {"include"}
    filter {"system:windows"} -- Inconsistent...
        links {"glfw3dll", "glew32", "opengl32"}
    filter {"system:linux or bsd or macosx"}
        links {"glfw", "GLEW", "GL"}

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
    files {"src/**.cc"}
    files {"tests/main.cc"}
    removefiles {"src/main.cc"}
    includedirs {"include"}
    filter {"system:windows"} -- Inconsistent...
        links {"glfw3dll", "glew32", "opengl32"}
    filter {"system:linux or bsd or macosx"}
        links {"glfw", "GLEW", "GL"}
