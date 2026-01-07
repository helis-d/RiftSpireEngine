#include "Scripting.h"
#include "Blocks/OperatorBlocks.h"
#include "Blocks/ControlFlowBlocks.h"
#include "Blocks/EventBlocks.h"
#include "Blocks/DataBlocks.h"
#include "Blocks/TimeBlocks.h"
#include "Blocks/DebugBlocks.h"
#include "Blocks/MovementBlocks.h"
#include "Blocks/HealthBlocks.h"
#include "Blocks/AbilityBlocks.h"
// #include <Core/Logger.h>  // TODO: Integrate logger

namespace RiftSpire
{
    void InitScripting()
    {
        // RS_INFO("Initializing RiftBlocks Visual Scripting System...");
        
        // Register all block categories
        RegisterOperatorBlocks();
        RegisterControlFlowBlocks();
        RegisterEventBlocks();
        RegisterDataBlocks();
        RegisterTimeBlocks();
        RegisterDebugBlocks();
        RegisterMovementBlocks();
        RegisterHealthBlocks();
        RegisterAbilityBlocks();
        RegisterTargetingBlocks();
        RegisterAttackBlocks();
        RegisterDamageBlocks();
        RegisterDefenseBlocks();
        RegisterBuffDebuffBlocks();
        RegisterStatusEffectBlocks();
        RegisterNetworkBlocks();
        RegisterSyncBlocks();
        
        // RS_INFO("Registered {} block types", BlockRegistry::Get().GetBlockCount());
    }
    
    void ShutdownScripting()
    {
        // RS_INFO("Shutting down RiftBlocks Visual Scripting System");
    }
}
