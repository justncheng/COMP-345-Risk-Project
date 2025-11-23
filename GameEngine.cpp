#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Map.h"
#include "Player.h"
#include "PlayerStrategies.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <iomanip>

// default constructor
GameEngine::GameEngine() {
    currentState = new GameState(Start);
}

// copy constructor
GameEngine::GameEngine(const GameEngine& other) {
    currentState = new GameState(*(other.currentState));
}

// assignment operator
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if(this != &other) {
        delete currentState; // free existing resource
        currentState = new GameState(*(other.currentState)); // deep copy
    }
    return *this;
}

GameEngine::~GameEngine() {
    delete currentState; // free allocated memory
}

void GameEngine::transition(const string& command) {
    //Transition between states based on commands
    switch(*currentState) {
        case Start:
            if(command == "loadmap") {
                *currentState = MapLoaded;
				  Notify(this);
            }
            break;
        case MapLoaded:
            if(command == "validatemap") {
                *currentState = MapValidated;
				 Notify(this);
            }
            break;
        case MapValidated:
            if(command == "addplayer") {
                *currentState = PlayersAdded;
				 Notify(this);
            }
            break;
        case PlayersAdded:
            if(command == "gamestart") {
                *currentState = AssignReinforcements;
				 Notify(this);
            }
            break;
        case AssignReinforcements:
            if(command == "issueorder") {
                *currentState = IssueOrders;
				 Notify(this);
			}
            break;
        case IssueOrders:
            if(command == "issueordersend") {
                *currentState = ExecuteOrders;
				 Notify(this);
			}
            break;
		case ExecuteOrders:
            if(command == "endexecorders") {
				*currentState = AssignReinforcements;
				 Notify(this);
            }
            else if(command == "win") {
                *currentState = Win;
				 Notify(this);
            }
            break;
        case Win:
            if(command == "replay") {
                *currentState = Start;
				 Notify(this);
            }
            else if(command == "quit") {
                *currentState = End;
				 Notify(this);
            }
            break;
        default:
            break;
	}
}

//Run the startup phase of the game
void GameEngine::startupPhase(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck) {
    commandProcessor->readCommand(); //Read in a command
	Command* command = commandProcessor->getCommand(); //Get a command

    map = nullptr;
	players = new vector<Player*>();

    //While a command exists
    while(command != nullptr) {
		bool validCommand = commandProcessor->validate(command); //Validate the command

        if (validCommand) {
            //Split the command into the command name and the command arguement
            string cmdStr = command->getCommand();

            size_t spacePos = cmdStr.find(' ');
            string cmdName;
            string cmdArg;

            if (spacePos != string::npos) {
                cmdName = cmdStr.substr(0, spacePos);
                cmdArg = cmdStr.substr(spacePos + 1);
            }
            else {
                cmdName = cmdStr;
            }

            if (cmdName == "loadmap") {
                //Load map
                map = MapLoader::loadMap(cmdArg);

                //Check if map was successfully loaded
                if (map == nullptr) {
                    command->saveEffect("Could not open file " + cmdArg); //Update the command's effect
                }
                else {
                    this->transition(cmdName); //Move to the MapLoaded state
                }
            }
            else if (cmdName == "validatemap") {
                //Check if the map is valid
                if (!map->validate()) {
					commandProcessor->setCurrentState(MapLoaded); //Reset commandProcessor to MapLoadedState
					command->saveEffect("Map validation failed. Please load a valid map"); //Update the command's effect
                }
                else {
                    this->transition(cmdName); //Move to the MapValidated state
                }
            }
            else if (cmdName == "addplayer") {
                //Check if player maximum is reached
                if (players->size() >= 6) {
                    //Update the command's effect
                    command->saveEffect("Cannot add more players. Maximum limit reached.");
				}
                else {
                    Player* p = new Player(); //Add new player
                    p->setName(cmdArg);
                    players->push_back(p);
                    this->transition(cmdName); //Move to the PlayersAdded state
                }
            }
            else if (cmdName == "gamestart") {
                //Check if player minimum is reached
                if (players->size() < 2) {
					commandProcessor->setCurrentState(PlayersAdded); //Reset commandProcessor to PlayersAdded state
					command->saveEffect("2 players are necessary to start the game. Please add more players."); //Update the command's effect
                }
                else {
					startupPhaseProcess(commandProcessor, map, players, deck); //Run the startup phase process

                    cout << "\n" << command->getEffect() << "\n"; //Print the command's effect

					this->transition(cmdName); //Move to the AssignReinforcements state

                    break;
                }
            }
        }

		cout << "\n" << command->getEffect() << "\n\n"; //Print the command's effect

		commandProcessor->readCommand(); //Read in the next command
		command = commandProcessor->getCommand(); //Get the next command
	}
}

//Run the startup phase process (assign territories, determine order of play, etc.)
void GameEngine::startupPhaseProcess(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck)
{
    srand(time(NULL)); //Seed randomizer

    int territoriesPerPlayer;

    territoriesPerPlayer = map->getTerritories().size() / players->size(); //Calculate territories per player

    //Iterate through all territories in the map
    for (Territory* territory : map->getTerritories()) {
        int randomIndex;

        int totalTerritoriesAssigned = 0;

        for (int i = 0; i < players->size(); i++) {
            totalTerritoriesAssigned += players->at(i)->getTerritories().size();
        }

        //Check if each player has recieved the minimum number of territories per player
        if (totalTerritoriesAssigned == territoriesPerPlayer * players->size()) {
            territoriesPerPlayer++; //Increase territories per player to assign remaining territories
        }

        do {
            randomIndex = rand() % players->size(); //Randomly pick a player
        } while (players->at(randomIndex)->getTerritories().size() >= territoriesPerPlayer); //Check if the player has already recieved the maximum number of territories

        players->at(randomIndex)->addTerritory(territory); //Assign the territory to the player
        territory->setOwner(players->at(randomIndex)); //Assign the player as the owner of the territory
    }

    vector<Player*>* shuffledPlayers = new vector<Player*>(); //Declare a vector of players to randomize order of play

    //While there are still players to be shuffled
    while (players->size() > 0) {
        int randomIndex = rand() % players->size(); //Randomly pick a player
        shuffledPlayers->push_back(players->at(randomIndex)); //Add the player to the shuffled vector
        players->erase(players->begin() + randomIndex); //Remove the player from the original vector
    }

    players = shuffledPlayers; //Assign the original vector to the shuffled vector

    //Iterate through all players
    for (int i = 0; i < players->size(); i++) {
        players->at(i)->setArmies(50); //Assign 50 armies
        players->at(i)->getHand()->addCard(deck->draw()); //Draw a card from the deck
        players->at(i)->getHand()->addCard(deck->draw()); //Draw a card from the deck
    }
}

//Run the main game loop
void GameEngine::mainGameLoop(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck) {
	int currentTurn = 0;
    bool gameover = false;
    while (!gameover) {
        currentTurn++;
		cout << "\nTurn " << currentTurn << "\n-------\n";
		cout << "Game status:\n\n";

        for (Player* p : *players) {
            cout << *p;
        }

        // Clear all negotiate effects from the previous turn
        for (Player* p : *players) {
			p->clearNegotiatedPlayers(); // Lasts only for one turn
        }

        reinforcementPhase(map, players); //Distribute reinforcements to players
        issueOrdersPhase(players, deck); //Players issue orders
		executeOrdersPhase(players); //Execute orders issued by players

        for (int i = 0; i < players->size(); i++) {
            if (players->at(i)->getTerritories().size() == 0) {
				cout << "\n" << players->at(i)->getName() << " has been eliminated from the game.\n";
				delete players->at(i); //Free memory allocated to the player
                players->erase(players->begin() + i);
                i--;
            }
            else if (players->at(i)->getTerritories().size() == map->getTerritories().size()) {
                cout << "\n" << players->at(i)->getName() << " owns all territories and wins the game!\n";
                gameover = true;
                break;
            }
        }
    }
}

//Distribute reinforcements to players
void GameEngine::reinforcementPhase(Map*& map, vector<Player*>*& players) {
	for (Player* player : *players) { //Iterate through all players
        int armiesToAdd = static_cast<int>(player->getTerritories().size() / 3); //Calculate armies to add based on territories owned

		for (Continent* continent : map->getContinents()) { //Iterate through all continents
			bool ownsEntireContinent = true;

			for (Territory* territory : continent->getTerritories()) { //Iterate through all territories in the continent
				if (territory->getOwner() != player) { //Check if the player owns the territory
					ownsEntireContinent = false;
                    break;
                }
			}

            if (ownsEntireContinent) {
				armiesToAdd += continent->getBonus(); //Add continent bonus if player owns entire continent
            }
        }

		armiesToAdd = max(armiesToAdd, 3); //Ensure minimum of 3 armies are added

		player->setArmies(player->getArmies() + armiesToAdd); //Add armies to player
	}
}

void GameEngine::issueOrdersPhase(vector<Player*>*& players, Deck*& deck) {
    bool firstTurn = true;
    bool ordersIssued = true;
    while (ordersIssued) {   // Continue until no orders are issued in a full pass
        ordersIssued = false;
        for (Player* player : *players) {
            if(!firstTurn && player->getPlayerStrategy()->getStrategyString() != "Human")
                continue;
            if (player->issueOrder(deck)) // Issues one order this pass
                ordersIssued = true;  // At least one order was issued this pass
        }

        if(firstTurn) {
            firstTurn = false;
		}
    }
}

void GameEngine::executeOrdersPhase(vector<Player*>*& players) {
    bool ordersRemaining = true;
    // Step 1: Execute all deploy orders for all players
    while (ordersRemaining) {
        ordersRemaining = false;   // Continue until no deploy orders are found in a full pass
        for (Player* player : *players) {
            OrdersList* ordersList = player->getOrdersList();  // Get the player's orders list
            for (int i = 0; i < ordersList->size(); i++) {
                Order* order = ordersList->getOrders() [i];  // Get the order at index i
                // Check if it's a deploy order, execute and remove it
                if (order->getName() == "Deploy") {
                    if (order->validate()) order->execute();  // Execute the order if valid
                    else order->setExecuted(false);     // Mark as not executed if invalid
                    ordersList->remove(i);      // Remove the order from the list
                    ordersRemaining = true;
                    --i;          // Compensate for remove
                }
            }
        }
    }

    // Step 2: Execute all remaining orders in round-robin fashion
    ordersRemaining = true;
    while (ordersRemaining) {    /// Continue until no orders remain
        ordersRemaining = false;
        for (auto player : *players) {   // Iterate through each player
            OrdersList* ordersList = player->getOrdersList();
            if (ordersList->size() > 0) {
                Order* order = ordersList->getOrders()[0];  // Get the first order
                if (order->getName() != "Deploy") {
                    if (order->validate()) {
                        order->execute();
                    }
                    else {
                        order->setExecuted(false);
                        cout << *order << "\n";
                    }
                    ordersList->remove(0);
                    ordersRemaining = true;
                }
            }
        }
    }
}



string GameEngine::getStateString() const {
    switch(*currentState) {
        case Start: return "Start";
        case MapLoaded: return "MapLoaded";
        case MapValidated: return "MapValidated";
        case PlayersAdded: return "PlayersAdded";
        case AssignReinforcements: return "AssignReinforcements";
        case IssueOrders: return "IssueOrders";
        case ExecuteOrders: return "ExecuteOrders";
        case Win: return "Win";
        case End: return "End";
        default: return "Unknown State";
    }
}

ostream& operator << (ostream& os, const GameEngine& gEngine) {
    os << "Current Game State: " << gEngine.getStateString();
    return os;
}
string GameEngine::stringToLog() const {
    return "GameEngine transitioned to state: " + getStateString();
}
void GameEngine::runTournament(const vector<string>& maps,
                               const vector<string>& strategies,
                               int gamesPerMap,
                               int maxTurns)
{
    if (maps.empty() || strategies.size() < 2 || gamesPerMap <= 0 || maxTurns <= 0) {
        cout << "[Tournament] Invalid parameters.\n";
        return;
    }

    // results[mapIndex][gameIndex] = winning strategy or "Draw"
    vector<vector<string>> results(
        maps.size(),
        vector<string>(gamesPerMap, "Draw")
    );

    cout << "=============================================\n";
    cout << "            TOURNAMENT MODE START            \n";
    cout << "=============================================\n\n";

    // Helper: winner is player who owns all map territories, or nullptr
    auto getWinner = [](Map* map) -> Player* {
        Player* owner = nullptr;
        const vector<Territory*>& territories = map->getTerritories();
        if (territories.empty()) return nullptr;

        for (Territory* t : territories) {
            Player* tOwner = t->getOwner();
            if (tOwner == nullptr) return nullptr;  // some unowned -> no winner yet
            if (owner == nullptr) owner = tOwner;
            else if (owner != tOwner) return nullptr; // more than one owner
        }
        return owner;
    };

    random_device rd;
    mt19937 rng(rd());

    for (size_t mi = 0; mi < maps.size(); ++mi) {
        const string& mapFile = maps[mi];
        cout << "[Tournament] Map: " << mapFile << "\n";

        for (int gi = 0; gi < gamesPerMap; ++gi) {
            cout << "  > Game " << (gi + 1) << " on " << mapFile << "\n";

            // ----- Load and validate map -----
            Map* map = MapLoader::loadMap(mapFile);
            if (!map || !map->validate()) {
                cout << "    ! Failed to load/validate map " << mapFile << ". Marking game as Draw.\n";
                results[mi][gi] = "Draw";
                if (map) delete map;
                continue;
            }

            // ----- Create deck -----
            Deck* deck = new Deck();
            for (int k = 0; k < 4; ++k) {
                deck->addCard(unique_ptr<Card>(new Card(CardType::Reinforcement)));
                deck->addCard(unique_ptr<Card>(new Card(CardType::Bomb)));
                deck->addCard(unique_ptr<Card>(new Card(CardType::Blockade)));
                deck->addCard(unique_ptr<Card>(new Card(CardType::Airlift)));
                deck->addCard(unique_ptr<Card>(new Card(CardType::Diplomacy)));
            }

            // ----- Create players with strategies -----
            vector<Player*>* players = new vector<Player*>();
            players->reserve(strategies.size());

            for (size_t si = 0; si < strategies.size(); ++si) {
                const string& sName = strategies[si];

                PlayerStrategy* strat = nullptr;
                // Human is NOT allowed in tournament (CommandProcessor will enforce that)
                if (sName == "Aggressive")      strat = new AggressivePlayerStrategy(nullptr);
                else if (sName == "Benevolent") strat = new BenevolentPlayerStrategy(nullptr);
                else if (sName == "Neutral")    strat = new NeutralPlayerStrategy(nullptr);
                else if (sName == "Cheater")    strat = new CheaterPlayerStrategy(nullptr);
                else                            strat = new AggressivePlayerStrategy(nullptr); // fallback

                string playerName = "P" + to_string(si + 1) + "-" + sName;
                Player* p = new Player(playerName, strat);
                players->push_back(p);
            }

            // ----- Randomly assign territories to players -----
            vector<Territory*> terrVec = map->getTerritories();
            shuffle(terrVec.begin(), terrVec.end(), rng);

            for (size_t ti = 0; ti < terrVec.size(); ++ti) {
                Player* owner = players->at(ti % players->size());
                Territory* t = terrVec[ti];
                owner->addTerritory(t);
                t->setOwner(owner);
            }

            // ----- Initial armies + 2 cards each -----
            for (Player* p : *players) {
                p->setArmies(50);
                if (!deck->isEmpty()) p->getHand()->addCard(deck->draw());
                if (!deck->isEmpty()) p->getHand()->addCard(deck->draw());
            }

            // ----- Fresh engine for this game -----
            GameEngine engine;

            int turn = 0;
            string winnerStr = "Draw";

            while (turn < maxTurns) {
                // Clear negotiations each turn
                for (Player* p : *players) {
                    p->clearNegotiatedPlayers();
                }

                engine.reinforcementPhase(map, players);
                engine.issueOrdersPhase(players, deck);
                engine.executeOrdersPhase(players);

                Player* winner = getWinner(map);
                if (winner != nullptr) {
                    PlayerStrategy* ps = winner->getPlayerStrategy();
                    winnerStr = ps ? ps->getStrategyString() : winner->getName();
                    cout << "    -> Winner: " << winnerStr
                         << " (" << winner->getName() << ")\n";
                    break;
                }

                ++turn;
            }

            if (winnerStr == "Draw") {
                cout << "    -> Game reached max turns (" << maxTurns << "). Result: Draw.\n";
            }

            results[mi][gi] = winnerStr;

            // ----- Cleanup for this game -----
            for (Player* p : *players) {
                delete p;
            }
            delete players;
            delete deck;
            delete map;
        }
    }

    // ----- Print final tournament results table -----
    cout << "\n=============================================\n";
    cout << "            TOURNAMENT RESULTS               \n";
    cout << "=============================================\n";

    cout << left << setw(20) << "Map";
    for (int gi = 0; gi < gamesPerMap; ++gi) {
        cout << setw(15) << ("Game " + to_string(gi + 1));
    }
    cout << "\n";

    for (size_t mi = 0; mi < maps.size(); ++mi) {
        cout << left << setw(20) << maps[mi];
        for (int gi = 0; gi < gamesPerMap; ++gi) {
            cout << setw(15) << results[mi][gi];
        }
        cout << "\n";
    }

    cout << "=============================================\n\n";

    // Log end of tournament if you have a LoggingObserver attached
    Notify(this);
}

