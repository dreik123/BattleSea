#pragma once
#include <utility>
#include <string>
#include <assert.h>
#include <stdint.h>

#include "CoreTypes.h"

// TODO create cpp and move to core folder

constexpr char CharToLower(const char InValue);
constexpr int8_t DigitCharToInt(const char InValue);
constexpr int8_t LetterCharToInt(const char InValue, bool InIsCaseInsensetive);
constexpr char IndexToLetterChar(const int InValue, bool InIsUpperCase);

// TODO constexpr
static const std::string IndexToDigitStr(const int InValue)
{
    assert(InValue >= 0);
    return std::to_string(InValue + 1); // convert to user-friendly value
}

class CellIndex
{
public:
    // Input like 'a', '2', but a '10' is imposible to keep as is
    CellIndex(int x, int y)
        : InternalCoordinates{ static_cast<int8_t>(x), static_cast<int8_t>(y) }
    {
        // TODO range validation
    }

    // Input like "a2"
    CellIndex(const std::string& coord)
    {
        // TODO extend code to have not only 10 as possible 2digit value in the future
        assert(
            (coord.size() == 2 && isalpha(coord[0]) && isdigit(coord[1])) ||
            (coord.size() == 3 && isalpha(coord[0]) && coord[1] == '1' && coord[2] == '0')
        );

        InternalCoordinates.first = LetterCharToInt(coord[0], true);
        assert(InternalCoordinates.first >= 0 && InternalCoordinates.first < GridRowCount);
        if (coord.size() == 2)
        {
            InternalCoordinates.second = DigitCharToInt(coord[1]) - 1; // -1 to handle array index and view difference
        }
        else if (coord[1] == '1' && coord[2] == '0')
        {
            InternalCoordinates.second = GridRowCount - 1; // -1 to handle array index and view difference
        }
        else
        {
            assert(false && "Incorrect passing argument to initiate CellIndex object.");
        }
        assert(InternalCoordinates.second >= 0 && InternalCoordinates.second < GridColumnCount);
    }

    // Returns array indexes
    std::pair<int, int> AsIndexesPair() const
    {
        return { X(), Y() };
    }

    [[nodiscard]] inline int X() const
    {
        return InternalCoordinates.first;
    }

    [[nodiscard]] inline int Y() const
    {
        return InternalCoordinates.second;
    }

    [[nodiscard]] const std::string ToString(const bool InIsUpperCase = false) const
    {
        std::string result;
        result.push_back(IndexToLetterChar(InternalCoordinates.first, InIsUpperCase));
        result += IndexToDigitStr(InternalCoordinates.second);

        return result;
    }

    friend bool operator==(const CellIndex& c1, const CellIndex& c2);

private:
    std::pair<int8_t, int8_t> InternalCoordinates;
};

inline bool operator==(const CellIndex& c1, const CellIndex& c2)
{
    return c1.InternalCoordinates == c2.InternalCoordinates;
}

// TODO impl CellRange if need


// Compile time analog of tolower()
static constexpr char CharToLower(const char InValue) {
    return (InValue >= 'A' && InValue <= 'Z') ? InValue + ('a' - 'A') : InValue;
}

// Compile time analog of isdigit()
static constexpr char IsDigit(const char InValue) {
    return InValue >= '0' && InValue <= '9';
}

// Compile time analog of isalpha()
static constexpr char IsAlpha(const char InValue) {
    return (InValue >= 'A' && InValue <= 'Z') || (InValue >= 'a' && InValue <= 'z');
}

constexpr int8_t DigitCharToInt(const char InValue)
{
    assert(IsDigit(InValue));
    return InValue - '0';
}

constexpr int8_t LetterCharToInt(const char InValue, bool InIsCaseInsensetive = true)
{
    assert(IsAlpha(InValue));
    const char value = InIsCaseInsensetive ? CharToLower(InValue) : InValue;

    return value - 'a';
}

// InValue is in [0-9]. 0 means 'a'
constexpr char IndexToLetterChar(const int InValue, bool InIsUpperCase)
{
    assert(InValue >= 0);
    return InValue + (InIsUpperCase ? 'A' : 'a');
}

// Only for 0-9 values


// TODO consider separate place and approach if need for these tests
constexpr void CellIndexTests()
{
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