#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Map.h"
#include "Player.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

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
                    players->push_back(new Player(cmdArg)); //Add new player
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
                    srand(time(NULL)); //Seed randomizer

                    int territoriesPerPlayer;

                    territoriesPerPlayer = map->getTerritories().size() / players->size(); //Calculate territories per player

                    //Iterate through all territories in the map
                    for (Territory* territory : map->getTerritories()) {
                        int randomIndex;

						int totalTerritoriesAssigned = 0;

                        for(int i = 0; i < players->size(); i++) {
							totalTerritoriesAssigned += players->at(i)->getTerritories().size();
						}

                        //Check if each player has recieved the minimum number of territories per player
                        if(totalTerritoriesAssigned == territoriesPerPlayer * players->size()) {
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
                    while(players->size() > 0) {
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

                    cout << "\n" << command->getEffect() << "\n"; //Print the command's effect

					this->transition(cmdName); //Move to the AssignReinforcements state

                    //SWITCH TO PLAY PHASE

                    break;
                }
            }
        }

		cout << "\n" << command->getEffect() << "\n\n"; //Print the command's effect

		commandProcessor->readCommand(); //Read in the next command
		command = commandProcessor->getCommand(); //Get the next command
	}
}

//Run the main game loop
void GameEngine::mainGameLoop(CommandProcessor*& commandProcessor, Map*& map, vector<Player*>*& players, Deck*& deck) {
	reinforcementPhase(map, players); //Distribute reinforcements to players
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

void GameEngine::issueOrdersPhase(vector<Player*>*& players) {
    bool ordersIssued = true;
    while (ordersIssued) {   // Continue until no orders are issued in a full pass
        ordersIssued = false;
        for (Player* player : *players) {
            if (player->hasOrdersToIssue()) {
                player->issueOrder(); // Issues one order this pass
                ordersIssued = true;  // At least one order was issued this pass
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
