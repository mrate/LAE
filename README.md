# README #

### Building LuaAdventureEngine ###

* To build LuaAdventureEngine you need to use CMake. It's best to create 2 directories for RELEASE and DEBUG builds:

mkdir -p build/{release,debug}
cd builds/release
cmake -DCMAKE_BUILD_TYPE=Release ../../
cd ../debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../

This will create Makefile in each of the directory. To build a specific type just go to the directory and run make.

* Prerequisites

Projects has following dependencies (Ubuntu package name is in parenthesis at each library):

    CMake (cmake)
    gcc (gcc-4.9-multilib g++-4.9-multilib)
    boost 1.54 (libboost-all-dev)
    libpng (libpng12-dev)
    liblua5.1 liblua5.2 (TODO:)
    freeglut3 (freeglut3-dev)
    fmod ex 4.44.46
    libluabind-dev (libluabind-dev)
    tinyxml (libtinyxml-dev)