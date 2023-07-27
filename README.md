# BattleSea

The project is terminal-based implementation of well-known Battle Sea game.
The goal of this work was getting experience with the latest C++17 and C++20 features.
There are still several improvements to do covered in main.cpp file.

--------
There is a list of used features and places where you can take a look at them(please check extended examples below):
- **C++17**
    - std::variant (implementing of runtime polymorphysm)
    - class templace argument deduction
    - [[nodiscard]] attribute
    - std::any (for EventBus listeners implementation)
    - std::optional (for possible shot which may be unset in InputRequest)
    - constexpr variables and functions
- **C++20**
    - concepts
    - std::jthread (thread with std::stop_token)
    - named parameters



You can see some of them in-place below:

#### Usage of std::variant
```c
using PolymophEventType = std::variant<
    events::GameStateChangedEvent,
    events::GridGeneratedEvent,
    events::PlayerTurnsEvent,
    events::FullGridsSyncEvent,
    events::LocalShotErrorEvent,
    events::GameOverEvent
>;

std::visit(overloaded{
    [this](const events::GameStateChangedEvent& event)
    {
        onGameStateUpdated(event.newState);
    },
    [renderer=m_renderer](const events::GridGeneratedEvent& event)
    {
        renderer->renderGeneratedShips(event.playerGridToConfirm);
    },
    ...
    [renderer=m_renderer](const events::GameOverEvent& event)
    {
        renderer->renderGameOver(event.winnerName, event.isLocalPlayer);
    },
}, instruction); // instruction is PolymophEventType

// Please check src\Views.cpp file (RenderInstructionExecutor::processRenderInstruction)
```
---
#### Usage of std::any in context of EventBus
```c
class EventBus
{
public:
    using EventListener = std::function<void(const std::any&)>;

    template <typename EventType>
    ListenerHandleId subscribe(const EventListener& listener);

    void unsubscribe(const ListenerHandleId& handleId);

    template <typename DerivedEvent>
    void publish(const DerivedEvent& event);

private:
    ...
}


// Subscription
    m_shotMissedEventHandleId = m_eventBus->subscribe<events::ShotMissedEvent>([this](const std::any& anyEvent)
    {
        const auto event = any_cast<events::ShotMissedEvent>(anyEvent);
        if (event.shootingPlayer != getPlayerType())
        {
            return;
        }

        m_opponentGrid.at(event.shot) = CellState::Missed;
    });

// Publishing
    const events::ShotMissedEvent shotMissedEvent{.shootingPlayer = m_currentPlayer, .shot = cell};
    m_eventBus->publish(shotMissedEvent);
```
---
#### Usage of named arguments
```c
    const events::PlayerSwitchedEvent playerSwitchedEvent{.previousPlayer = prevPlayer, .nextPlayer = m_currentPlayer};

    const events::FullGridsSyncEvent fullGridsSyncEvent
    {
        .firstGrid = getPlayerGridInfo(Player::Player1),
        .secondGrid = getPlayerGridInfo(Player::Player2),
    };
```
---
#### Usage of concepts from C++20, named restictions for the template type
```c
    template <typename T>
    concept EnumType = std::is_enum_v<T>();

    template<typename EnumType>
    class BitMask { /*implementation*/ }
```

<br>

build and launch
----------------------

It's kind of simple project for implementation practice mostly, so main platform is Windows.
The project is developed to be used with Visual Studio, you may find .sln file in repo.


Technically it's possible to launch it from cmd:

```c
git clone <project>
msbuild /m /p:Configuration=Release .
bin\game\Release\BattleSeaGame.exe
```

license
----------------------
The game project is under MIT license, so you are free to copy, reuse modify this code in your needs (projects, work).
