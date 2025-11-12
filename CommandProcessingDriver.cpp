#include "CommandProcessing.h"
#include <string>
using namespace std;


void testCommandProcessor(int argc, char* argv[]) {
    cout << "=== Command Processing Testing ===\n\n";

    CommandProcessor* processor;

    // Parse command line arguments
    if(argc >= 2 && string(argv[1]) == "-console") {
        processor = new CommandProcessor();
    }
    else if (argc >= 3 && string(argv[1]) == "-file") {
        processor = new FileCommandProcessorAdapter(string(argv[2]));
    }
    else {
        cerr << "Usage: " << argv[0] << " -console OR " << argv[0] << " -file <filename>" << endl;
        return;
    }

    // Read and process commands
    // This only runs for the external game commands and doesn't show any effect for internal game commands 
    // as of now because internal Game state cases are implemented in GameEngine class.

    bool continueLoop = true;
    while(continueLoop) {
        processor->readCommand();
        Command* cmd = processor->getCommand();

        processor->validate(cmd);
        cout << *cmd << "\n\n";

        if (processor->getCurrentState() == AssignReinforcements) {
            processor->setCurrentState(Win);
        }

        if(processor->getCurrentState() == End) {
            continueLoop = false;
        }
    }
    delete processor;
    
    cout << "=== Command Processing Testing Complete ===\n\n";
}