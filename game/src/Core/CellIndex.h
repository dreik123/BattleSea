#pragma once
#include <utility>
#include <string>
#include <assert.h>
#include <stdint.h>

#include "CoreTypes.h"


class CellIndex
{
public:
    // Input in indexes view
    CellIndex(int x, int y);

    // Input like "a2" or "c10"
    CellIndex(const std::string& coord);

    // Returns array indexes
    [[nodiscard]] inline std::pair<int, int> asIndexesPair() const { return { x(), y() }; }

    [[nodiscard]] inline int x() const { return m_internalCoordinates.first; }

    [[nodiscard]] inline int y() const { return m_internalCoordinates.second; }

    [[nodiscard]] const std::string toString(const bool InIsUpperCase = false) const;

    friend bool operator==(const CellIndex& c1, const CellIndex& c2);
    friend bool operator!=(const CellIndex& c1, const CellIndex& c2);

private:
    std::pair<int8_t, int8_t> m_internalCoordinates;
};


inline bool operator==(const CellIndex& c1, const CellIndex& c2)
{
    return c1.m_internalCoordinates == c2.m_internalCoordinates;
}

inline bool operator!=(const CellIndex& c1, const CellIndex& c2)
{
    return c1.m_internalCoordinates != c2.m_internalCoordinates;
}



// TODO impl CellRange if need (might be used for generation)
