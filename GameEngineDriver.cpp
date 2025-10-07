#include "GameEngine.h"
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
