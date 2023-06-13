#include "CellIndex.h"
#include "CharUtilities.h"

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

    m_internalCoordinates.first = char_utilities::letterCharToInt(coord[0]);
    assert(m_internalCoordinates.first >= 0 && m_internalCoordinates.first < CLASSIC_GRID_ROW_COUNT);
    if (coord.size() == 2)
    {
        m_internalCoordinates.second = char_utilities::digitCharToInt(coord[1]) - 1; // -1 to handle array index and view difference
    }
    else if (coord[1] == '1' && coord[2] == '0')
    {
        m_internalCoordinates.second = CLASSIC_GRID_ROW_COUNT - 1; // -1 to handle array index and view difference
    }
    else
    {
        assert(false && "Incorrect passing argument to initiate CellIndex object.");
    }
    assert(m_internalCoordinates.second >= 0 && m_internalCoordinates.second < CLASSIC_GRID_COLUMN_COUNT);
}

const std::string CellIndex::toString(const bool isUpperCase) const
{
    std::string result;
    result.push_back(char_utilities::indexToLetterChar(m_internalCoordinates.first, isUpperCase));
    result += char_utilities::indexToDigitStr(m_internalCoordinates.second);

    return result;
}


// TODO consider separate place and approach if need for these tests
constexpr void cellIndexTests()
{
    using namespace char_utilities;

    static_assert(letterCharToInt('a') == 0);
    static_assert(letterCharToInt('b') == 1);
    static_assert(charToLower('C') == 'c');
    static_assert(letterCharToInt(charToLower('C')) == 2);
    static_assert(letterCharToInt('C') == 2);
    static_assert(letterCharToInt('c') == 2);

    static_assert(indexToLetterChar(4, false) == 'e');
    static_assert(indexToLetterChar(5, true) == 'F');
    //static_assert(IndexToDigitStr(1) == "2");
    //static_assert(IndexToDigitStr(9) == "10");
}
