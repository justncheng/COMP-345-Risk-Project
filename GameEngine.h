#pragma once

#include <string>
#include <iostream>
using namespace std;

// Finite State Enum
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

// Game Engine class
class GameEngine {
    private:
        // pointer data member for Game States
        GameState* currentState;
    public:
        // default constructor
        GameEngine();
        // copy constrcutor
        GameEngine(const GameEngine& other);
        // assignment operator
        GameEngine& operator=(const GameEngine& other);
        // destructor
        ~GameEngine();
        // function to transition between states
        void transition(const string& command);
        // stream inssertion operator overload 
        friend ostream& operator << (ostream& os, const GameEngine& gEngine);

        // For testing/debugging
        string getStateString() const;
};
