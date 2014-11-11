# README #

## Building LuaAdventureEngine ##

To build LuaAdventureEngine you need to use CMake. It's best to create 2 directories for RELEASE and DEBUG builds:

```
#!bash

mkdir -p build/{release,debug}
cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../../ && cd ../../
cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../../ && cd ../../
```

This will create Makefile in each of the directory. To build a specific type just go to the directory and run make:

```
#!bash

cd build/release
make
```

### Prerequisites ###

Projects has following dependencies (Ubuntu package name is in parenthesis at each library):

* CMake (cmake)
* gcc (gcc-4.9-multilib g++-4.9-multilib)
* boost 1.54 (libboost-all-dev)
* libpng (libpng12-dev)
* zlib (zlib1g-dev)
* liblua5.2 (liblua5.2-dev)
* freeglut3 (freeglut3-dev)
* fmod ex 4.44.46
* libluabind-dev (libluabind-dev)
* tinyxml (libtinyxml-dev)