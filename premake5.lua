dependencies = { base = path.getrelative(os.getcwd(), path.getabsolute("deps")) }

function dependencies.load()
    dir = path.join(dependencies.base, "*.lua")
    deps = os.matchfiles(dir)

    for i, dep in pairs(deps) do
        dep = dep:gsub(".lua", "")
        require(dep)
    end
end

dependencies.load()
-------------------------------------------------
workspace "menu-tool"
    startproject "menu-tool"
    location "./build"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"
    targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
    targetname "%{prj.name}"

    configurations { "debug", "release" }

    if os.istarget("darwin") then
        platforms { "x64", "arm64" }
    else
        platforms { "x86", "x64" }
    end

    filter "platforms:x86"
        architecture "x86"
    filter {}

    filter "platforms:x64"
        architecture "x86_64"
    filter {}

    filter "platforms:arm64"
        architecture "ARM64"
    filter {}

    filter { "language:C++", "toolset:not msc*" }
	    buildoptions "-std=c++20"
    filter {}

    filter "toolset:msc*"
        buildoptions "/bigobj"
        buildoptions "/Zc:__cplusplus"
        buildoptions "/std:c++20"
    filter {}

    filter { "system:windows" }
	    systemversion "latest"
    filter {}

    filter { "system:macosx" }
	    systemversion "12.0"
    filter {}

    symbols "On"
    staticruntime "On"
    warnings "Extra"

    if os.istarget("linux") or os.istarget("darwin") then
        buildoptions "-pthread"
        linkoptions "-pthread"
    end

    filter "configurations:release"
        optimize "Full"
        defines { "NDEBUG" }
        flags { "FatalCompileWarnings" }
    filter {}

    filter "configurations:debug"
        optimize "Debug"
        defines { "DEBUG", "_DEBUG" }
    filter {}

project "menu-tool"
    kind "ConsoleApp"
    language "C++"
    targetname "menu-tool"

    files {
        "./include/**.hpp",
        "./src/utils/**.hpp",
        "./src/utils/**.cpp",
        "./src/menu/**.hpp",
        "./src/menu/**.cpp",
        "./src/tool/**.hpp",
        "./src/tool/**.cpp"
    }

    includedirs {
        "./include",
    }

    fmt:link()

group "Dependencies"
    fmt:project()
