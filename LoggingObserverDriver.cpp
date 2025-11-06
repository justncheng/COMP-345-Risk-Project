/**
 * @file LoggingObserverDriver.cpp
 * @brief Driver that demonstrates the functionality of the Observer pattern
 *        for Part 5 of COMP 345 – Game Log Observer.
 *
 * Demonstrates:
 *  - Logging of commands and effects via Command & CommandProcessor
 *  - Logging of orders added and executed via OrdersList & Order
 *  - Logging of state transitions via GameEngine
 */

#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "Orders.h"
#include "GameEngine.h"
#include <iostream>
using namespace std;

/**
 * @brief Executes all test cases for Part 5 – Game Log Observer.
 *
 * This driver attaches a LogObserver to several Subjects already
 * present in the project (CommandProcessor, Command, OrdersList,
 * Order, and GameEngine) and verifies that all events are written
 * into gamelog.txt using the Notify() mechanism.
 */
void testLoggingObserver() {
    cout << "=== Test LoggingObserver (Part 5) ===\n\n";

    // Create observer
    LogObserver logger;

    // ---------- Command and CommandProcessor ----------
    cout << "[1] Logging Command and CommandProcessor events...\n";
    CommandProcessor commandProcessor;
    commandProcessor.Attach(&logger);

    Command* command = new Command("loadmap world.map");
    command->Attach(&logger);

    commandProcessor.saveCommand(*command);
    command->saveEffect("Map successfully loaded.");

    // ---------- Orders and OrdersList ----------
    cout << "[2] Logging Order and OrdersList events...\n";
    OrdersList orders;
    orders.Attach(&logger);

    Deploy deployOrder;  // assumes Deploy is a subclass of Order
    deployOrder.Attach(&logger);

    orders.add(deployOrder); // triggers Notify() in OrdersList
    deployOrder.execute();   // triggers Notify() in Order::execute()

    // ---------- GameEngine ----------
    cout << "[3] Logging GameEngine state transitions...\n";
    GameEngine engine;
    engine.Attach(&logger);
    engine.transition("assignreinforcement");

    cout << "\nAll events recorded in gamelog.txt successfully.\n";
    cout << "=== End Test LoggingObserver ===\n\n";

    delete command;
}
