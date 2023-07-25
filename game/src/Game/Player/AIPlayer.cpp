#include "AIPlayer.h"

#include <string>
#include <thread>
#include <chrono>

#include "Game/Events/Events.h"

AIPlayer::AIPlayer(const Player player, const GameConfig& config, std::shared_ptr<EventBus>& bus)
    : m_currentPlayer(player)
    , m_opponentGrid{}
    , m_eventBus(bus)
    , m_lastHits{}
    , m_state(AIPlayerState::RandomShooting)
    , m_opponentShipDecks(config.numberOfMultiDeckShips)
    , rd {}
    , mt(rd())
    , m_middleGameCoeff((m_opponentGrid.data.size()* m_opponentGrid.data.at(0).size()) / 2)
{
    std::sort(m_opponentShipDecks.begin(), m_opponentShipDecks.end(), std::greater<uint8_t>());
    m_shotMissedEventHandleId = m_eventBus->subscribe<events::ShotMissedEvent>([this](const std::any& anyEvent)
        {
            const auto event = any_cast<events::ShotMissedEvent>(anyEvent);
            if (event.shootingPlayer != getPlayerType())
            {
                return;
            }

            m_opponentGrid.at(event.shot) = CellState::Missed;

            // switch to random shooting
        });
    m_shipDamagedEventHandleId = m_eventBus->subscribe<events::ShipDamagedEvent>([this](const std::any& anyEvent)
        {
            const auto event = any_cast<events::ShipDamagedEvent>(anyEvent);
            if (event.injuredPlayer != getOppositePlayer(getPlayerType()))
            {
                return;
            }
            m_opponentGrid.at(event.shot) = CellState::Damaged;
            m_lastHits.push_back(event.shot);
            // switch to state after hit state
            m_state = AIPlayerState::ShootingAfterHit;
        });
        m_shipDestroyedEventHandleId = m_eventBus->subscribe<events::ShipDestroyedEvent>([this](const std::any& anyEvent)
            {
                const auto event = any_cast<events::ShipDestroyedEvent>(anyEvent);
                if (event.injuredPlayer != getOppositePlayer(getPlayerType()))
                {
                    return;
                }
                m_lastHits.push_back(event.lastHit);

                // Mark the whole ship in destroyed
                for (const CellIndex& shipCell : event.ship.getOccupiedCells())
                {
                    m_opponentGrid.at(shipCell) = CellState::Destroyed;
                }

                for (const CellIndex& cellAround : event.surroundedCells)
                {
                    m_opponentGrid.at(cellAround) = CellState::Missed;
                }
                auto destroyedShipIt = std::find(m_opponentShipDecks.begin(), m_opponentShipDecks.end(), (uint8_t)m_lastHits.size());
                m_opponentShipDecks.erase(destroyedShipIt);
                m_lastHits.clear();
                m_state = AIPlayerState::RandomShooting;
            });
}

AIPlayer::~AIPlayer()
{
    m_eventBus->unsubscribe(m_shotMissedEventHandleId);
    m_eventBus->unsubscribe(m_shipDamagedEventHandleId);
    m_eventBus->unsubscribe(m_shipDestroyedEventHandleId);
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
    return std::string("AI Bot");
}

InputRequest AIPlayer::getInput()
{
    InputRequest turn;
    turn.shotCell = getShootingCell();

    simulateThinking();

    return turn;
}

CellIndex AIPlayer::getShootingCell()
{
    switch (m_state)
    {
    case AIPlayerState::RandomShooting:
    {
        return getRandomShootingCell();
    }
    case AIPlayerState::ShootingAfterHit:
    {
        return getShootingAfterHitCell();
    }
    case AIPlayerState::MiddleGameRandomShooting:
    {
        return getMiddleGameRandomShootingCell();
    }
    default:
    {
        assert(false && "Unprocessed AIPlayerState. Take care of it!");
    }
    }
}

CellIndex AIPlayer::getRandomShootingCell()
{
    const Player opponent = getOppositePlayer(getPlayerType());

    std::vector<CellIndex> concealedCells;

    for (int i = 0; i < m_opponentGrid.data.size(); ++i)
    {
        for (int j = 0; j < m_opponentGrid.data[i].size(); ++j)
        {
            if (m_opponentGrid.data[i][j] == CellState::Concealed)
            {
                concealedCells.push_back(CellIndex(i, j));
            }
        }
    }

    if (concealedCells.size() < m_middleGameCoeff)
    {
        m_state = AIPlayerState::MiddleGameRandomShooting;
        return getMiddleGameRandomShootingCell();
    }

    std::uniform_int_distribution<int> dist(0, (int)concealedCells.size() - 1);

    const CellIndex cell(concealedCells.at(dist(mt)));

    return cell;
}

CellIndex AIPlayer::getShootingAfterHitCell()
{
    const Player opponent = getOppositePlayer(getPlayerType());

    std::vector<CellIndex> possibleCellShots;
    if (m_lastHits.size() > 1)
    {
        std::sort(m_lastHits.begin(), m_lastHits.end(), [](const CellIndex& a, const CellIndex& b)
            {
                return b < a;
            });
        auto firstHit = m_lastHits.begin();
        auto lastHit = m_lastHits.end() - 1;

        if (firstHit->x() - lastHit->x() == 0)
        {
            if (firstHit->y() - lastHit->y() > 0)
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(lastHit->x(), lastHit->y() - 1), // up
                    CellIndex(lastHit->x(), firstHit->y() + 1) // down
                };
            }
            else
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(lastHit->x(), firstHit->y() - 1), // up
                    CellIndex(lastHit->x(), lastHit->y() + 1) // down
                };
            }
        }
        else
        {
            if (firstHit->x() - lastHit->x() > 0)
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(lastHit->x() - 1, lastHit->y()), // left
                    CellIndex(firstHit->x() + 1, firstHit->y()) // right
                };
            }
            else
            {
                possibleCellShots = std::vector<CellIndex>{
                    CellIndex(firstHit->x() - 1, firstHit->y()), // left
                    CellIndex(lastHit->x() + 1, lastHit->y()) // right
                };
            }
        }
    }
    else // m_lastHits.size() <= 1
    {
        assert(m_lastHits.size() == 1);
        auto firstHit = m_lastHits.begin();
        possibleCellShots = std::vector<CellIndex>{
            CellIndex(firstHit->x(), firstHit->y() - 1), // up
            CellIndex(firstHit->x(), firstHit->y() + 1), // down
            CellIndex(firstHit->x() - 1, firstHit->y()), // left
            CellIndex(firstHit->x() + 1, firstHit->y()) // right
        };
        
    }
    std::vector<CellIndex> allowedCells;
    for (auto& c : possibleCellShots)
    {
        if (c.x() < 0 ||
            c.y() < 0 ||
            c.x() >= m_opponentGrid.data.at(0).size() ||
            c.y() >= m_opponentGrid.data.size())
        {
            continue;
        }
        if (m_opponentGrid.at(c) == CellState::Missed ||
            m_opponentGrid.at(c) == CellState::Damaged)
        {
            continue;
        }

        allowedCells.push_back(c);
    }


    std::uniform_int_distribution<int> dist(0, (int)allowedCells.size() - 1);
    const CellIndex cell{ allowedCells.at(dist(mt)) };

    return cell;
}

CellIndex AIPlayer::getMiddleGameRandomShootingCell()
{
    const Player opponent = getOppositePlayer(getPlayerType());

    std::vector<std::vector<CellIndex>> allowedCellsGroups;
    for (int i = 0; i < m_opponentGrid.data.size(); ++i)
    {
        std::vector<CellIndex> allowedCells;
        for (int j = 0; j < m_opponentGrid.data[i].size(); ++j)
        {
            if (m_opponentGrid.data[i][j] == CellState::Concealed)
            {
                allowedCells.push_back(CellIndex(i, j));
            }
            else // m_opponentGrid.data[i][j] != CellState::Concealed
            {
                // exclude groups of cells where size less than max opponent ship
                if (allowedCells.size() >= m_opponentShipDecks.at(0))
                {
                    allowedCellsGroups.push_back(allowedCells);
                }
                allowedCells.clear();
            }
        }
        // if cells combo doesn't stop to the end of row
        if (!allowedCells.empty() && allowedCells.size() >= m_opponentShipDecks.at(0))
        {
            allowedCellsGroups.push_back(allowedCells);
        }

    }

    for (int i = 0; i < m_opponentGrid.data[0].size(); ++i)
    {
        std::vector<CellIndex> allowedCells;
        for (int j = 0; j < m_opponentGrid.data.size(); ++j)
        {
            if (m_opponentGrid.data[j][i] == CellState::Concealed)
            {
                allowedCells.push_back(CellIndex(j, i));
            }
            else // m_opponentGrid.data[j][i] != CellState::Concealed
            {
                // exclude groups of cells where size less than max opponent ship
                if (allowedCells.size() >= m_opponentShipDecks.at(0))
                {
                    allowedCellsGroups.push_back(allowedCells);
                }
                allowedCells.clear();
            }
        }
        // if cells combo doesn't stop to the end of column
        if (!allowedCells.empty() && allowedCells.size() >= m_opponentShipDecks.at(0))
        {
            allowedCellsGroups.push_back(allowedCells);
        }
    }

    std::vector<CellIndex> selectedCellGroup;
    if (allowedCellsGroups.size() > 1)
    {
        std::uniform_int_distribution<int> dist(0, (int)allowedCellsGroups.size() - 1);
        selectedCellGroup = allowedCellsGroups.at(dist(mt));
    }
    else // allowedCellsGroups.size() == 1
    {
        selectedCellGroup = allowedCellsGroups.at(0);
    }
    
    if (selectedCellGroup.size() > 1)
    {
        std::uniform_int_distribution<int> dist2(0, (int)selectedCellGroup.size() - 1);
        return CellIndex(selectedCellGroup.at(dist2(mt)));
    }
    else // selectedCellGroup.size() == 1
    {
        return CellIndex(selectedCellGroup.at(0));
    }
}


void AIPlayer::simulateThinking()
{
    using namespace std::chrono_literals;
    constexpr auto thinkingTime = 350ms;
    std::this_thread::sleep_for(thinkingTime);
}
