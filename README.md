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

* CMake 2.8 (cmake)
* gcc 4.9 (gcc-4.9-multilib g++-4.9-multilib)
* boost 1.54 (libboost-all-dev)
* libpng 1.2 (libpng12-dev)
* zlib 1.2 (zlib1g-dev)
* liblua5.2 (liblua5.2-dev)
* freeglut3 2.8.1 (freeglut3-dev)
* libluabind-dev 0.9.1 (libluabind-dev)
* tinyxml 2.6.2 (libtinyxml-dev)
* fmod ex 4.44.46 (http://www.fmod.org/download/fmodex/api/Linux/fmodapi44446linux.tar.gz)
