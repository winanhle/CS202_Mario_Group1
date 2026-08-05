cmake_minimum_required(VERSION 3.28)
project(CS202_Mario LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.1.0 
)
FetchContent_MakeAvailable(SFML)


set(COMMON_SOURCES
    src/entities/player/PlayerManager.cpp
    src/entities/player/Mario.cpp
    src/entities/player/Luigi.cpp
    src/entities/player/input/PlayerInputHandler.cpp
    src/entities/player/forms/NormalForm.cpp
    src/entities/player/forms/SuperForm.cpp
    src/entities/player/forms/FireForm.cpp
)

set(SHARED_SOURCES
    ${COMMON_SOURCES}
    src/core/Game.cpp
    src/core/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/PauseState.cpp
    src/states/GameOverState.cpp
    src/world/GameWorld.cpp
    src/entities/MapManager.cpp
    src/entities/enemy/Enemy.cpp
    src/entities/enemy/EnemyManager.cpp
    src/entities/item/Item.cpp
    src/entities/item/ItemManager.cpp
    src/entities/camera/CameraManager.cpp
    src/ui/HUDManager.cpp
    src/ui/SaveManager.cpp
)

add_executable(Mario
    Mario.cpp
    ${SHARED_SOURCES}
)

add_executable(PlayState_Test
    PlayState_Test.cpp
    ${SHARED_SOURCES}
)

add_custom_command(TARGET Mario POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Mario>/assets
)

foreach(TARGET Mario PlayState_Test)
    target_link_libraries(${TARGET} PRIVATE sfml-graphics sfml-audio)
endforeach()
