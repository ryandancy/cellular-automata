cmake_minimum_required(VERSION 3.14)
project(game_of_life)

set(CMAKE_CXX_STANDARD 14)

add_executable(game_of_life main.cpp gol/Chunk.cpp gol/Chunk.h gol/Side.cpp gol/Side.h gol/Topology.cpp gol/Topology.h
        gol/util.h gol/Neighbourhood.cpp gol/Neighbourhood.h)
