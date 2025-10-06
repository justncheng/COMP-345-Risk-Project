#pragma once

#include <string>
#include <iostream>
using namespace std;

enum GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcements,
    IssueOrders,
    ExecuteOrders,
    Win,
    End
};

class GameEngine {
    private:
        GameState currentState;
    public:
        GameEngine();
        ~GameEngine();
        void transition(const string& command);
        friend ostream& operator << (ostream& os, const GameEngine& gEngine);

        // For testing/debugging
        string getStateString() const;
};
