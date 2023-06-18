#include "pch.h"
#include "Game/WarShip.h"
#include "CellIndexMock.h"
#include <gmock/gmock.h>
#include <vector>
#include <memory>

class WarShipTest : public testing::Test
{
protected:
    void SetUp() override
    {
        auto temp = std::make_unique<testing::StrictMock<WarShip>>(m_mock);
    }
protected:
    std::unique_ptr<WarShip> m_warShip;
    testing::StrictMock<CellIndexMock>* m_mock;
};


