#pragma once
#include <memory>

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
    GameController(std::shared_ptr<IBattleSeaGame>& InGame, std::shared_ptr<IBattleSeaView>& InView);

    virtual void RunGame() override;

protected:
    std::shared_ptr<IBattleSeaGame> Game;
    std::shared_ptr<IBattleSeaView> View;
};
