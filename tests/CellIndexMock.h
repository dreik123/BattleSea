#pragma once
#include <gmock/gmock.h>
#include "Core/CellIndex.h"

class CellIndexMock : public CellIndex
{
public:
    MOCK_METHOD((std::pair<int, int>), asIndexesPair, (), (const));
    MOCK_METHOD(int, x, (), (const));
    MOCK_METHOD(int, y, (), (const));
    MOCK_METHOD(const std::string, toString, (const bool isUpperCase), (const));
};
