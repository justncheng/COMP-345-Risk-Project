#include "GameEngine.h"

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
    switch(*currentState) {
        case Start:
            if (command == "loadmap") {
                *currentState = MapLoaded;
                cout << "Map Loaded" << endl;
            }
            else {
                cout << "Invalid command. Please load a map using 'loadmap'." << endl;
            }
            break;

        case MapLoaded:
            if(command == "validatemap") {
                *currentState = MapValidated;
                cout << "Map Validated" << endl;
            }
            else if(command == "loadmap") {
                cout << "Map reloaded." << endl;
            }
            else {
                cout << "Invalid command. Please validate the map using 'validatemap' or load a new map using 'loadmap'." << endl;
            }
            break;
        case MapValidated:
            if(command == "addplayer") {
                *currentState = PlayersAdded;
                cout << "Player Added" << endl;
            }
            else {
                cout << "Invalid command. Please add players using 'addplayer'" << endl;
            }
            break;
        case PlayersAdded:
            if(command == "assigncountries") {
                *currentState = AssignReinforcements;
                cout << "Reinforcements/countries Assigned" << endl;
            }
            else if(command == "addplayer") {
                *currentState = PlayersAdded;
                cout << "Another Player Added" << endl;
            }
            else {
                cout << "Invalid command. Please assign reinforcements using 'assigncountries'" << endl;
            }
            break;
        case AssignReinforcements:
            if(command == "issueorder") {
                *currentState = IssueOrders;
                cout << "Orders Issued" << endl;
            }
            else {
                cout << "Invalid command. Please issue orders using 'issueorder'" << endl;
            }
            break;
        case IssueOrders:
            if(command == "endissueorders") {
                *currentState = ExecuteOrders;
                cout << "Orders Executed" << endl;
            }
            else if(command == "issueorder") {
                *currentState = IssueOrders;
                cout << "More Orders Issued" << endl;
            }
            else {
                cout << "Invalid command. Please execute orders using 'executeorders'" << endl;
            }
            break;
        case ExecuteOrders:
            if(command == "win") {
                *currentState = Win;
                cout << "Game Won. Enter 'end' to end the game or 'play' to play another game" << endl;
            }
            else if (command == "execorder") {
                *currentState = ExecuteOrders;
                cout << "Another Order Executed" << endl;
            }
            else if (command == "endexecorders") {
                *currentState = AssignReinforcements;
                cout << "Orders Execution Phase Ended. Back to Assign Reinforcements." << endl;
            }
            else {
                cout << "Invalid command. Please declare a winner using 'win'" << endl;
            }
            break;

        case Win:
            if(command == "end") {
                *currentState = End;
                cout << "Game Ended" << endl;
            }
            else if (command == "play") {
                *currentState = Start;
                cout << "Starting a new game." << endl;
            }
            else {
                cout << "Invalid command. Please end the game using 'end'" << endl;
            }
            break;
        default:
            cout << "Game has ended. No further commands accepted." << endl;
            break;
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
