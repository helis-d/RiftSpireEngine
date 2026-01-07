# RiftSpire Game Executable
set(GAME_NAME RiftSpire)

# Collect game source files
file(GLOB_RECURSE GAME_SOURCES
    "*.cpp"
)

file(GLOB_RECURSE GAME_HEADERS
    "*.h"
)

# Create executable
add_executable(${GAME_NAME} ${GAME_SOURCES} ${GAME_HEADERS})

# Link engine
target_link_libraries(${GAME_NAME} PRIVATE RiftSpireEngine)

# Include directories
target_include_directories(${GAME_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set as startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_NAME})

# Organize in IDE folders
set_target_properties(${GAME_NAME} PROPERTIES FOLDER "Game")

# Copy assets to output directory (when we have assets)
# add_custom_command(TARGET ${GAME_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:${GAME_NAME}>/Assets
# )
