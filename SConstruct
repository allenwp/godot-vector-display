#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", "thirdparty/"])
sources = Glob("src/*.cpp")
sources += Glob("src/output/*.cpp")

env.Append(CPPPATH=["thirdparty/asio/common/", "thirdparty/asio/host/", "thirdparty/asio/host/pc/"])
sources += Glob("thirdparty/asio/common/asio.cpp")
sources += Glob("thirdparty/asio/host/asiodrivers.cpp")
sources += Glob("thirdparty/asio/host/pc/asiolist.cpp")
# These libs were copied from the ASIO SDK sample host project. Some are probably not needed(?)
env["LIBS"].extend(["odbc32", "odbccp32", "winmm", "kernel32", "user32", "gdi32", "winspool", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid"])

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libvectordisplay.{}.{}.framework/libvectordisplay.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/libvectordisplay{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
