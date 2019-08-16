cmake_minimum_required(VERSION 3.14)
project(game_of_life)

set(CMAKE_CXX_STANDARD 14)

add_executable(game_of_life main.cpp src/Chunk.cpp src/Chunk.h src/Side.cpp src/Side.h src/Topology.cpp src/Topology.h
        src/util.h src/Neighbourhood.cpp src/Neighbourhood.h)
