#pragma once
#include "IPlayer.h"
#include "Core/CoreTypes.h"
#include "Core/CellIndex.h"
#include "Core/CharUtilities.h"

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
    virtual InputRequest getInput() override
    {
        std::string user_input;
        std::cin >> user_input;

        InputRequest request;

        if (user_input == "q" || user_input == "quit")
        {
            request.isQuitRequested = true;
            return request;
        }

        if (validateUserInput(user_input))
        {
            request.shotCell = CellIndex(user_input);
        }

        return request;
    }

private:
    bool validateUserInput(const std::string& input) const
    {
        // Expactations from input: aX or aXX
        const bool hasLetterAndOneDigit = 
            input.size() == 2 &&
            char_utilities::isAlpha(input[0]) &&
            char_utilities::isDigit(input[1]);
        const bool hasLetterAndTwoDigits = 
            input.size() == 3 &&
            char_utilities::isAlpha(input[0]) &&
            char_utilities::isDigit(input[1]) &&
            char_utilities::isDigit(input[2]);

        // Bound limitations are checked in game logic
        return hasLetterAndOneDigit || hasLetterAndTwoDigits;
    }

protected:
    Player m_currentPlayer;
};
