#pragma once

#include <iostream>
#include <list>
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include "PlayerStrategies.h"

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
        Player(string name, PlayerStrategy* playerStrategy); //Parameterized Constructor
        Player(const Player& player); //Copy Constructor
        ~Player(); //Destructor
        Player& operator = (const Player& player); //Assignment Operator Overloading
        bool operator == (const Player& player); //Equals Operator Overloading
        bool operator != (const Player& player); //Not Equals Operator Overloading
        friend ostream& operator << (ostream &output, const Player &player); //Stream Insertion Operator Overloading

        list<Territory*> toDefend(); //Returns list of territories to defend
        list<Territory*> toAttack(); //Returns list of territories to attack
        bool issueOrder(Deck* deck); //Returns if an order was issued
        void issueOrder(Order* order); //Issues an order

		void addTerritory(Territory* territory); //Adds a territory to the player's list of territories
        void removeTerritory(Territory* territory); //Removes a territory from the player's list of territories

        //Mutator Methods (Setters)
        void setName(string newName);
        void setArmies(int armyNumber);
        void setTerritories(list<Territory*> newTerritories); //Sets list of territories
        void setHand(Hand* newHand); //Sets hand
        void setOrdersList(OrdersList* newOrdersList); //Sets orders list
        void setPlayerStrategy(PlayerStrategy* newStrategy); //Sets strategy

        //Accessor Methods (Getters)
		string getName(); //Returns name
        int getArmies();
        list<Territory*> getTerritories(); //Returns list of territories
        Hand* getHand(); //Returns hand
        OrdersList* getOrdersList(); //Returns orders list
        PlayerStrategy* getPlayerStrategy(); //Returns player strategy

        //For tracking truces
        void addNegotiatedPlayer(Player* other);
        bool hasNegotiatedWith(Player* other) const;
        void clearNegotiatedPlayers();

    private:
        //Data Members
        string name;
        int armies;
        list<Territory*> territories;
        Hand* hand;
        OrdersList* ordersList;
        std::vector<Player*> negotiatedPlayers;
        PlayerStrategy* playerStrategy;
};