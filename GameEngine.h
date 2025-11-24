#pragma once

#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "CommandProcessing.h"
#include "LoggingObserver.h" 
using namespace std;

// Forward declarations
class CommandProcessor;
class Player;
class Map;
class Deck;

// Finite State Enum
// enum GameState;

// Game Engine class
class GameEngine : public Subject, public ILoggable{
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

		// function to handle the startup phase
        void startupPhase(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck);
		// function to process the startup phase (assign territories, determine order of play, etc.)
        void startupPhaseProcess(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck);
		// function to handle the main game loop
        void mainGameLoop(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck);
		// function to distribute reinforcements to players
        void reinforcementPhase(Map*& map, vector<Player*>*& players);
        // function to issue orders to players
        void issueOrdersPhase(vector<Player*>*& players, Deck*& deck);
        // function to execute orders 
        void executeOrdersPhase(vector<Player*>*& players);

		 string stringToLog() const override;
			
        // stream inssertion operator overload 
        friend ostream& operator << (ostream& os, const GameEngine& gEngine);

		// Tournament mode function
        void runTournament(const vector<string>& maps, const vector<string>& strategies, int gamesPerMap, int maxTurns);

        // For testing/debugging
        string getStateString() const;
};
