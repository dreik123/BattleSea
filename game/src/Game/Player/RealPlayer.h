#pragma once
#include "IPlayer.h"
#include "Core/CoreTypes.h"
#include "Core/CellIndex.h"

#include <iostream>

class RealPlayer : public IPlayer
{
public:
    RealPlayer(Player player)
        : m_currentPlayer(player)
    {
    }
    virtual std::string getName() const override
    {
        return std::string("Player");
    }
    virtual bool isLocalPlayer() const override
    {
        return true;
    }
    virtual Player getPlayerType() const override
    {
        return m_currentPlayer;
    }
    virtual std::optional<InputRequest> getInput() override
    {
        std::string user_input;
        std::cin >> user_input;

        InputRequest request;

        if (user_input == "q" || user_input == "quit")
        {
            request.isQuitRequested = true;
        }
        else
        {
            // some validation
            request.shotCell = CellIndex(user_input);
        }

        return request;
    }

protected:
    Player m_currentPlayer;
};
