#pragma once

// RiftSpire Visual Block Scripting System
// Main include header

#include "Core/BlockTypes.h"
#include "Core/Value.h"
#include "Core/Block.h"
#include "Core/BlockRegistry.h"
#include "Core/BlockScript.h"

#include "Execution/ExecutionContext.h"
#include "Execution/ScriptVM.h"

#include "Serialization/ScriptSerializer.h"

namespace RiftSpire
{
    /// Initialize the scripting system (registers all blocks)
    void InitScripting();
    
    /// Shutdown the scripting system
    void ShutdownScripting();
}
