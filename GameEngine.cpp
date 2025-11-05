#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Map.h"
#include "Player.h"

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
            }
            break;
        case MapLoaded:
            if(command == "validatemap") {
                *currentState = MapValidated;
            }
            break;
        case MapValidated:
            if(command == "addplayer") {
                *currentState = PlayersAdded;
            }
            break;
        case PlayersAdded:
            if(command == "gamestart") {
                *currentState = AssignReinforcements;
            }
            break;
        case AssignReinforcements:
            if(command == "issueorder") {
                *currentState = IssueOrders;
			}
            break;
        case IssueOrders:
            if(command == "issueordersend") {
                *currentState = ExecuteOrders;
			}
            break;
		case ExecuteOrders:
            if(command == "endexecorders") {
				*currentState = AssignReinforcements;
            }
            else if(command == "win") {
                *currentState = Win;
            }
            break;
        case Win:
            if(command == "replay") {
                *currentState = Start;
            }
            else if(command == "quit") {
                *currentState = End;
            }
            break;
        default:
            break;
	}
}

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
