#ifndef GAMELOGADAPTERS_H
#define GAMELOGADAPTERS_H

#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "Orders.h"
#include "GameEngine.h"
#include <string>

/**
 * @brief Adapter for Command class to add logging functionality
 */
class LoggableCommand : public Subject, public ILoggable {
public:
    Command* command;

    LoggableCommand(Command* cmd) : command(cmd) {}
    std::string stringToLog() const override {
        return "Command executed: " + command->getCommand();
    }
};

/**
 * @brief Adapter for CommandProcessor
 */
class LoggableCommandProcessor : public Subject, public ILoggable {
public:
    CommandProcessor* processor;

    LoggableCommandProcessor(CommandProcessor* cp) : processor(cp) {}
    std::string stringToLog() const override {
        return "CommandProcessor saved a command.";
    }
};

/**
 * @brief Adapter for OrdersList
 */
class LoggableOrdersList : public Subject, public ILoggable {
public:
    OrdersList* ordersList;

    LoggableOrdersList(OrdersList* ol) : ordersList(ol) {}
    std::string stringToLog() const override {
        return "Order added to OrdersList.";
    }
};

/**
 * @brief Adapter for Order
 */
class LoggableOrder : public Subject, public ILoggable {
public:
    Order* order;

    LoggableOrder(Order* o) : order(o) {}
    std::string stringToLog() const override {
        return "Order executed.";
    }
};

/**
 * @brief Adapter for GameEngine
 */
class LoggableGameEngine : public Subject, public ILoggable {
public:
    GameEngine* engine;

    LoggableGameEngine(GameEngine* ge) : engine(ge) {}
    std::string stringToLog() const override {
        return "GameEngine transitioned to a new state.";
    }
};

#endif

