cmake_minimum_required(VERSION 3.28)
project(CS202_Mario LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
option(MARIO_USE_SYSTEM_SFML "Use an installed SFML package instead of FetchContent" OFF)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(MARIO_USE_SYSTEM_SFML)
    find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Audio)
    set(MARIO_SFML_TARGETS SFML::Graphics SFML::Audio)
else()
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.1.0
    )
    FetchContent_MakeAvailable(SFML)
    set(MARIO_SFML_TARGETS sfml-graphics sfml-audio)
endif()


set(COMMON_SOURCES
    src/entities/player/PlayerManager.cpp
    src/entities/player/Mario.cpp
    src/entities/player/Luigi.cpp
    src/entities/player/input/PlayerInputHandler.cpp
    src/entities/player/forms/NormalForm.cpp
    src/entities/player/forms/SuperForm.cpp
    src/entities/player/forms/FireForm.cpp
    src/entities/projectile/Fireball.cpp
    src/entities/projectile/FireballManager.cpp
)

set(SHARED_SOURCES
    ${COMMON_SOURCES}
    src/core/Game.cpp
    src/core/LevelManager.cpp
    src/core/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/LevelSelectState.cpp
    src/states/MapEditorState.cpp
    src/states/PauseState.cpp
    src/states/GameOverState.cpp
    src/states/WinState.cpp
    src/states/IntermissionState.cpp
    src/states/CharacterSelectState.cpp
    src/states/ModeSelectState.cpp
    src/states/InitialsEntryState.cpp
    src/states/LeaderboardState.cpp
    src/world/GameWorld.cpp
    src/entities/map/MapManager.cpp
    src/entities/map/block/BlockBehavior.cpp
    src/entities/enemy/Enemy.cpp
    src/entities/enemy/EnemyManager.cpp
    src/entities/enemy/EnemyFactory.cpp
    src/entities/item/Item.cpp
    src/entities/item/ItemManager.cpp
    src/entities/lift/Lift.cpp
    src/entities/lift/LiftManager.cpp
    src/entities/firebar/FireBar.cpp
    src/entities/firebar/FireBarManager.cpp
    src/entities/camera/CameraManager.cpp
    src/ui/HUDManager.cpp
    src/ui/SaveManager.cpp
    src/ui/SettingsManager.cpp
    src/ui/SettingsMenu.cpp
    src/ui/SoundManager.cpp
    src/ui/MenuBackground.cpp
    src/ui/UIUtils.cpp
    src/tinyxml2.cpp
)

if(WIN32)
    set(RESOURCE_SOURCES src/resource.rc)
endif()

add_executable(MarioGame
    src/main.cpp
    ${SHARED_SOURCES}
    ${RESOURCE_SOURCES}
)

foreach(TARGET MarioGame)
    target_link_libraries(${TARGET} PRIVATE ${MARIO_SFML_TARGETS})
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:${TARGET}>/assets
    )
endforeach()
add_compile_options(-Wall -Wextra -Wunused)
