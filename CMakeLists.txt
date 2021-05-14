cmake_minimum_required(VERSION 3.0.0)

project(synthese_project C)
set(CMAKE_C_STANDARD 99)

# In case you have trouble to set env var un comment the following lines and put the path to the lib's location
# set(ENV{G3X_PATH} lib/location/)
# set(ENV{PROCBIT} 32b or 64b)
# set(ENV{VERSION} 6c)

if (DEFINED ENV{G3X_PATH})
    message(STATUS "The g3x library is located at $ENV{G3X_PATH}")
else()
    message(FATAL_ERROR "The g3x library cannot be located. Make sure the environment variable 'G3X_PATH' contains the path to the folder containing the g3x library.")
endif()

set(G3X_INCLUDE_PATH "$ENV{G3X_PATH}/include")
set(INCLUDES "-I./include -I$ENV{G3X_PATH}/include -I/usr/include")
set(LIBRARIES "-lrt -lm -L/usr/lib -lGL -lGLU -lglut -L$ENV{G3X_PATH} -lg3x.$ENV{VERSION}.$ENV{PROCBIT}")


add_executable(
        synthesis
        include/Utils.h
        include/Camera.h
        include/CameraConfig.h
        include/Object.h
        include/Rayon.h
        src/Utils.c
        src/Camera.c
        src/CameraConfig.c
        src/Object.c
        src/Rayon.c
        src/Main.c
)

target_include_directories(synthesis PRIVATE "${G3X_INCLUDE_PATH}")

target_compile_options(synthesis PUBLIC -ansi -Wall ${INCLUDES})
target_link_libraries(synthesis PUBLIC ${LIBRARIES})
