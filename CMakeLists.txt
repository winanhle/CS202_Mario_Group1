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

add_executable(Mario
    Mario.cpp
    src/core/Game.cpp
    src/core/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/PauseState.cpp
    src/states/GameOverState.cpp
    src/world/GameWorld.cpp
    src/entities/player/PlayerManager.cpp
    src/entities/player/Mario.cpp
    src/entities/player/input/PlayerInputHandler.cpp
    src/entities/player/states/PlayerState.cpp
    src/entities/MapManager.cpp
    src/entities/EnemyManager.cpp
    src/entities/ItemManager.cpp
    src/entities/camera/CameraManager.cpp
    src/ui/HUDManager.cpp
    src/ui/SaveManager.cpp
)

add_executable(PlayState_Test
    PlayState_Test.cpp
    src/states/PlayState.cpp
    src/world/GameWorld.cpp
    src/entities/player/PlayerManager.cpp
    src/entities/player/Mario.cpp
    src/entities/player/input/PlayerInputHandler.cpp
    src/entities/player/states/PlayerState.cpp
    src/entities/MapManager.cpp
    src/entities/EnemyManager.cpp
    src/entities/ItemManager.cpp
    src/entities/camera/CameraManager.cpp
    src/ui/HUDManager.cpp
    src/ui/SaveManager.cpp
)

# Linking 'sfml-graphics' automatically pulls in 'sfml-window' and 'sfml-system'.
foreach(TARGET Mario PlayState_Test)
    target_link_libraries(${TARGET} PRIVATE sfml-graphics sfml-audio)
endforeach()