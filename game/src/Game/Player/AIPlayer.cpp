#include "AIPlayer.h"
#include "Game/GameInterfaces.h"
#include "Core/CoreTypes.h"

#include <string>

AIPlayer::AIPlayer(const Player player, const std::shared_ptr<IBattleSeaGame>& game)
    : m_currentPlayer(player)
    , m_gameInstance(game)
{

}


bool AIPlayer::isLocalPlayer() const
{
    return false;
}

Player AIPlayer::getPlayerType() const
{
    return m_currentPlayer;
}

std::string AIPlayer::getName() const
{
    return std::string("Bot");
}

InputRequest AIPlayer::getInput()
{
    const Player opponent = getOppositePlayer(getPlayerType());

    CellState state = CellState::Concealed;
    InputRequest turn;
    
    return turn;
}