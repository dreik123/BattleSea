#pragma once
#include "Core/CoreDefines.h"

#if !EVENTS_STD_ANY_APPROACH
struct Event
{
    // I commented it out to use aggregate initialization for derived events
    //virtual ~Event() = default;
};
#endif // EVENTS_STD_ANY_APPROACH