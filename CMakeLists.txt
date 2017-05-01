cmake_minimum_required(VERSION 3.5)
project(Evolve)
#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -W -Wall -Wl,--no-as-needed")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L./lib/ -lGLU -lGL -fopenmp -lglut -lglui")

include_directories(include)

SET(SRCS src/Main.cpp
        src/Brain/DRAWS.cpp
        src/Brain/DWRAON.cpp

        src/glview.cpp
        src/World.cpp
        src/Unit.cpp
        src/Forms.cpp

        include/ANeuron.hpp
        include/Color.hpp
        include/DRAWS.hpp
        include/DWRAON.hpp
        include/Forms.hpp
        include/glui.h
        include/glview.hpp
        include/IBrain.hpp
        include/Input.hpp
        include/IView.hpp
        include/Settings.hpp
        include/Unit.hpp
        include/Utils.hpp
        include/Vector.hpp
        include/World.hpp
        )

add_executable(evolve "${SRCS}")
