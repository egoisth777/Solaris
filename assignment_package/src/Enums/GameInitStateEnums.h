#ifndef GAMEINITSTATEENUMS_H
#define GAMEINITSTATEENUMS_H

#define GameInitState MGameState::GameInitStateEnums

namespace MGameState{

    /// Defines flags for game initialization
    /// Before calling start() and the first tick(), the COMPLETE flag should be fulfilled
    enum class GameInitStateEnums : int{
        NULL_STATE          = 0,
        GAME_MGR_CONSTRUCT  = 1,
        GL_INIT             = 1 << 1,
        TERRAIN_GENERATE    = 1 << 2,

        // Add more states here, also update COMPLETE flag

        ENGINE_START    = 1 << 10,
        GAME_START      = 1 << 11,
        INIT_COMPLETE   = GAME_MGR_CONSTRUCT | GL_INIT,
        READY_FOR_PLAY  = INIT_COMPLETE | TERRAIN_GENERATE
    };
}

#endif // GAMEINITSTATEENUMS_H
