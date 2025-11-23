#include "CommandProcessing.h"
#include "GameEngine.h"
#include <sstream>
#include <cstdlib>

// Command Class Implementation:

    // Default Constructor
    Command::Command() {
        command = new string(" ");
        effect = new string(" ");
    }

    // Parameterized Constructor
    Command::Command(const string& cmd, const string& eff) {
        command = new string(cmd);
        effect = new string(" ");
    }

    // Copy COnstructor
    Command::Command(const Command& other) {
        command = new string(*(other.command));
        effect = new string(*(other.effect));
    }

    // Assignment Operator
    Command& Command::operator=(const Command& other) {
        if (this != &other) {
            delete command;
            delete effect;
            command = new string(*(other.command));
            effect = new string(*(other.effect));
        }
        return *this;
    }

    // Stream Insertion Operator
    ostream& operator<<(ostream& os, const Command& cmd) {
        os << "Command: " << *(cmd.command) << ", Effect: " << *(cmd.effect);
        return os;
    }

    // Destructor
    Command::~Command() {
        delete command;  // delete dynamically allocated memory
        delete effect;
    }

    // Getters
    string& Command::getCommand() const {
        return *command;
    }

    string& Command::getEffect() const {
        return *effect;
    }

    void Command::saveEffect(const string& newEffect) {
        *effect = newEffect;
        Notify(this);
    }

    string Command::stringToLog() const {
        return "Command: " + *command + ", Effect: " + *effect;
    }


// CommandProcessor Class Implementation:

    // Default Constructor
    CommandProcessor::CommandProcessor() {
        commands = new vector<Command*>();
        currentState = new GameState(Start);
    }

    // Parameterized Constructor
    CommandProcessor::CommandProcessor(GameState* state) {
        commands = new vector<Command*>();
        currentState = new GameState(*(state));
    }

    // Copy Constructor
    CommandProcessor::CommandProcessor(const CommandProcessor& other) {
        commands = new vector<Command*>();
        // deep copy
        for (auto cmd : *(other.commands)) {
            commands->push_back(new Command(*cmd));
        }
        currentState = new GameState(*(other.currentState));
    }

    // Assignment Operator
    CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
        if (this == &other) return *this;

        for (auto cmd : *(commands)) delete cmd;
        delete commands;
        delete currentState;

        commands = new vector<Command*>();
        for (auto cmd : *(other.commands)) {
            commands->push_back(new Command(*cmd));
        }

        currentState = new GameState(*(other.currentState));
        return *this;
    }

    // Destructor
    CommandProcessor::~CommandProcessor() {
        for (auto cmd : *(commands)) delete cmd;
        delete commands;
        delete currentState;
    }

    // Stream Insertion operator
    ostream& operator<<(ostream& os, const CommandProcessor& cp) {
        os << "CommandProcessor [" << cp.commands->size()
           << " commands], State: " << cp.getStateString();
        // List all commands
        for (auto cmd : *(cp.commands)) {
            os << "\n - " << *cmd;
        }
        return os;
    }

    void CommandProcessor::readCommand() {
        string userCmd;
        cout << "Enter command: ";
        getline(cin, userCmd);
        Command* cmd = new Command(userCmd, " Effect not yet executed.");
        this->saveCommand(cmd);
    }

    void CommandProcessor::saveCommand(Command* cmd) {
        commands->push_back(cmd);
        Notify(this);
    }

    // set method
    void CommandProcessor::setCurrentState(GameState state) {
        *currentState = state;
    }

    // get methods
    Command* CommandProcessor::getCommand() {
        if (!commands->empty()) {
            return commands->back(); // return the last command
        }
        return nullptr;
    }

    GameState CommandProcessor::getCurrentState() const {
        return *currentState;
    }

    void CommandProcessor::printCommands() const {
        for (auto cmd : *(commands)) {
            cout << *cmd << endl;
        }
    }

    // validate method
    bool CommandProcessor::validate(Command* cmd) {
        string cmdStr = cmd->getCommand();

        // Command parser for the name 
        size_t spacePos = cmdStr.find(' ');
        string cmdName;
        string cmdArg;

        if (spacePos != string::npos) {
            cmdName = cmdStr.substr(0, spacePos);
            cmdArg = cmdStr.substr(spacePos + 1);
        }
        else {
            cmdName = cmdStr;
        }

        bool isValid = false;

        switch (*currentState) {

            case Start:

                // --- Tournament Command Support (NEW) ---
                if (cmdName == "tournament") {

                    vector<string> maps;
                    vector<string> strategies;
                    int G = 0, D = 0;

                    if (CommandProcessor::parseTournamentCommand(cmdStr, maps, strategies, G, D)) {
                        cmd->saveEffect("Valid tournament command.");
                        isValid = true;
                    }
                    else {
                        cmd->saveEffect("Invalid tournament command.");
                        isValid = false;
                    }

                    Notify(this);
                    return isValid;
                }
                // --- End NEW tournament code ---

                if (cmdName == "loadmap") {
                    if (!cmdArg.empty()) {
                        cmd->saveEffect("Map " + cmdArg + " loaded successfully.");
                        *currentState = MapLoaded;
                        isValid = true;
                    }
                    else {
                        cmd->saveEffect("Invalid. Error: No map file specified.");
                    }
                }
                else {
                    cmd->saveEffect("Invalid command in Start state.");
                }
                break;

            case MapLoaded:
                if (cmdName == "loadmap") {
                    if (!cmdArg.empty()) {
                        cmd->saveEffect("Map " + cmdArg + " loaded successfully.");
                        isValid = true;
                    }
                    else {
                        cmd->saveEffect("Invalid. Error: No map file specified.");
                    }
                }
                else if (cmdName == "validatemap") {
                    cmd->saveEffect("Map validation successful.");
                    *currentState = MapValidated;
                    isValid = true;
                }
                else {
                    cmd->saveEffect("Invalid command in MapLoaded state.");
                }
                break;

            case MapValidated:
                if (cmdName == "addplayer") {
                    if (!cmdArg.empty()) {
                        cmd->saveEffect("Player " + cmdArg + " added successfully.");
                        *currentState = PlayersAdded;
                        isValid = true;
                    }
                    else {
                        cmd->saveEffect("Invalid. Error: Player name not specified.");
                    }
                }
                else {
                    cmd->saveEffect("Invalid command in MapValidated state.");
                }
                break;

            case PlayersAdded:
                if (cmdName == "addplayer") {
                    if (!cmdArg.empty()) {
                        cmd->saveEffect("Player " + cmdArg + " added successfully.");
                        isValid = true;
                    }
                    else {
                        cmd->saveEffect("Invalid. Error: Player name not specified.");
                    }
                }
                else if (cmdName == "gamestart") {
                    cmd->saveEffect("The game has begun.");
                    *currentState = AssignReinforcements;
                    isValid = true;
                }
                else {
                    cmd->saveEffect("Invalid command in PlayersAdded state.");
                }
                break;

            case Win:
                if (cmdName == "quit") {
                    cmd->saveEffect("Game ended. Exiting...");
                    *currentState = End;
                    isValid = true;
                }
                else if (cmdName == "replay") {
                    cmd->saveEffect("Game restarted. Back to Start now.");
                    *currentState = Start;
                    isValid = true;
                }
                else {
                    cmd->saveEffect("Invalid command in Win state.");
                }
                break;
        }

        Notify(this);
        return isValid;
    }

    string CommandProcessor::stringToLog() const {
        return "CommandProcessor [" + to_string(commands->size()) +
               " commands], State: " + getStateString();
    }

    string CommandProcessor::getStateString() const {
        switch (*currentState) {
            case Start:               return "Start";
            case MapLoaded:           return "MapLoaded";
            case MapValidated:        return "MapValidated";
            case PlayersAdded:        return "PlayersAdded";
            case AssignReinforcements:return "AssignReinforcements";
            case IssueOrders:         return "IssueOrders";
            case ExecuteOrders:       return "ExecuteOrders";
            case Win:                 return "Win";
            case End:                 return "End";
            default:                  return "Unknown State";
        }
    }

    // Tournament command parser
    bool CommandProcessor::parseTournamentCommand(
        const string& cmdStr,
        vector<string>& maps,
        vector<string>& strategies,
        int& games,
        int& maxTurns
    ) {
        maps.clear();
        strategies.clear();
        games = 0;
        maxTurns = 0;

        // Must start with "tournament"
        if (cmdStr.rfind("tournament", 0) != 0)
            return false;

        // Helper to extract comma-separated lists after flags like "-M " and "-P "
        auto extractList = [&](const string& flag) -> vector<string> {
            vector<string> out;
            size_t pos = cmdStr.find(flag);
            if (pos == string::npos) return out;

            pos += flag.size();
            size_t end = cmdStr.find(" -", pos);
            string segment = (end == string::npos)
                ? cmdStr.substr(pos)
                : cmdStr.substr(pos, end - pos);

            string item;
            stringstream ss(segment);
            while (getline(ss, item, ',')) {
                size_t a = item.find_first_not_of(" \t");
                size_t b = item.find_last_not_of(" \t");
                if (a != string::npos && b != string::npos)
                    out.push_back(item.substr(a, b - a + 1));
            }
            return out;
        };

        // Extract lists for -M and -P
        maps       = extractList("-M ");
        strategies = extractList("-P ");

        // Extract integers -G and -D (games per map and max turns)
        size_t gpos = cmdStr.find("-G ");
        if (gpos != string::npos) {
            games = atoi(cmdStr.substr(gpos + 3).c_str());
        }

        size_t dpos = cmdStr.find("-D ");
        if (dpos != string::npos) {
            maxTurns = atoi(cmdStr.substr(dpos + 3).c_str());
        }

        // -------- Validation Rules (Assignment 3) --------
        if (maps.size() < 1 || maps.size() > 5) return false;
        if (strategies.size() < 2 || strategies.size() > 4) return false;

        for (string s : strategies) {
            if (s == "Human") return false; // Human forbidden in tournament
            if (s != "Aggressive" &&
                s != "Benevolent" &&
                s != "Neutral" &&
                s != "Cheater")
                return false;
        }

        if (games < 1 || games > 5) return false;
        if (maxTurns < 10 || maxTurns > 50) return false;

        return true;
    }


// FileCommandProcessorAdapter Class Implementation

    // Parameterized Constructor
    FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string& fName)
    : CommandProcessor()  {
        fileName = new string(fName);
        commandFile = new ifstream(fName);

        if (!commandFile->is_open()) {
            cerr << "Error opening file: " << fName << endl;
        }
    }

    // Copy Constructor
    FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other)
    : CommandProcessor(other) {
        fileName = new string(*(other.fileName));
        commandFile = new ifstream(*fileName);
    }

    // Assignent Operator
    FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
        if (this == &other)
            return *this;

        // Clean up existing resources
        delete fileName;
        if (commandFile) {
            commandFile->close();
            delete commandFile;
        }

        // Deep copy
        fileName = new std::string(*(other.fileName));
        commandFile = new std::ifstream(*fileName);

        // Call base assignment
        CommandProcessor::operator=(other);

        return *this;
    }

    // Destructor
    FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
        delete fileName;
        if (commandFile) {
            if (commandFile->is_open())
                commandFile->close();
            delete commandFile;
        }
    }

    void FileCommandProcessorAdapter::readCommand() {
        string fileCmd;

        if (commandFile->is_open() && getline(*commandFile, fileCmd)) {
            cout << "File command: " << fileCmd << endl;
            Command* cmd = new Command(fileCmd, " Effect not yet executed.");
            this->saveCommand(cmd);
        }
        else {
            cout << "End of command file reached or file not open." << endl;
        }
    }
