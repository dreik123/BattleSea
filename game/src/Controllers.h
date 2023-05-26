#pragma once

struct IBattleSeaGame;
struct IBattleSeaView;

// DS: It's expected to communicate between MVC entries via 'event-based' subsystem, 
// but for this moment old-school approach should cover our needs
struct IController
{
    virtual void RunGame() = 0;
};

class GameController : public IController 
{
public:
    GameController(IBattleSeaGame* InGame, IBattleSeaView* InView)
        : Game(InGame)
        , View(InView)
    {
    }

    virtual void RunGame() override;

protected:
    IBattleSeaGame* Game;
    IBattleSeaView* View;
};
