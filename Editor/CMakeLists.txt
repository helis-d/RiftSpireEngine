# RiftSpire Editor

set(EDITOR_NAME RiftSpireEditor)

# Collect source files
file(GLOB_RECURSE EDITOR_SOURCES
    "*.cpp"
)

file(GLOB_RECURSE EDITOR_HEADERS
    "*.h"
)

# Create editor executable
add_executable(${EDITOR_NAME} ${EDITOR_SOURCES} ${EDITOR_HEADERS})

# Include directories
target_include_directories(${EDITOR_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Engine
)

# Find NanoVG
find_package(nanovg CONFIG QUIET)

# Link engine library
target_link_libraries(${EDITOR_NAME} PRIVATE
    RiftSpireEngine
    RiftScripting
    imgui::imgui
    OpenGL::GL
    $<$<TARGET_EXISTS:nanovg::nanovg>:nanovg::nanovg>
)

# Windows-specific
if(WIN32)
    target_compile_definitions(${EDITOR_NAME} PRIVATE
        RS_PLATFORM_WINDOWS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# Organize in IDE folders
set_target_properties(${EDITOR_NAME} PROPERTIES FOLDER "Editor")
