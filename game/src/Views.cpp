#include "Views.h"
#include "Game/Events/Events.h"

#include <iostream>                     // is temporary used for console presentation
#include <queue>
#include <variant>
#include <condition_variable>

// Constants
namespace
{
    constexpr unsigned char LEFT_TOP_EDGE = 201;
    constexpr unsigned char RIGHT_TOP_EDGE = 187;
    constexpr unsigned char LEFT_BOTTOM_EDGE = 200;
    constexpr unsigned char RIGHT_BOTTOM_EDGE = 188;

    constexpr unsigned char LEFT_CENTER_PIECE = 204;
    constexpr unsigned char RIGHT_CENTER_PIECE = 185;
    constexpr unsigned char TOP_CENTER_PIECE = 203;
    constexpr unsigned char BOTTOM_CENTER_PIECE = 202;

    constexpr unsigned char VERTICAL_PIECE = 186;
    constexpr unsigned char HORIZONTAL_PIECE = 205;

    constexpr unsigned char CENTER_PIECE = 206;

    constexpr unsigned char EMPTY_FILLER = 176;
    constexpr unsigned char SHIP_FILLER = 219;
}

constexpr static uint8_t HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL = 3;
constexpr static uint8_t VERTICAL_SYMBOLS_AMOUNT_PER_CELL = 1;
constexpr static uint8_t SPACES_BETWEEN_GRIDS = 10;



// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction (not needed as of C++20, but just keep it as reminder)
//template<class... Ts>
//overloaded(Ts...) -> overloaded<Ts...>;

// This class provided to encapsulate events logic in cpp file and not introduce them to views header.
class RenderInstructionExecutor
{
    // HINT: If you need to extend event type modify PolymophEventType, subscribeToEvents() and processRenderInstruction()
    using PolymophEventType = std::variant<
        events::GameStateChangedEvent,
        events::GridGeneratedEvent,
        events::PlayerTurnsEvent,
        events::FullGridsSyncEvent,
        events::LocalShotErrorEvent,
        events::GameOverEvent
    >;
public:
    RenderInstructionExecutor(TerminalView* renderer, std::shared_ptr<EventBus>& bus);
    ~RenderInstructionExecutor();

    void renderInstructions();
    void renderInstructions(std::stop_token token);

    // Uses copy on purpose in methods below
    void addRenderInstruction(PolymophEventType instruction);
    PolymophEventType popRenderInstruction();
    PolymophEventType popRenderInstructionUnsafe();

private:
    void subscribeToEvents();
    void unsubscribeFromEvents();
    void onGameStateUpdated(const GameState& state);

    void processRenderInstruction(const PolymophEventType& instruction);

private:
    std::queue<PolymophEventType> m_delayedEvents;
    TerminalView* m_renderer;
    std::shared_ptr<EventBus> m_eventBus;

    std::condition_variable m_cv;
    std::mutex m_cv_mutex;

private:
    ListenerHandleId m_gameStateChangedEventHandleId;
    ListenerHandleId m_gridGeneratedEventHandleId;
    ListenerHandleId m_playerTurnsEventHandleId;
    ListenerHandleId m_fullGridsSyncEventHandleId;
    ListenerHandleId m_localShotErrorEventHandleId;
    ListenerHandleId m_gameOverEventHandleId;
};


RenderInstructionExecutor::RenderInstructionExecutor(TerminalView* renderer, std::shared_ptr<EventBus>& bus)
    : m_renderer(renderer)
    , m_eventBus(bus)
{
    subscribeToEvents();
}

RenderInstructionExecutor::~RenderInstructionExecutor()
{
    // probably will not be triggered even
    unsubscribeFromEvents();
}

void RenderInstructionExecutor::renderInstructions()
{
    std::unique_lock<std::mutex> lk(m_cv_mutex);
    m_cv.wait(lk, [this] { return m_delayedEvents.size() > 0; });

    const PolymophEventType instructionCopy = popRenderInstructionUnsafe(); // already under lock
    processRenderInstruction(instructionCopy);
}

void RenderInstructionExecutor::renderInstructions(std::stop_token token)
{
    std::unique_lock<std::mutex> lk(m_cv_mutex);
    m_cv.wait(lk, [this, &token] { return m_delayedEvents.size() > 0 || token.stop_requested(); });

    // Renderer can be destroyed already after waiting, need to check stop token
    if (token.stop_requested())
    {
        return;
    }

    const PolymophEventType instructionCopy = popRenderInstructionUnsafe(); // already under lock
    processRenderInstruction(instructionCopy);
}

void RenderInstructionExecutor::addRenderInstruction(PolymophEventType instruction)
{
    std::unique_lock<std::mutex> lk(m_cv_mutex);
    m_delayedEvents.push(instruction);
    lk.unlock();
    m_cv.notify_one();
}

RenderInstructionExecutor::PolymophEventType RenderInstructionExecutor::popRenderInstruction()
{
    std::unique_lock<std::mutex> lk(m_cv_mutex);
    assert(!m_delayedEvents.empty());
    const PolymophEventType instructionCopy = m_delayedEvents.front();
    m_delayedEvents.pop();
    lk.unlock();

    return instructionCopy;
}

RenderInstructionExecutor::PolymophEventType RenderInstructionExecutor::popRenderInstructionUnsafe()
{
    assert(!m_delayedEvents.empty());
    const PolymophEventType instructionCopy = m_delayedEvents.front();
    m_delayedEvents.pop();

    return instructionCopy;
}


#define SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(eventBus, EventClass) \
eventBus->subscribe<EventClass>([this](const std::any& any_event)   \
{                                                                   \
    const auto event = std::any_cast<EventClass>(any_event);        \
    addRenderInstruction(event);                                    \
});

void RenderInstructionExecutor::subscribeToEvents()
{
    m_gameStateChangedEventHandleId = SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(m_eventBus, events::GameStateChangedEvent);
    m_gridGeneratedEventHandleId = SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(m_eventBus, events::GridGeneratedEvent);
    m_playerTurnsEventHandleId = SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(m_eventBus, events::PlayerTurnsEvent);
    m_fullGridsSyncEventHandleId = SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(m_eventBus, events::FullGridsSyncEvent);
    m_localShotErrorEventHandleId = SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(m_eventBus, events::LocalShotErrorEvent);
    m_gameOverEventHandleId = SUBSCRIBE_EVENT_AS_RENDER_INSTRUCTION(m_eventBus, events::GameOverEvent);

    // TODO damaged and destroyed ship (instead of FullGridsSyncEvent per frame) when it's possible to render certain shot
}

void RenderInstructionExecutor::unsubscribeFromEvents()
{
    m_eventBus->unsubscribe<events::GameStateChangedEvent>(m_gameStateChangedEventHandleId);
    m_eventBus->unsubscribe<events::GridGeneratedEvent>(m_gridGeneratedEventHandleId);
    m_eventBus->unsubscribe<events::PlayerTurnsEvent>(m_playerTurnsEventHandleId);
    m_eventBus->unsubscribe<events::FullGridsSyncEvent>(m_fullGridsSyncEventHandleId);
    m_eventBus->unsubscribe<events::LocalShotErrorEvent>(m_localShotErrorEventHandleId);
    m_eventBus->unsubscribe<events::GameOverEvent>(m_gameOverEventHandleId);
}

void RenderInstructionExecutor::onGameStateUpdated(const GameState& state)
{
    // Expected to make this method thread-safe outside
    switch (state)
    {
    case GameState::Unitialized:
    {
        assert(false && "Invalid game state!!!");
        break;
    }
    case GameState::StartScreen:
    {
        m_renderer->renderStartScreen();
        break;
    }
    case GameState::ShipsSetup:
    {
        system("cls");
        std::cout << "Waiting for ships generation...\n";
        break;
    }
    case GameState::Battle:
    {
        system("cls");
        std::cout << "Waiting for game data...\n";
        break;
    }
    case GameState::GameOver:
    {
        return;
    }
    default:
        assert(false && "Unprocessed game state in TerminalView::onGameStateUpdated()");
        break;
    }
}

void RenderInstructionExecutor::processRenderInstruction(const PolymophEventType& instruction)
{
    // TODO check is it okay to use ref here in lambdas?
    std::visit(overloaded{
        [this](const events::GameStateChangedEvent& event)
        {
            onGameStateUpdated(event.newState);
        },
        [renderer=m_renderer](const events::GridGeneratedEvent& event)
        {
            renderer->renderGeneratedShips(event.playerGridToConfirm);
            std::cout << "Do you like this setup?\nEnter - approve! Any button - regenerate\n";
        },
        [renderer=m_renderer](const events::PlayerTurnsEvent& event)
        {
            renderer->renderRequestToTurn(event.playerName, event.isLocalPlayer);
        },
        [renderer=m_renderer](const events::FullGridsSyncEvent& event)
        {
            renderer->renderGameGrids(event.firstGrid, event.secondGrid);
        },
        [renderer=m_renderer](const events::LocalShotErrorEvent& event)
        {
            renderer->renderShotError(event.errorType);
        },
        [renderer=m_renderer](const events::GameOverEvent& event)
        {
            renderer->renderGameOver(event.winnerName, event.isLocalPlayer);
            std::cout << "Please relaunch game if you want to play again\n";
        },
    }, instruction);
}



TerminalView::TerminalView(std::shared_ptr<EventBus>& bus)
    : m_eventBus(bus)
    , m_renderExecutorImpl(new RenderInstructionExecutor(this, bus))
{
}

void TerminalView::update()
{
    m_renderExecutorImpl->renderInstructions();
}

void TerminalView::updateWithStopToken(std::stop_token token)
{
    m_renderExecutorImpl->renderInstructions(token);
}

void TerminalView::renderStartScreen()
{
    std::cout << "Welcome to Battle Sea game. Enjoy it.\nPress any key to choose your setup\n";
}

void TerminalView::renderGameGrids(const GameGrid modelData1, const GameGrid modelData2)
{
    system("cls");
    renderTwoGrids(modelData1, modelData2, true);
}

void TerminalView::renderShotError(const ShotError error)
{
    switch (error)
    {
    case ShotError::Ok:
        break;
    case ShotError::OutOfGrid:
        std::cout << "Out of grid. Try again\n";
        break;
    case ShotError::RepeatedShot:
        std::cout << "You've already shooted at this cell! Try again\n";
        break;
    default:
        assert(false && "Unexpected ShotError. Please process it!");
        break;
    }
}

void TerminalView::renderGameOver(const std::string winnerName, const bool isLocalPlayer)
{
    std::cout << winnerName << (isLocalPlayer ? "(You)" : "") << " won!\n";
}

void TerminalView::renderGeneratedShips(const GameGrid& grid)
{
    system("cls");
    std::cout << "Generated the ships for you:\n";
    renderSingleGrid(grid);
}

void TerminalView::renderRequestToTurn(const std::string playerName, const bool isLocalPlayer)
{
    std::cout << playerName << (isLocalPlayer ? "(You)" : "")  << " turns:\n";
}

// TODO add ships
void TerminalView::renderSingleGrid(const GameGrid& grid)
{
    assert(grid.data.size() != 0 && grid.data.front().size() != 0);
    assert(grid.data.size() <= ROW_AXIS_NAMES.size());

    const size_t rowCount = grid.data.size();
    const size_t colCount = grid[0].size();

    renderLetterAxisWithAlignment(' ');
    for (int i(1); i <= colCount; i++)
    {
        // TODO improve this to be dependent on some formatting approach
        std::cout << "  " << i << " ";
    }
    std::cout << std::endl;

    renderLetterAxisWithAlignment(' ');
    std::cout << LEFT_TOP_EDGE;
    for (int i(0); i < rowCount - 1; i++)
    {
        renderHorizontalDelimitersPerCell();
        std::cout << TOP_CENTER_PIECE;
    }

    renderHorizontalDelimitersPerCell();
    std::cout << RIGHT_TOP_EDGE << std::endl;

    for (int i(0); i < colCount - 1; i++)
    {
        renderLetterAxisWithAlignment(ROW_AXIS_NAMES[i]);
        for (int j(0); j < rowCount; j++)
        {
            renderVerticalDelimitersPerCell();
            const CellIndex index = CellIndex(i, j);
            renderCell(index, grid.at(index));
        }
        renderVerticalDelimitersPerCell();
        std::cout << std::endl;

        renderLetterAxisWithAlignment(' ');
        std::cout << LEFT_CENTER_PIECE;
        for (int j(0); j < rowCount - 1; j++)
        {
            renderHorizontalDelimitersPerCell();
            std::cout << CENTER_PIECE;
        }
        renderHorizontalDelimitersPerCell();
        std::cout << RIGHT_CENTER_PIECE << std::endl;
    }

    renderLetterAxisWithAlignment(ROW_AXIS_NAMES.back());
    for (int i(0); i < rowCount; i++)
    {
        renderVerticalDelimitersPerCell();
        const CellIndex index = CellIndex(rowCount - 1, i);
        renderCell(index, grid.at(index));
    }

    renderVerticalDelimitersPerCell();
    std::cout << std::endl;

    renderLetterAxisWithAlignment(' ');
    std::cout << LEFT_BOTTOM_EDGE;
    for (int i(0); i < rowCount - 1; i++)
    {
        renderHorizontalDelimitersPerCell();
        std::cout << BOTTOM_CENTER_PIECE;
    }

    renderHorizontalDelimitersPerCell();
    std::cout << RIGHT_BOTTOM_EDGE << std::endl;
}

void TerminalView::renderTwoGrids(const GameGrid& gridLeft, const GameGrid& gridRight, const bool isHorizontally/* = true*/)
{
    if (!isHorizontally)
    {
        renderSingleGrid(gridLeft);
        std::cout << std::endl;
        renderSingleGrid(gridRight);
        std::cout << std::endl;

        return;
    }

    // Check non-emptiness
    assert(gridLeft.data.size() != 0 && gridLeft.data.front().size() != 0);
    assert(gridRight.data.size() != 0 && gridRight.data.front().size() != 0);
    // Check size equality
    assert(gridLeft.data.size() == gridRight.data.size());
    assert(gridLeft.data.front().size() == gridRight.data.front().size());
    // Check that each row has own name
    assert(gridLeft.data.size() <= ROW_AXIS_NAMES.size());

    const size_t rowCount = gridLeft.data.size();
    const size_t colCount = gridLeft[0].size();

    const std::array grids = { gridLeft, gridRight };

    // Numbers top axis
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(' ');
        for (int i(1); i <= colCount; i++)
        {
            // TODO improve this to be dependent on some formatting approach
            std::cout << "  " << i << " ";
        }
        renderGridOffset();
    }
    std::cout << std::endl;

    // Grid top title
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(' ');
        std::cout << LEFT_TOP_EDGE;
        for (int i(0); i < colCount - 1; i++)
        {
            renderHorizontalDelimitersPerCell();
            std::cout << TOP_CENTER_PIECE;
        }

        renderHorizontalDelimitersPerCell();
        std::cout << RIGHT_TOP_EDGE;
        renderGridOffset();
    }
    std::cout << std::endl;

    // N - 1 lines of vertical delimiters and horizontal grid lines
    for (int i(0); i < rowCount - 1; i++)
    {
        for (int count(0); count < grids.size(); count++)
        {
            const GameGrid& grid = grids[count];
            renderLetterAxisWithAlignment(ROW_AXIS_NAMES[i]);
            for (int j(0); j < colCount; j++)
            {
                renderVerticalDelimitersPerCell();
                const CellIndex index = CellIndex(i, j);
                renderCell(index, grid.at(index));
            }
            renderVerticalDelimitersPerCell();

            renderGridOffset();
        }
        std::cout << std::endl;

        for (int count(0); count < grids.size(); count++)
        {
            renderLetterAxisWithAlignment(' ');
            std::cout << LEFT_CENTER_PIECE;
            for (int j(0); j < colCount - 1; j++)
            {
                renderHorizontalDelimitersPerCell();
                std::cout << CENTER_PIECE;
            }
            renderHorizontalDelimitersPerCell();
            std::cout << RIGHT_CENTER_PIECE;

            renderGridOffset();
        }
        std::cout << std::endl;
    }

    // Last layer of vertical delimiters
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(ROW_AXIS_NAMES.back());
        for (int i(0); i < colCount; i++)
        {
            renderVerticalDelimitersPerCell();
            const CellIndex index = CellIndex(colCount - 1, i);
            renderCell(index, grids[count].at(index));
        }

        renderVerticalDelimitersPerCell();
        renderGridOffset();
    }
    std::cout << std::endl;

    // Grid bottom title
    for (int count(0); count < grids.size(); count++)
    {
        renderLetterAxisWithAlignment(' ');
        std::cout << LEFT_BOTTOM_EDGE;
        for (int i(0); i < colCount - 1; i++)
        {
            renderHorizontalDelimitersPerCell();
            std::cout << BOTTOM_CENTER_PIECE;
        }

        renderHorizontalDelimitersPerCell();
        std::cout << RIGHT_BOTTOM_EDGE;

        renderGridOffset();
    }
    std::cout << std::endl;
}

void TerminalView::renderCell(const CellIndex&, const CellState state)
{
    switch (state)
    {
    case CellState::Concealed:
    {
        renderSymbolNTimes(' ', HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
        break;
    }
    case CellState::Damaged:
    {
        // HACK for now, should be fixed with formatting approach
        if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 1)
        {
            std::cout << "x";
        }
        else if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 3)
        {
            std::cout << " x ";
        }
        break;
    }
    case CellState::Destroyed:
    {
        // HACK for now, should be fixed with formatting approach
        if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 1)
        {
            std::cout << "X";
        }
        else if (HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL == 3)
        {
            std::cout << " X ";
        }
        break;
    }
    case CellState::Missed:
    {
        renderSymbolNTimes(EMPTY_FILLER, HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
        break;
    }
    case CellState::Ship:
    {
        renderSymbolNTimes(SHIP_FILLER, HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
        break;
    }
    default:
        break;
    }
}

void TerminalView::renderHorizontalDelimitersPerCell()
{
    renderSymbolNTimes(HORIZONTAL_PIECE, HORIZONTAL_SYMBOLS_AMOUNT_PER_CELL);
}

void TerminalView::renderVerticalDelimitersPerCell()
{
    renderSymbolNTimes(VERTICAL_PIECE, VERTICAL_SYMBOLS_AMOUNT_PER_CELL);
}

void TerminalView::renderGridOffset()
{
    renderSymbolNTimes(' ', SPACES_BETWEEN_GRIDS);
}

void TerminalView::renderLetterAxisWithAlignment(const char symbol)
{
    std::cout << " " << symbol << " ";
}

void TerminalView::renderSymbolNTimes(const char symbol, const unsigned int times)
{
    for (size_t i = 0; i < times; i++)
    {
        std::cout << symbol;
    }
}

