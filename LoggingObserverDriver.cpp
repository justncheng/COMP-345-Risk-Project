/**
 * @file LoggingObserverDriver.cpp
 * @brief Demonstrates Part 5: Game Log Observer integration.
 *
 * This driver tests that Command, CommandProcessor, Order,
 * OrdersList, and GameEngine correctly inherit from Subject
 * and ILoggable, and call Notify(this) when key events occur.
 */

#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "Orders.h"
#include "GameEngine.h"
#include <iostream>
using namespace std;

/**
 * @brief Test function for the Game Log Observer system.
 */
void testLoggingObserver() {
    cout << "=== Testing Game Log Observer Integration ===\n\n";

    LogObserver logger;

    // Attach observer to all key subjects
    CommandProcessor cp;
    cp.Attach(&logger);

    Command* cmd = new Command("loadmap world.map");
    cmd->Attach(&logger);

    OrdersList orders;
    orders.Attach(&logger);

    Order order; // or a concrete subclass like Deploy
    order.Attach(&logger);

    GameEngine engine;
    engine.Attach(&logger);

    // Trigger loggable actions
    cp.saveCommand(cmd);
    cmd->saveEffect("Map successfully loaded.");
    orders.addOrder(&order);
    order.execute();
    engine.transition("assignreinforcement");

    cout << "Log entries written to gamelog.txt.\n";
    cout << "=== End of LoggingObserver Test ===\n\n";

    delete cmd;
}


