#include "GameEngine.h"
using namespace std;

void testGameStates() {
    GameEngine gEngine;
    string command;
    cout << gEngine << endl;
    // Loop until the state is end
    while(gEngine.getStateString() != "End") {
        cout << "Enter command: ";
        cin >> command;
        gEngine.transition(command);
        cout << gEngine << endl;    
    }

    
}
