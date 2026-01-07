# RiftBlocks Visual Scripting Module
set(MODULE_NAME RiftScripting)

set(SCRIPTING_SOURCES
    # Core
    Core/Block.cpp
    Core/BlockRegistry.cpp
    Core/BlockScript.cpp
    Core/Value.cpp
    Scripting.cpp
    
    # Execution
    Execution/ScriptVM.cpp
    Execution/ExecutionContext.cpp
    
    # Serialization
    Serialization/ScriptSerializer.cpp
    
    # Blocks - Categories
    Blocks/OperatorBlocks.cpp
    Blocks/ControlFlowBlocks.cpp
    Blocks/EventBlocks.cpp
    Blocks/DataBlocks.cpp
    Blocks/MovementBlocks.cpp
    Blocks/HealthBlocks.cpp
    Blocks/AbilityBlocks.cpp
    Blocks/DebugBlocks.cpp
    Blocks/TimeBlocks.cpp
)

set(SCRIPTING_HEADERS
    Scripting.h
    
    # Core
    Core/Block.h
    Core/BlockRegistry.h
    Core/BlockScript.h
    Core/BlockTypes.h
    Core/Value.h
    
    # Execution
    Execution/ScriptVM.h
    Execution/ExecutionContext.h
    
    # Serialization
    Serialization/ScriptSerializer.h
    
    # Blocks
    Blocks/AllBlocks.h
    Blocks/OperatorBlocks.h
    Blocks/ControlFlowBlocks.h
    Blocks/EventBlocks.h
    Blocks/DataBlocks.h
    Blocks/MovementBlocks.h
    Blocks/HealthBlocks.h
    Blocks/AbilityBlocks.h
    Blocks/DebugBlocks.h
    Blocks/TimeBlocks.h
)

add_library(${MODULE_NAME} STATIC ${SCRIPTING_SOURCES} ${SCRIPTING_HEADERS})

target_include_directories(${MODULE_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # For Core/UUID.h
)
target_link_libraries(${MODULE_NAME} PUBLIC glm::glm)
