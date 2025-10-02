#include "Player.h"

Player::Player() //Default Constructor
{
    hand = new Hand(); //Creates empty hand
    ordersList = new OrdersList(); //Creates empty orders list
}

Player::Player(const Player& player) //Copy Constructor
{
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

    return (*hand == *player.hand) && (*ordersList == *player.ordersList); //Checks if the hands and orders lists are equal
}

bool Player::operator != (const Player& player) //Not Equals Operator Overloading
{
    return !(*this == player);
}

ostream& operator << (ostream &output, const Player &player) //Stream Insertion Operator Overloading
{
    //Outputs the player's information

    output << "Player's Territories: ";

    //Outputs the player's list of territories as a comma-separated list
    for(Territory* territory : player.territories)
    {
        output << *(territory);

        if(player.territories.back() != territory)
        {
            output << ", ";
        }
    }
 
    output << "\nPlayer's Hand: " << *(player.hand); //Outputs the player's hand
    output << "\nPlayer's Orders List: " << *(player.ordersList); //Outputs the player's orders list

    return output;
}

//Functions

list<Territory> Player::toDefend()
{
    return {Territory(), Territory()}; //Returns arbitrary list of territories
}

list<Territory> Player::toAttack()
{
    return {Territory(), Territory(), Territory()}; //Returns arbitrary list of territories
}

void Player::issueOrder()
{
    Order order = Order(); //Creates an order
    ordersList->add(order); //Adds the order to the orders list
}

//Mutator Methods (Setters)

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