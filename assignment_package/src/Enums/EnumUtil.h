#ifndef ENUMUTIL_H
#define ENUMUTIL_H
#include <type_traits>
/// State Helper functions for enums as flag

template<typename Enum>
static inline Enum operator|(Enum a, Enum b) {
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(a) | static_cast<std::underlying_type_t<Enum>>(b));
}

template<typename Enum>
static inline Enum operator&(Enum a, Enum b) {
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(a) & static_cast<std::underlying_type_t<Enum>>(b));
}

template<typename Enum>
static inline Enum operator~(Enum a) {
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(a));
}

template<typename Enum>
void switchMask(Enum& state, Enum target) {
    if (isUnderState(state, target)) {
        state = state & ~target;
    } else {
        state = state | target;
    }
}

template<typename Enum>
bool isUnderState(Enum inState, Enum target) {
    return (inState & target) == target;
}
#endif // ENUMUTIL_H
