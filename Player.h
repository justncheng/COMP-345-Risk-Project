#pragma once

#include <iostream>
#include <list>
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

using namespace std;

//Forward declarations to avoid circular dependency
class Territory;
class Hand;
class Order;
class OrdersList;

class Player //Player class represents a player playing the game
{
    public:
        Player(); //Default Constructor
        Player(string name); //Parameterized Constructor
        Player(const Player& player); //Copy Constructor
        ~Player(); //Destructor
        Player& operator = (const Player& player); //Assignment Operator Overloading
        bool operator == (const Player& player); //Equals Operator Overloading
        bool operator != (const Player& player); //Not Equals Operator Overloading
        friend ostream& operator << (ostream &output, const Player &player); //Stream Insertion Operator Overloading

        list<Territory*> toDefend(); //Returns list of territories to defend
        list<Territory*> toAttack(); //Returns list of territories to attack
        // bool hasOrdersToIssue();
        bool issueOrder(Deck* deck);
        void issueOrder(Order* order); //Issues an order

		void addTerritory(Territory* territory); //Adds a territory to the player's list of territories

        //Mutator Methods (Setters)
        void setName(string newName);
        void setArmies(int armyNumber);
        void setTerritories(list<Territory*> newTerritories); //Sets list of territories
        void setHand(Hand* newHand); //Sets hand
        void setOrdersList(OrdersList* newOrdersList); //Sets orders list

        //Accessor Methods (Getters)
		string getName(); //Returns name
        int getArmies();
        list<Territory*> getTerritories(); //Returns list of territories
        Hand* getHand(); //Returns hand
        OrdersList* getOrdersList(); //Returns orders list

    private:
        //Data Members
        string name;
        int armies;
        list<Territory*> territories;
        Hand* hand;
        OrdersList* ordersList;
};