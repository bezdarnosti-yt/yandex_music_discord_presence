cmake_minimum_required(VERSION 3.10)
project(yandex-music-discord-rpc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await:strict")

set(DISCORD_SDK_ROOT "${CMAKE_SOURCE_DIR}/discord_game_sdk")
set(DISCORD_SDK_INCLUDE "${DISCORD_SDK_ROOT}/cpp")
set(DISCORD_SDK_LIB_DIR "${DISCORD_SDK_ROOT}/lib/x86_64")

file(GLOB DISCORD_SDK_SOURCES "${DISCORD_SDK_INCLUDE}/*.cpp")

file(GLOB SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB INCL "${CMAKE_SOURCE_DIR}/include/*.h")


add_executable(${PROJECT_NAME} WIN32
    ${SRC}
    src/app.rc
    ${DISCORD_SDK_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${DISCORD_SDK_INCLUDE}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    ${DISCORD_SDK_ROOT}/lib/x86_64/discord_game_sdk.dll.lib
    windowsapp.lib  # WinRT
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${DISCORD_SDK_ROOT}/lib/x86_64/discord_game_sdk.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)