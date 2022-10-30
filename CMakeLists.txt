cmake_minimum_required(VERSION 3.20)
project(DuckShooter)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)

set(SOURCE_FILES
        "${SRC_DIR}/Main.cpp"
        "${SRC_DIR}/Texture.cpp"
        "${SRC_DIR}/Texture.h"
        "${SRC_DIR}/DuckSpawner.cpp"
        "${SRC_DIR}/DuckSpawner.h"

)

find_package(SDL2 CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(sdl2-image CONFIG REQUIRED)

find_package(sdl2-ttf CONFIG REQUIRED)

find_package(sdl2-mixer CONFIG REQUIRED)


add_executable(${PROJECT_NAME}
               "${SOURCE_FILES}")
        
set_target_properties(${PROJECT_NAME}
                        PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/$<CONFIG>"
)

target_include_directories(${PROJECT_NAME} PUBLIC "${SDL2_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME}
                        PUBLIC "${SDL2_LIBRARIES}"
                               "glm::glm"
                               "SDL2::SDL2_image"
                               "SDL2::SDL2_mixer"
                               "SDL2::SDL2_ttf"                       
)