cmake_minimum_required(VERSION 3.14)
project(game_of_life)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "/usr/local/opt/qt/lib/cmake") # Set to your Qt cmake path

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)


add_executable(game_of_life main.cpp src/Chunk.cpp src/Chunk.h src/Side.cpp src/Side.h src/Topology.cpp src/Topology.h
        src/util.h src/Neighbourhood.cpp src/Neighbourhood.h src/Ruleset.cpp src/Ruleset.h src/Automaton.cpp
        src/Automaton.h src/mainwindow.h src/mainwindow.cpp src/ChunkGraphicsItem.cpp src/ChunkGraphicsItem.h src/AutomatonScene.cpp src/AutomatonScene.h)

target_link_libraries(game_of_life Qt5::Core)
target_link_libraries(game_of_life Qt5::Widgets)
target_link_libraries(game_of_life Qt5::Quick)
