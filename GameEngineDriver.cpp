#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Player.h"
#include "Map.h"
using namespace std;

void testGameStates() {
    cout << "=== Game States Testing ===\n\n";

    GameEngine gEngine;
    string command;
    cout << gEngine << endl;
    // Loop until the state is end
    while(gEngine.getStateString() != "End") {
        cout << "Enter command: ";
        cin >> command;
        gEngine.transition(command);
        cout << "\n";
        cout << gEngine << endl;
    }

    cout << "\n=== Game States Testing Complete ===";
}

void testStartupPhase()
{
    cout << "=== Startup Phase Testing ===\n\n";

	//Declare necessary objects
    GameEngine gEngine;
	CommandProcessor* commandProcessor = new CommandProcessor();
	Map* map;
	vector<Player*>* players;
	Deck* deck = new Deck();

	//Add cards to the deck

    deck->addCard(make_unique<Card>(CardType::Reinforcement));
    deck->addCard(make_unique<Card>(CardType::Bomb));
    deck->addCard(make_unique<Card>(CardType::Blockade));
    deck->addCard(make_unique<Card>(CardType::Airlift));
    deck->addCard(make_unique<Card>(CardType::Diplomacy));

    deck->addCard(make_unique<Card>(CardType::Reinforcement));
    deck->addCard(make_unique<Card>(CardType::Bomb));
    deck->addCard(make_unique<Card>(CardType::Blockade));
    deck->addCard(make_unique<Card>(CardType::Airlift));
    deck->addCard(make_unique<Card>(CardType::Diplomacy));

    deck->addCard(make_unique<Card>(CardType::Reinforcement));
    deck->addCard(make_unique<Card>(CardType::Bomb));
    deck->addCard(make_unique<Card>(CardType::Blockade));
    deck->addCard(make_unique<Card>(CardType::Airlift));
    deck->addCard(make_unique<Card>(CardType::Diplomacy));

    deck->addCard(make_unique<Card>(CardType::Reinforcement));
    deck->addCard(make_unique<Card>(CardType::Bomb));
    deck->addCard(make_unique<Card>(CardType::Blockade));
    deck->addCard(make_unique<Card>(CardType::Airlift));
    deck->addCard(make_unique<Card>(CardType::Diplomacy));

    //Run the startup phase
	gEngine.startupPhase(commandProcessor, map, players, deck);

    //Print the map
    cout << *map << "\n\n";

    //Print the order of play
    cout << "Order of play:\n";

    for(int i = 0; i < players->size(); i++) {
        cout << players->at(i)->getName() << "\n";
	}

    //Print all player's information

	cout << "\nArmies, Hands, and Territories of players:\n\n";

    for (int i = 0; i < players->size(); i++) {
        cout << *players->at(i) << "\n";
    }

	cout << "\n=== Startup Phase Testing Complete ===\n\n";

	//Delete allocated memory

    delete commandProcessor;
    delete map;
    delete players;
    delete deck;
}
