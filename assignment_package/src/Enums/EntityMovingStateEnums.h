#ifndef ENTITYMOVINGSTATEENUMS_H
#define ENTITYMOVINGSTATEENUMS_H

#include <iostream>
#define MovingState MEntity::EntityMovingState

namespace MEntity{

    enum class EntityMovingState : int
    {
        WALKING = 1 << 0,
        SPEEDUP = 1 << 1,
        SWIM    = 1 << 2,

        /// Remove collision & gravity
        /// Remove swim effect
        FLY     = 1 << 3,

        /// Lock x-axis rotation,
        /// Low down moving/rotating speed
        PHOTO   = 1 << 4
    };

    /// If one mask is enabled, disable it, vise versa.
    inline void switchMask(EntityMovingState& state, EntityMovingState target);
    inline bool isUnderState(EntityMovingState inState, EntityMovingState target);
    inline bool isSpeedUp(EntityMovingState state);

    inline EntityMovingState operator|(EntityMovingState a, EntityMovingState b) {
        return static_cast<EntityMovingState>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline EntityMovingState operator&(EntityMovingState a, EntityMovingState b) {
        return static_cast<EntityMovingState>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline EntityMovingState operator~(EntityMovingState a) {
        return static_cast<EntityMovingState>(~static_cast<int>(a));
    }

    inline void switchMask(EntityMovingState& state, EntityMovingState target){
        if(isUnderState(state, target)){
            state = state & ~target;
        }
        else{
            state = state | target;
        }
    }

    inline bool isUnderState(EntityMovingState inState, EntityMovingState target){
        return (inState & target) == target;
    }

    inline bool isSpeedUp(EntityMovingState state){
        return isUnderState(state, EntityMovingState::SPEEDUP);
    }
}

#endif // ENTITYMOVINGSTATEENUMS_H
