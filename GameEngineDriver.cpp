#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Player.h"
#include "Map.h"
#include <memory>
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

void testStartupPhase() {
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
    cout << "\nCurrent map: " << *map << "\n\n";

    //Print the order of play
    cout << "Order of play:\n";

    for(int i = 0; i < players->size(); i++) {
        cout << players->at(i)->getName() << "\n";
	}

    //Print all player's information

	cout << "\nArmies, Hands, and Territories of players:\n";

    for (int i = 0; i < players->size(); i++) {
        cout << *players->at(i);
    }

	cout << "\n=== Startup Phase Testing Complete ===\n";

	//Delete allocated memory
    delete commandProcessor;
    delete map;
    delete players;
    delete deck;
}

void testMainGameLoop() {
    cout << "=== Main Game Loop Testing ===\n\n";
    
    //Declare necessary objects
    GameEngine gEngine;
    CommandProcessor* commandProcessor = new CommandProcessor();
    Map* map;
    vector<Player*>* players = new vector<Player*>;
    Deck* deck = new Deck();

    //Testing each phase of the main gameplay loop individually for demonstration purposes
 
	//Reinforcement Phase Testing
	map = MapLoader::loadMap("TestMap.map"); //Load a test map

	//Add players
    Player* p1 = new Player();      // default constructor sets HumanPlayerStrategy(this)
    p1->setName("Player 1");
    players->push_back(p1);

    Player* p2 = new Player();
    p2->setName("Player 2");
    players->push_back(p2);

    Player* p3 = new Player();
    p3->setName("Player 3");
    players->push_back(p3);

	//Distribute territories manually for testing

	//Scenario 1: Player owns all territories in a continent
    for(Territory* territory : (map->getContinents()).at(0)->getTerritories()) {
        players->at(0)->addTerritory(territory);
        territory->setOwner(players->at(0));
	}

	//Scenario 2: Player owns some territories but not all territories in any continent
    for (Territory* territory : (map->getContinents()).at(1)->getTerritories()) {
        if (players->at(1)->getTerritories().size() >= 3) {
            break;
        }

        players->at(1)->addTerritory(territory);
        territory->setOwner(players->at(1));
    }

    for (Territory* territory : (map->getContinents()).at(2)->getTerritories()) {
        if (players->at(1)->getTerritories().size() >= 6) {
            break;
        }

        players->at(1)->addTerritory(territory);
        territory->setOwner(players->at(1));
    }

    for (Territory* territory : (map->getContinents()).at(3)->getTerritories()) {
        if (players->at(1)->getTerritories().size() >= 9) {
            break;
        }

        players->at(1)->addTerritory(territory);
        territory->setOwner(players->at(1));
    }

    for (Territory* territory : (map->getContinents()).at(4)->getTerritories()) {
        if (players->at(1)->getTerritories().size() >= 12) {
            break;
        }

        players->at(1)->addTerritory(territory);
        territory->setOwner(players->at(1));
    }

	//Scenario 3: Player owns only a few territories, so they receive minimum reinforcements
    for (Territory* territory : (map->getContinents()).at(5)->getTerritories()) {
        if (players->at(2)->getTerritories().size() >= 2) {
            break;
        }

        players->at(2)->addTerritory(territory);
        territory->setOwner(players->at(2));
    }

	//Display players' territories and armies before reinforcement phase
	cout << "Before reinforcement phase:\n\n";
    for(Player* player : *players) {
		cout << player->getName() << " has " << player->getArmies() << " armies and owns " << player->getTerritories().size() << " territories";

        if (player->getTerritories().size() > 0) {
            cout << ":\n";
        }
        else {
            cout << ".\n";
        }

        for(Territory* territory : player->getTerritories()) {
            cout << "- " << territory->getName() << "\n";
        }

		cout << "\n";
	}

	gEngine.reinforcementPhase(map, players);

	//Display players' armies after reinforcement phase
    cout << "After reinforcement phase:\n\n";
    for (Player* player : *players) {
		cout << player->getName() << " has " << player->getArmies() << " armies.\n";
    }

    cout << "\nBefore issue orders phase:\n\n";
    for (Player* player : *players) {
        cout << player->getName() << " ordersList's: " << *(player->getOrdersList()) << "\n";
    }

    //Order Issuing Phase Testing
    gEngine.issueOrdersPhase(players, deck);   

    cout << "After issue orders phase:\n\n";
    for (Player* player : *players) {
        cout << player->getName() << " ordersList's: " << *(player->getOrdersList()) << "\n";
    }

	//Order Execution Phase Testing

    cout << "Execute Orders Phase\n\n";
	gEngine.executeOrdersPhase(players); 

	players->at(0)->setTerritories(list<Territory*>()); //Manually eliminate Player 1 for testing purposes
    
	delete players->at(2); //Delete Player 3 to avoid memory leak
	players->erase(players->begin() + 2); //Remove Player 3 from the players vector

    for(Territory* territory : map->getTerritories()) {
        if (territory->getOwner() != players->at(1)) {
            players->at(1)->addTerritory(territory); //Give all territories to Player 2 for testing purposes
            territory->setOwner(players->at(1));
        }
	}

    players->at(1)->getHand()->addCard(make_unique<Card>(CardType::Reinforcement));

    cout << "\n";

	//Testing the main game loop
    cout << "Main Game Loop\n\n";
	gEngine.mainGameLoop(commandProcessor, map, players, deck);

    cout << "\n=== Main Game Loop Testing Complete ===\n";

    //Delete allocated memory
    delete commandProcessor;
    delete map;
    delete players;
    delete deck;
}
