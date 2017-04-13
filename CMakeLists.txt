cmake_minimum_required(VERSION 3.6)
project(Evolve)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -W -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -L./lib/ -lGL -lGLU -lglut -lglui")

include_directories(include)

SET(SRCS src/Main.cpp
        src/Brain/DRAWS.cpp
        src/Brain/DWRAON.cpp

        src/glview.cpp
        src/World.cpp
        src/Unit.cpp
        src/Forms.cpp
        )

add_executable(evolve "${SRCS}")
