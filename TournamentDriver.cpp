#include "CommandProcessing.h"
#include "GameEngine.h"
#include <iostream>
using namespace std;

/*
 * testTournament()
 * Demonstrates:
 *  1. Processing of the tournament command
 *  2. Validation through CommandProcessor::validate()
 *  3. Execution through GameEngine::runTournament()
 *
 * Required by Assignment 3
 */
void testTournament()
{
    cout << "=== Tournament Mode Driver ===\n\n";

    // Example tournament command (can be edited for demo)
    string inputCmd = "tournament -M map1.map,map2.map -P Aggressive,Cheater -G 3 -D 30";
    cout << "Processing command: " << inputCmd << "\n\n";

    CommandProcessor cp;
    Command* cmd = new Command(inputCmd, " ");

    // 1. Validate the command
    bool valid = cp.validate(cmd);

    cout << "Validation Result: " << (valid ? "VALID" : "INVALID") << "\n";
    cout << "Command Effect: " << cmd->getEffect() << "\n\n";

    if (!valid) {
        cout << "Command rejected. Ending driver.\n";
        delete cmd;
        return;
    }

    // 2. Parse tournament parameters
    vector<string> maps;
    vector<string> strategies;
    int games = 0;
    int maxTurns = 0;

    CommandProcessor::parseTournamentCommand(inputCmd, maps, strategies, games, maxTurns);

    // 3. Show extracted parameters
    cout << "Tournament Parameters:\n";
    cout << "Maps: ";
    for (auto& m : maps) cout << m << " ";
    cout << "\nStrategies: ";
    for (auto& s : strategies) cout << s << " ";
    cout << "\nGames per map: " << games;
    cout << "\nMax turns: " << maxTurns << "\n\n";

    // 4. Run the tournament
    GameEngine engine;
    cout << "=== Running Tournament ===\n\n";
    engine.runTournament(maps, strategies, games, maxTurns);

    cout << "\n=== Tournament Driver Complete ===\n";

    delete cmd;
}
