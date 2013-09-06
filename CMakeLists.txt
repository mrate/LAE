PROJECT(LAE_Core)
 
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../libs)
SET(BOOST_VERSION 1_54_0)
SET(PROJECT_BINARY_DIR ${CMAKE_BUILD_TYPE})

#INCLUDE_DIRECTORIES(/home/mrate/libs/zlib/include)
INCLUDE_DIRECTORIES(../../libs/boost_${BOOST_VERSION})
INCLUDE_DIRECTORIES(/home/mrate/libs/fmodapi44418linux/api/inc)
INCLUDE_DIRECTORIES(../../libs/tinyxml)
INCLUDE_DIRECTORIES(../../libs/luabind-0.9.1)
INCLUDE_DIRECTORIES(/usr/include/lua5.1)

SET(MAIN_SRC
    cache/TextStorage.cpp
    cache/ImageCache.cpp
    cache/AnimationCache.cpp
    cache/SoundCache.cpp
    game/GameEngine.cpp
    game/GameState.cpp
    game/GameTimer.cpp
    graphics/AnimatedSprite.cpp
    graphics/Animation.cpp
    graphics/AnimationPlayer.cpp
    graphics/Font.cpp
    graphics/Image.cpp
    graphics/Screen.cpp
    graphics/TextPanel.cpp
    audio/MediaPlayer.cpp
    audio/Sound.cpp
    logger/Logger.cpp
    lua/LuaRegistrator.cpp
    objects/Actor.cpp
    objects/Camera.cpp
    objects/InventoryItem.cpp
    objects/Item.cpp
    logger/OStreamContext.cpp
    script/ScriptInterpret.cpp
    screens/GameManager.cpp
    screens/GameRoom.cpp
    screens/RoomPlayer.cpp
    screens/ScriptPlayer.cpp
    ../../libs/tinyxml/tinystr.cpp
    ../../libs/tinyxml/tinyxml.cpp
    ../../libs/tinyxml/tinyxmlerror.cpp
    ../../libs/tinyxml/tinyxmlparser.cpp
    math/MathUtils.cpp
    math/point.cpp
    math/vector.cpp
    game/Global.cpp
    utils/utils.cpp
    xml/Xml.cpp
)

SET(CMAKE_DEBUG_POSTFIX _d)
ADD_LIBRARY(LAE_Core ${MAIN_SRC})

if(DEFINED UNIX)
  MESSAGE(">>> LINUX CONFIG <<<")

  ADD_DEFINITIONS(-Dnullptr=0)

  SET(CMAKE_CXX_FLAGS "-m32 -Wall -std=c++0x -pipe")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -g3 -D_DEBUG")
  
  SET(CMAKE_C_FLAGS "-m32 -Wall")
  SET(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  SET(CMAKE_C_FLAGS_DEBUG "-ggdb3 -g3 -D_DEBUG")

  ADD_DEFINITIONS(-Doverride=)
  ADD_DEFINITIONS(-DTIXML_USE_STL)
endif(DEFINED UNIX)

