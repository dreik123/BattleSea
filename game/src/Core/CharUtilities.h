#pragma once

namespace char_utilities
{
    // Compile time analog of tolower()
    static constexpr char charToLower(const char value) {
        return (value >= 'A' && value <= 'Z') ? value + ('a' - 'A') : value;
    }

    // Compile time analog of isdigit()
    static constexpr char isDigit(const char value) {
        return value >= '0' && value <= '9';
    }

    // Compile time analog of isalpha()
    static constexpr char isAlpha(const char value) {
        return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
    }

    static constexpr int8_t digitCharToInt(const char value)
    {
        assert(isDigit(value));
        return value - '0';
    }

    // It's insensetive to case by default, so both 'A' and 'a' will be converted to 0 
    static constexpr int8_t letterCharToInt(const char value)
    {
        assert(isAlpha(value));
        return charToLower(value) - 'a';
    }

    // value is in [0-9]. 0 means 'a'
    static constexpr char indexToLetterChar(const int value, bool isUpperCase)
    {
        assert(value >= 0);
        return value + (isUpperCase ? 'A' : 'a');
    }

    // TODO constexpr for static_assert support (will be done in the next commits)
    static const std::string indexToDigitStr(const int value)
    {
        assert(value >= 0);
        return std::to_string(value + 1); // convert to user-friendly value
    }
}
