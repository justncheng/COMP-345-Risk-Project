#include "CommandProcessing.h"
#include <string>
using namespace std;

void testCommandProcessor(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    testCommandProcessor(argc, argv);
    return 0;

}

void testCommandProcessor(int argc, char* argv[]) {
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

    // read and process commands
    // This only runs for the externam game commands and doesn't show any effect as of now because
    // other Game state cases are not implemented.
    bool continueLoop = true;
    while(continueLoop) {
        processor->readCommand();
        Command* cmd = processor->getCommand();

        processor->validate(cmd);
        cout << *cmd << endl;

        if(processor->getCurrentState() == End) {
            continueLoop = false;
        } 
    }
    delete processor;

}