#pragma once
#include <utility>
#include <string>
#include <assert.h>
#include <stdint.h>

#include "Core/CoreTypes.h"


class CellIndex
{
public:
    // Input in indexes view
    CellIndex(int x, int y);

    // Input like "a2" or "c10"
    CellIndex(const std::string& coord);

    // Returns array indexes
    [[nodiscard]] inline std::pair<int, int> AsIndexesPair() const { return { X(), Y() }; }

    [[nodiscard]] inline int X() const { return InternalCoordinates.first; }

    [[nodiscard]] inline int Y() const { return InternalCoordinates.second; }

    [[nodiscard]] const std::string ToString(const bool InIsUpperCase = false) const;

    friend bool operator==(const CellIndex& c1, const CellIndex& c2);
    friend bool operator!=(const CellIndex& c1, const CellIndex& c2);

private:
    std::pair<int8_t, int8_t> InternalCoordinates;
};


inline bool operator==(const CellIndex& c1, const CellIndex& c2)
{
    return c1.InternalCoordinates == c2.InternalCoordinates;
}

inline bool operator!=(const CellIndex& c1, const CellIndex& c2)
{
    return c1.InternalCoordinates != c2.InternalCoordinates;
}



// TODO impl CellRange if need (might be used for generation)
