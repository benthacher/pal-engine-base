cmake_minimum_required(VERSION 3.29)
project(pal_example)

# MUST SET BACKEND for PAL engine to build
set(PAL_BACKEND_SOURCES ${CMAKE_SOURCE_DIR}/src/pal_backend.c)
set(PAL_BACKEND_LIBRARIES "")
set(PAL_BACKEND_INCLUDES "")

set(ASSETS_MIDI_DIR ${CMAKE_SOURCE_DIR}/assets/midi)
set(ASSETS_SPRITES_DIR ${CMAKE_SOURCE_DIR}/assets/sprites)

set(MIDI_FILES
    # ${ASSETS_MIDI_DIR}/song.mid
)

set(SPRITE_FILES
    # ${ASSETS_SPRITES_DIR}/sprite.gif
)

add_subdirectory(${CMAKE_SOURCE_DIR}/pal-engine)

add_executable(pal_example
    src/main.c
)

target_link_libraries(pal_example pal_engine)
target_include_directories(pal_example PUBLIC include)