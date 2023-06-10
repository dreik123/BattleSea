#pragma once

#include <bitset>
#include <type_traits>

template <typename T>
concept EnumType = std::is_enum_v<T>();

// Type T enums Must start from 0 and contains Count member
template<typename EnumType>
class BitMask
{
public:
    void setFlag(EnumType flag)
    {
        const int bit = static_cast<int>(flag);
        m_bits.set(bit, true);
    }
    void clearFlag(EnumType flag)
    {
        const int bit = static_cast<int>(flag);
        m_bits.set(bit, false);
    }
    void toggleFlag(EnumType flag)
    {
        const int bit = static_cast<int>(flag);
        m_bits.flip(bit);
    }
    bool checkFlag(EnumType flag) const
    {
        const int bit = static_cast<int>(flag);
        return checkBit(bit);
    }
    bool checkBit(int bitIndex) const
    {
        return m_bits.test(bitIndex);
    }
    int countBits() const { return (int)m_bits.count(); }
    bool isEmpty() const { return countBits() == 0; }

    void resetFlags()
    {
        m_bits.reset();
    }

    void setAllFlags()
    {
        for (size_t i = 0; i < m_bits.size(); i++)
        {
            setFlag(static_cast<EnumType>(i));
        }
    }

    const std::vector<EnumType> getSetFlagsAsValues() const
    {
        std::vector<EnumType> values;
        for (int i = 0; i < m_bits.size(); i++)
        {
            if (checkBit(i))
            {
                values.push_back(static_cast<EnumType>(i));
            }
        }
        return values;
    }

private:
    static constexpr int s_size = static_cast<int>(EnumType::Count);
    std::bitset<s_size> m_bits;
};

