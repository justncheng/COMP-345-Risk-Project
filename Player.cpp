#include "Player.h"

Player::Player() //Default Constructor
{
	name = "Unnamed Player"; //Sets player name
    armies = 0; //Sets armies
    hand = new Hand(); //Creates empty hand
    ordersList = new OrdersList(); //Creates empty orders list
}

Player::Player(string name) //Parameterized Constructor
{
    this->name = name; //Sets player name
    armies = 0; //Sets armies
    hand = new Hand(); //Creates empty hand
    ordersList = new OrdersList(); //Creates empty orders list
}

Player::Player(const Player& player) //Copy Constructor
{
	name = player.name; //Copies player name
    armies = player.armies; //Copies armies

    //Adds territories
    for(Territory* territory : player.territories)
    {
        territories.push_back(new Territory(*territory));
    }

    hand = new Hand(*player.hand); //Creates hand
    ordersList = new OrdersList(*player.ordersList); //Creates orders list
}

Player::~Player() //Destructor
{
    //Deletes each territory
    for(Territory* territory : territories)
    {
        delete territory;
    }

    delete hand; //Deletes hand
    delete ordersList; //Deletes orders list
}

Player& Player::operator = (const Player& player) //Assignment Operator Overloading
{
	name = player.name; //Copies player name
    armies = player.armies; //Copies armies

    //Checks if the player is not equal to itself
    if (this != &player) 
    {
        //Deletes exisiting territories
        for(Territory* territory : territories) 
        {
            delete territory;
        }

        territories.clear();

        //Adds new territories
        for(Territory* territory : player.territories)
        {
            territories.push_back(new Territory(*territory));
        }

        //Deletes hand and creates new hand
        delete hand;
        hand = new Hand(*player.hand);

        //Deletes orders list and creates new orders list
        delete ordersList;
        ordersList = new OrdersList(*player.ordersList);
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

    output << "\n" << player.name << "'s Territories: ";

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

    return output;
}

//Functions

list<Territory> Player::toDefend()
{
    return {Territory("TerritoryA", 30, 50, "ContinentA"), Territory("TerritoryB", 20, 40, "ContinentA")}; //Returns arbitrary list of territories
}

list<Territory> Player::toAttack()
{
    return {Territory("TerritoryA", 10, 30, "ContinentB"), Territory("TerritoryB", 30, 20, "ContinentB"), Territory("TerritoryC", 40, 50, "ContinentB")}; //Returns arbitrary list of territories
}

void Player::issueOrder(Order* order)
{
    ordersList->add(order); //Adds the order to the orders list
}

bool Player::issueOrder()
{
    // deploy phase and if player hasarmies, issue deploy orders first
    while(armies > 0) {     // while loop to deploy all armies
        list<Territory> defendList = toDefend(); // get territories to defend
        Territory* deployTerritory = nullptr;

        if(!defendList.empty()) {
            deployTerritory = &defendList.front(); // choose first territory to defend for deployment
        }
        else if (!territories.empty()) {
            deployTerritory = territories.front(); // if no territories to defend, deploy to any owned territory    
        }

        if(deployTerritory != nullptr) {
            int armiesToDeploy = armies; // deploy all remaining armies
            Order* deployOrder = new Deploy(this, deployTerritory, armiesToDeploy);
            issueOrder(deployOrder);  // issue deploy order
            armies = 0;  // all armies deployed
            return true;
        }
    }

    // after deployment, issue advance orders to defeend own territories or attack enemy territories
    list<Territory> defendList = toDefend();
    list<Territory> attackList = toAttack();

    // isuue advance orders to defend own territories
    if(!defendList.empty()) {
        Territory* source = &defendList.front();
        Territory* target = nullptr;

        // find another own territory to move armies to defend
        for (auto& territory : territories) {   // iterate through owned territories
            if(territory != source) {
                target = territory;             // choose first different territory as target
                break;
            }
        }

        if(target != nullptr) {
            int sourceArmies = source->getArmies();   // get armies in source territory
            int numArmies = (sourceArmies > 1) ? (sourceArmies - 1) : 0; // keeping 1 army behind
            Order* advanceOrder = new Advance(this, source, target, numArmies);
            issueOrder(advanceOrder);    // issuing advancde order
            return true;
        }
    }

    // issue advance orders to attack enemy territories from own territories
    if(!attackList.empty() && !defendList.empty()) {
        Territory* source = &defendList.front();
        Territory* enemyTarget = &attackList.front();
        int sourceArmies = source->getArmies();
        int numArmies = (sourceArmies > 1) ? (sourceArmies - 1) : 0; 

        Order* attackOrder = new Advance(this, source, enemyTarget, numArmies);
        issueOrder(attackOrder);   // issuing advance order to attack
        return true;
    }

    // Issue card-based orders here if any cards exist in hand (already implemented in Cards.cpp)
    
    return false;
}


void Player::addTerritory(Territory* territory)
{
	territories.push_back(territory); //Adds the territory to the player's list of territories
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
    for (Territory* territory : territories) 
    {
        delete territory;
    }

    territories = newTerritories;
}

void Player::setHand(Hand* newHand)
{
    hand = newHand;
}

void Player::setOrdersList(OrdersList* newOrdersList)
{
    ordersList = newOrdersList;
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