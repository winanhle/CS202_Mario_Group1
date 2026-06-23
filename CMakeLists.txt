cmake_minimum_required(VERSION 3.28)

project(Mario)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Mario
    src/main.cpp
    src/core/Game.cpp
    src/core/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/PauseState.cpp
    src/states/GameOverState.cpp
    src/world/GameWorld.cpp
    src/entities/PlayerManager.cpp
    src/entities/EnemyManager.cpp
    src/entities/ItemManager.cpp
    src/ui/HUDManager.cpp
    src/ui/SaveManager.cpp
)

target_include_directories(Mario PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/include
)

target_link_directories(Mario PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/lib
)

target_link_libraries(Mario PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)