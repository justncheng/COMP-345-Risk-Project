#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

// Finite State Enum
enum GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcements,
    // IssueOrders,
    // ExecuteOrders,
    Win,
    End
};

// Command Class Definition
class Command {
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

};

// CommandProccesor Class Definition
class CommandProcessor {
    private:
        vector<Command*>* commands;  // commands is a pointer to a vector object that holds pointers to Command objects
        GameState* currentState;     // currentState is a pointer to a GameState enum
        
        virtual void readCommand();
        

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

        // get methods
        Command* getCommand();
        GameState getCurrentState() const;
        void printCommands() const;

        // save method
        void saveCommand(Command* cmd);

        // validate method
        void validate(Command* cmd);

};

// FileCommandProcessorAdapter Class Definition
class FileCommandProcessorAdapter : public CommandProcessor {
    private:
        string* fileName;
        ifstream* commandFile;

    protected:
        void readCommand() override;

    public:
        // Parameterized Constructor
        FileCommandProcessorAdapter(const string& file);

        // Copy Constructor
        FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);

        // Assignent Operator
        FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);

        // Destructor
        ~FileCommandProcessorAdapter();

};
