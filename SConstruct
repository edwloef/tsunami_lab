##
# @author Alexander Breuer (alex.breuer AT uni-jena.de)
#
# @section DESCRIPTION
# Entry-point for builds.
##
import SCons
import os

print("####################################")
print("### Tsunami Lab                  ###")
print("###                              ###")
print("### https://scalable.uni-jena.de ###")
print("####################################")
print()
print("runnning build script")

# configuration
vars = Variables()

vars.AddVariables(
    EnumVariable(
        "mode",
        "compile modes, option 'san' enables address and undefined behavior sanitizers",
        "release",
        allowed_values=("release", "debug", "release+san", "debug+san"),
    )
)

# exit in the case of unknown variables
if vars.UnknownVariables():
    print(
        "build configuration corrupted, don't know what to do with: "
        + str(vars.UnknownVariables().keys())
    )
    exit(1)

# create environment
env = Environment(variables=vars)
env.Tool("compilation_db")
env.CompilationDatabase()

# generate help message
Help(vars.GenerateHelpText(env))

# add default flags
env.Append(CXXFLAGS=["-std=c++17", "-fwrapv", "-Wall", "-Wextra", "-Wpedantic"])

# set optimization mode
if "debug" in env["mode"]:
    env.Append(CXXFLAGS=["-g"])
else:
    env.Append(CXXFLAGS=["-O3", "-flto=auto", "-fno-math-errno"])

# add sanitizers
if "san" in env["mode"]:
    env.Append(
        CXXFLAGS=[
            "-g",
            "-fsanitize=float-divide-by-zero",
            "-fsanitize=bounds",
            "-fsanitize=address",
            "-fsanitize=undefined",
            "-fno-omit-frame-pointer",
        ]
    )
else:
    env.Append(CXXFLAGS=["-Werror"])

# add Catch2
env.Append(CXXFLAGS=["-isystem", "submodules/Catch2/single_include"])

# add NetCDF
env.MergeFlags(["!nc-config --cflags", "!nc-config --libs"])

# set linker flags to match compiler flags
env.Append(LINKFLAGS=env["CXXFLAGS"])

# add nix header paths
env.Append(CPPPATH=os.environ.get("NIX_CFLAGS_COMPILE", "").split())

# set cxx
cxx = os.environ.get("CXX", "")
if cxx:
    env["CXX"] = cxx

# get source files
VariantDir(variant_dir="build/src", src_dir="src")

env.sources = []
env.tests = []

Export("env")
SConscript("build/src/SConscript")
Import("env")

env.Program(target="build/tsunami_lab", source=env.sources + env.standalone)

env.Program(target="build/tests", source=env.sources + env.tests)
