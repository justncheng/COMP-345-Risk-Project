#include "Player.h"

Player::Player() //Default Constructor
{
	name = "Unnamed Player"; //Sets player name
    armies = 0; //Sets armies
    hand = new Hand(); //Creates empty hand
    ordersList = new OrdersList(); //Creates empty orders list
    playerStrategy = new HumanPlayerStrategy(this); //Create default player strategy
}

Player::Player(string name, PlayerStrategy* playerStrategy) //Parameterized Constructor
{
    this->name = name; //Sets player name
    armies = 0; //Sets armies
    hand = new Hand(); //Creates empty hand
    ordersList = new OrdersList(); //Creates empty orders list
    this->playerStrategy = playerStrategy; //Sets player strategy
	playerStrategy->setPlayer(this); //Sets the player for the strategy
}

Player::Player(const Player& player) //Copy Constructor
{
	name = player.name; //Copies player name
    armies = player.armies; //Copies armies

    //Adds territories
    for(Territory* territory : player.territories)
    {
        territories.push_back(territory);
    }

    hand = new Hand(*player.hand); //Creates hand
    ordersList = new OrdersList(*player.ordersList); //Creates orders list
    playerStrategy = player.playerStrategy ? player.playerStrategy->clone() : nullptr; //Creates player strategy
}

Player::~Player() //Destructor
{
    delete hand; //Deletes hand
    delete ordersList; //Deletes orders list
    delete playerStrategy; //Deletes player strategy
}

Player& Player::operator = (const Player& player) //Assignment Operator Overloading
{
	name = player.name; //Copies player name
    armies = player.armies; //Copies armies

    //Checks if the player is not equal to itself
    if (this != &player) 
    {
        territories.clear();

        //Adds new territories
        for(Territory* territory : player.territories)
        {
            territories.push_back(territory);
        }

        //Deletes hand and creates new hand
        delete hand;
        hand = new Hand(*player.hand);

        //Deletes orders list and creates new orders list
        delete ordersList;
        ordersList = new OrdersList(*player.ordersList);

        //Deletes player strategy and creates new player strategy
        delete playerStrategy;
        playerStrategy = player.playerStrategy ? player.playerStrategy->clone() : nullptr;
    }

    return *this;
}

bool Player::operator == (const Player& player) //Equals Operator Overloading
{
     //Checks if the lists have equal size
    if(territories.size() != player.territories.size())
    {
        return false;
    }

    //Iterators for each list
    auto iter1 = territories.begin();
    auto iter2 = player.territories.begin();

    //Iterates through the lists
    for(; iter1 != territories.end() && iter2 != territories.end(); ++iter1, ++iter2)
    {
        //Checks if each element of the lists are equal to the corresponding element of the other list
        if(**iter1 != **iter2)
        {
            return false;
        }
    }

    return (name == player.name) && (armies == player.armies) && (*hand == *player.hand) && (*ordersList == *player.ordersList); //Checks if the names, hands, and orders lists are equal
}

bool Player::operator != (const Player& player) //Not Equals Operator Overloading
{
    return !(*this == player);
}

ostream& operator << (ostream &output, const Player &player) //Stream Insertion Operator Overloading
{
    //Outputs the player's information

    output << player.name << "'s Territories: ";

    //Outputs the player's list of territories as a comma-separated list
    for(Territory* territory : player.territories)
    {
        output << *(territory);

        if(player.territories.back() != territory)
        {
            output << ", ";
        }
    }
 
    output << "\n" + player.name + "'s Hand: " << *(player.hand); //Outputs the player's hand
    output << "\n" + player.name + "'s Orders List: " << *(player.ordersList); //Outputs the player's orders list
    output << player.name << "'s armies: " << player.armies << "\n"; //Outputs the player's armies
    output << player.name << "'s " << *(player.playerStrategy) << "\n"; //Outputs the player's strategy

    return output;
}

//Functions

list<Territory*> Player::toDefend()
{
    return playerStrategy->toDefend();
}

list<Territory*> Player::toAttack()
{
    return playerStrategy->toAttack();
}

bool Player::issueOrder(Deck* deck)
{
    return playerStrategy->issueOrder(deck);
}

void Player::issueOrder(Order* order)
{
    ordersList->add(order); //Adds the order to the orders list
}

void Player::addTerritory(Territory* territory)
{
	territories.push_back(territory); //Adds the territory to the player's list of territories
}

void Player::removeTerritory(Territory* territory)
{
    territories.remove(territory); //Removes the territory from the player's list of territories
}

//Mutator Methods (Setters)

void Player::setName(string newName)
{
    name = newName;
}

void Player::setArmies(int armyNumber)
{
    armies = armyNumber;
}

void Player::setTerritories(list<Territory*> newTerritories)
{
    territories.clear();
    territories = newTerritories;
}

void Player::setHand(Hand* newHand)
{
    delete hand;
    hand = newHand;
}

void Player::setOrdersList(OrdersList* newOrdersList)
{
    delete ordersList;
    ordersList = newOrdersList;
}

void Player::setPlayerStrategy(PlayerStrategy* newStrategy)
{
    delete playerStrategy;
    playerStrategy = newStrategy;
}

//Accessor Methods (Getters)

string Player::getName()
{
    return name;
}

int Player::getArmies()
{
    return armies;
}

list<Territory*> Player::getTerritories()
{
    return territories;
}

Hand* Player::getHand()
{
    return hand;
}

OrdersList* Player::getOrdersList()
{
    return ordersList;
}

PlayerStrategy* Player::getPlayerStrategy()
{
    return playerStrategy;
}

void Player::addNegotiatedPlayer(Player* other) {
    if (!other || other == this) return;
    // avoid duplicates
    if (std::find(negotiatedPlayers.begin(), negotiatedPlayers.end(), other) == negotiatedPlayers.end()) {
        negotiatedPlayers.push_back(other);
    }
}

bool Player::hasNegotiatedWith(Player* other) const {
    if (!other) return false;
    return std::find(negotiatedPlayers.begin(), negotiatedPlayers.end(), other) != negotiatedPlayers.end();
}

void Player::clearNegotiatedPlayers() {
    negotiatedPlayers.clear();
}