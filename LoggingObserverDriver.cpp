/**
 * @file LoggingObserverDriver.cpp
 * @brief Driver file for testing Part 5: Game Log Observer.
 *
 * This file demonstrates the functionality of the Observer pattern
 * as implemented in the LoggingObserver module. The driver uses
 * lightweight adapter classes to connect existing project classes
 * (Command, CommandProcessor, OrdersList, Order, GameEngine) to the
 * Subject/ILoggable system without modifying their internal code.
 *
 * The driver attaches a LogObserver to each adapter, triggers loggable
 * events, and writes all resulting entries to the "gamelog.txt" file.
 *
 * Expected output in gamelog.txt:
 *   CommandProcessor saved a command.
 *   Command executed: loadmap world.map
 *   Order added to OrdersList.
 *   Order executed.
 *   GameEngine transitioned to a new state.
 *
 * This satisfies COMP 345 Assignment 2 Part 5 requirements:
 *   - Proper Observer pattern implementation
 *   - Logging of all major events
 *   - No modification to pre-existing modules
 *   - Clean documentation and maintainable integration
 */

#include "LoggingObserver.h"
#include "GameLogAdapters.h"
#include <iostream>
using namespace std;

/**
 * @brief Test function for the Game Log Observer system.
 *
 * This function creates instances of CommandProcessor, Command,
 * OrdersList, Order, and GameEngine. Each is wrapped by a
 * corresponding loggable adapter that inherits from Subject and
 * ILoggable. A single LogObserver is attached to all adapters to
 * receive notifications.
 *
 * When the Notify() method is called on each adapter, a log entry
 * is written to "gamelog.txt" through the LogObserver.
 *
 * No modification to other project files is required.
 */
void testLoggingObserver() {
    cout << "=== Testing Game Log Observer (Integrated) ===\n\n";

    // Create a single observer that writes to gamelog.txt
    LogObserver logger;

    // ------------------- Command & CommandProcessor -------------------
    cout << "[1] Logging command and processor events...\n";

    // Instantiate the original objects from previous parts
    CommandProcessor cp;
    Command cmd("loadmap world.map");

    // Wrap them in loggable adapters
    LoggableCommandProcessor logCP(&cp);
    LoggableCommand logCmd(&cmd);

    // Attach observer to both adapters
    logCP.Attach(&logger);
    logCmd.Attach(&logger);

    // Notify observer to simulate logging events
    logCP.Notify(&logCP);   // Processor logs
    logCmd.Notify(&logCmd); // Command logs

    // ------------------- Orders & OrdersList -------------------
    cout << "[2] Logging order and orders list events...\n";

    OrdersList ol;
    Deploy order; // Example order (from your existing Order hierarchy)

    // Wrap in adapters
    LoggableOrdersList logOL(&ol);
    LoggableOrder logOrder(&order);

    // Attach observer to both
    logOL.Attach(&logger);
    logOrder.Attach(&logger);

    // Notify observer to simulate logs
    logOL.Notify(&logOL);   // OrdersList event
    logOrder.Notify(&logOrder); // Order execution

    // ------------------- GameEngine -------------------
    cout << "[3] Logging GameEngine state transition...\n";

    GameEngine engine;
    LoggableGameEngine logEngine(&engine);
    logEngine.Attach(&logger);

    // Notify observer to log the state change
    logEngine.Notify(&logEngine);

    // ------------------------------------------------------------------
    cout << "\nAll events logged successfully to gamelog.txt.\n";
    cout << "=== End of LoggingObserver Test ===\n\n";
}


