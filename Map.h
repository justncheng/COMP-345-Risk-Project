#pragma once
using namespace std;

class Player; // Forward declaration to avoid circular dependency

class Territory
{
public:
    bool operator != (const Territory& territory) {return true;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
    friend ostream& operator << (ostream &output, const Territory& territory) {return output;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
    Territory() : name(""), owner(nullptr) {}

    Territory(const string& name, Player* owner)
        : name(name), owner(owner) {} //DECLARED FOR ORDERS FUNCTIONALITY, PLEASE IMPLEMENT
    
    const string& getName() const { return name; }
    Player* getOwner() const { return owner; } //DECLARED FOR ORDERS FUNCTIONALITY, PLEASE IMPLEMENT

private:
    string name;
	Player* owner; // DECLARED FOR ORDERS FUNCTIONALITY, PLEASE IMPLEMENT
};