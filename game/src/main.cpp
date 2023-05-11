// BattleSea.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

class GameController;

struct IGame {};
class BattleSeaGame : public IGame {};

struct IView {};
class TerminalPresenter : public IView {};

GameController* CreateController(IGame* game, IView* presenter) { return nullptr; }

struct GameConfig {}; // containing info about amount and type of ships
IGame* CreateGame(const GameConfig&) { return nullptr; }

IView* CreatePresenter() { return new TerminalPresenter(); }


int main(int argc, char* argv[])
{
    // Classic MVC
    IGame* game = CreateGame(GameConfig());
    IView* presenter = CreatePresenter();
    GameController* controller = CreateController(game, presenter);

    return 0;
}
