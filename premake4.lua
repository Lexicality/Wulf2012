local trans = path.translate;
local join  = path.join;

local dest = "game";
local src  = "src";

local cdir = trans(path.getabsolute(os.getcwd()));

local destdir = join(cdir, dest);
local srcdir  = join(cdir, src);

local newdir;
local copy;
if (os.get() == "windows") then
    copy = "COPY /Y";
    newdir = "MKDIR";
else
    copy = "cp -f";
    newdir = "mkdir -p";
end

local function docmd(...)
    return table.concat({...}, ' ');
end

if (_ACTION == "vs2008" or _ACTION == "vs2005" or _ACTION == "vs2003" or _ACTION == "vs2002") then
    error("This project needs VS2010 or later!", 0);
end

-- make /game
if (not os.isdir("game")) then
    os.mkdir("game");
end


solution "Wulf2012"
    configurations { "Debug", "Release" }

    -- FORCE windows to be Win32: GLFW2.X doesn't support Win64
    if (os.get == "windows") then
        platforms { "x32" }
    end

    location "Build"

    project "WulfGame"
        location "Build"
        kind "ConsoleApp"
        language "C++"
        files {
            src .. "/**.h", src .. "/**.cpp";
            src .. "/OpenGL/stb_image.cc"; -- Force C++ to avoid a mildly annoying issue with CFLAGS
            src .. "/OpenGL/Shaders/*";
        }
        vpaths { [""] = src }

        includedirs { src }

        targetdir(dest)
        targetname("WulfGame")

        prebuildcommands {
            -- mkdir game/shaders
            docmd(newdir, join(destdir, "shaders"));
        }
        postbuildcommands {
            -- cp src/OpenGL/Shaders/* game/shaders
            docmd(copy, join(srcdir, trans("OpenGL/Shaders/*")), join(destdir, "shaders"));
        }

        flags { "FatalWarnings" }

        links { "glfw", "physfs" }

        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols" }

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimise" }

        configuration "windows"
            flags { "ExtraWarnings" }
            links { "opengl32", "glew32" }

        configuration "linux"
            buildoptions { "-std=c++0x", "-pedantic" }
            links { "GL", "GLEW" }

        configuration "macosx"
            --links { "OpenGL.Framework", "CoreFoundation.Framework" }
            linkoptions { "-stdlib=libc++", "-framework OpenGL", "-framework CoreFoundation" }
            buildoptions { "-std=c++11", "-stdlib=libc++" }

