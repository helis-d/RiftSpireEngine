# RiftSpire Engine Library
set(ENGINE_NAME RiftSpireEngine)

# Add Scripting module
add_subdirectory(Scripting)

# Collect all source files
file(GLOB_RECURSE ENGINE_SOURCES
    "Core/*.cpp"
    "Renderer/*.cpp"
    "ECS/*.cpp"
    "Physics/*.cpp"
    "Audio/*.cpp"
    "Network/*.cpp"
    "Platform/*.cpp"
)

# GLAD source (explicit)
set(GLAD_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad/src/gl.c")
list(APPEND ENGINE_SOURCES ${GLAD_SOURCE})

file(GLOB_RECURSE ENGINE_HEADERS
    "Core/*.h"
    "Renderer/*.h"
    "ECS/*.h"
    "Physics/*.h"
    "Audio/*.h"
    "Network/*.h"
    "Platform/*.h"
    "Vendor/glad/include/*.h"
)

# Create engine library
add_library(${ENGINE_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Include directories
target_include_directories(${ENGINE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor
)

# Link libraries
target_link_libraries(${ENGINE_NAME} PUBLIC
    glfw
    glm::glm
    spdlog::spdlog
    EnTT::EnTT
    imgui::imgui
    assimp::assimp
    OpenGL::GL
    RiftScripting
)

# Precompiled header (speeds up compilation) - C++ only
target_precompile_headers(${ENGINE_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:<memory$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<string$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<vector$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<unordered_map$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<functional$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<algorithm$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<fstream$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<sstream$<ANGLE-R>>"
)

# Disable PCH for C files
set_source_files_properties(${GLAD_SOURCE} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

# Windows-specific definitions
if(WIN32)
    target_compile_definitions(${ENGINE_NAME} PUBLIC
        RS_PLATFORM_WINDOWS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# Debug/Release definitions
target_compile_definitions(${ENGINE_NAME} PUBLIC
    $<$<CONFIG:Debug>:RS_DEBUG>
    $<$<CONFIG:Release>:RS_RELEASE>
)

# Organize in IDE folders
set_target_properties(${ENGINE_NAME} PROPERTIES FOLDER "Engine")
