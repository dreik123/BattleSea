#include "CellIndex.h"

namespace char_utilities
{
    // Compile time analog of tolower()
    static constexpr char CharToLower(const char value) {
        return (value >= 'A' && value <= 'Z') ? value + ('a' - 'A') : value;
    }

    // Compile time analog of isdigit()
    static constexpr char IsDigit(const char value) {
        return value >= '0' && value <= '9';
    }

    // Compile time analog of isalpha()
    static constexpr char IsAlpha(const char value) {
        return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
    }

    static constexpr int8_t DigitCharToInt(const char value)
    {
        assert(IsDigit(value));
        return value - '0';
    }

    static constexpr int8_t LetterCharToInt(const char value, bool isCaseInsensetive = true)
    {
        assert(IsAlpha(value));
        const char val = isCaseInsensetive ? CharToLower(value) : value;

        return val - 'a';
    }

    // InValue is in [0-9]. 0 means 'a'
    static constexpr char IndexToLetterChar(const int value, bool isUpperCase)
    {
        assert(value >= 0);
        return value + (isUpperCase ? 'A' : 'a');
    }

    // TODO constexpr for static_assert support (will be done in the next commits)
    static const std::string IndexToDigitStr(const int value)
    {
        assert(value >= 0);
        return std::to_string(value + 1); // convert to user-friendly value
    }
}


CellIndex::CellIndex(int x, int y)
    : m_internalCoordinates{ static_cast<int8_t>(x), static_cast<int8_t>(y) }
{
    // TODO range validation (based on Model data)
}

CellIndex::CellIndex(const std::string& coord)
{
    // TODO extend code to have not only 10 as possible 2digit value in the future
    assert(
        (coord.size() == 2 && isalpha(coord[0]) && isdigit(coord[1])) ||
        (coord.size() == 3 && isalpha(coord[0]) && coord[1] == '1' && coord[2] == '0')
    );

    m_internalCoordinates.first = char_utilities::LetterCharToInt(coord[0], true);
    assert(m_internalCoordinates.first >= 0 && m_internalCoordinates.first < GridRowCount);
    if (coord.size() == 2)
    {
        m_internalCoordinates.second = char_utilities::DigitCharToInt(coord[1]) - 1; // -1 to handle array index and view difference
    }
    else if (coord[1] == '1' && coord[2] == '0')
    {
        m_internalCoordinates.second = GridRowCount - 1; // -1 to handle array index and view difference
    }
    else
    {
        assert(false && "Incorrect passing argument to initiate CellIndex object.");
    }
    assert(m_internalCoordinates.second >= 0 && m_internalCoordinates.second < GridColumnCount);
}

const std::string CellIndex::toString(const bool isUpperCase) const
{
    std::string result;
    result.push_back(char_utilities::IndexToLetterChar(m_internalCoordinates.first, isUpperCase));
    result += char_utilities::IndexToDigitStr(m_internalCoordinates.second);

    return result;
}


// TODO consider separate place and approach if need for these tests
constexpr void CellIndexTests()
{
    using namespace char_utilities;

    static_assert(LetterCharToInt('a') == 0);
    static_assert(LetterCharToInt('b') == 1);
    static_assert(CharToLower('C') == 'c');
    static_assert(LetterCharToInt(CharToLower('C')) == 2);
    static_assert(LetterCharToInt('C') == 2);
    static_assert(LetterCharToInt('C', false) != 2);

    static_assert(IndexToLetterChar(4, false) == 'e');
    static_assert(IndexToLetterChar(5, true) == 'F');
    //static_assert(IndexToDigitStr(1) == "2");
    //static_assert(IndexToDigitStr(9) == "10");
}
