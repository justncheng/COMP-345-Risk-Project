#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "LoggingObserver.h"
using namespace std;

enum GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcements,
    IssueOrders,
    ExecuteOrders,
    Win,
    End
};

// Command Class Definition
class Command : public Subject, public ILoggable {
    private:
        // Dynamically allocated string variables
        string* command;
        string* effect;

    public:
        // Default Constructor
        Command();

        // Parameterized Constructor
        Command(const string& cmd, const string& eff);

        // Copy COnstructor
        Command(const Command& other);

        // Assignment Operator
        Command& operator=(const Command& other);

        // Stream Insertion Operator
        friend ostream& operator<<(ostream& os, const Command& cmd);

        // Destructor
        ~Command();

        // Getters
        string& getCommand() const;
        string& getEffect() const;

        void saveEffect(const string& newEffect);

		string stringToLog() const override;

};

// CommandProccesor Class Definition
class CommandProcessor : public Subject, public ILoggable  {
    private:
        vector<Command*>* commands;  // commands is a pointer to a vector object that holds pointers to Command objects
        GameState* currentState;     // currentState is a pointer to a GameState enum

    public:
        // Default Constructor
        CommandProcessor();

        // Parameterized Constructor
        CommandProcessor(GameState* state);

        // Copy Constructor
        CommandProcessor(const CommandProcessor& other);

        // Assignment Operator
        CommandProcessor& operator=(const CommandProcessor& other);

        // Destructor
        ~CommandProcessor();

        // Stream Insertion operator
        friend ostream& operator<<(ostream& os, const CommandProcessor& cp);

		// readCommand method
        virtual void readCommand();

        // set method
		void setCurrentState(GameState state);

        // get methods
        Command* getCommand();
        GameState getCurrentState() const;
        void printCommands() const;

        // save method
        void saveCommand(Command* cmd);

        // validate method
        bool validate(Command* cmd);

	 	string stringToLog() const override;

};

// FileCommandProcessorAdapter Class Definition
class FileCommandProcessorAdapter : public CommandProcessor {
    private:
        string* fileName;
        ifstream* commandFile;

    public:
        // Parameterized Constructor
        FileCommandProcessorAdapter(const string& file);

        // Copy Constructor
        FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);

        // Assignent Operator
        FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);

        // Destructor
        ~FileCommandProcessorAdapter();

		// Overriden readCommand method
        void readCommand() override;
};
