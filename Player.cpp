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
        territories.push_back(territory);
    }

    hand = new Hand(*player.hand); //Creates hand
    ordersList = new OrdersList(*player.ordersList); //Creates orders list
}

Player::~Player() //Destructor
{
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

list<Territory*> Player::toDefend()
{
    srand(time(NULL));

    int territoriesToDefend = rand() % 2 + 2;

    if(territoriesToDefend > territories.size())
    {
        territoriesToDefend = territories.size();
	}

    list<Territory*> defendList;

    while (territoriesToDefend > 0)
    {
		int territoryToDefendIndex = rand() % territories.size();

        for(Territory* territory : territories)
        {
            if(territoryToDefendIndex == 0)
            {
				bool alreadyInList = false;

                for(Territory* defendedTerritory : defendList)
                {
                    if(defendedTerritory == territory)
                    {
                        territoryToDefendIndex++;
                        territoriesToDefend++;
						alreadyInList = true;
                        break;
                    }
				}

                if (!alreadyInList)
                {
                    defendList.push_back(territory);
                }

                break;
            }

            territoryToDefendIndex--;
		}

        territoriesToDefend--;
    }

	return defendList;
}

list<Territory*> Player::toAttack()
{
    srand(time(NULL));

    int territoriesToAttack = rand() % 2 + 2;

    if (territoriesToAttack > territories.size())
    {
        territoriesToAttack = territories.size();
    }

    list<Territory*> attackList;

    while (territoriesToAttack > 0)
    {
        int territoryNeighbourToAttackIndex = rand() % territories.size();

        for (Territory* territory : territories)
        {
            if (territoryNeighbourToAttackIndex == 0)
            {
                bool invalid = false;

                int territoryToAttackIndex = rand() % territory->getAdjacentTerritories().size();

                for (Territory* toAttack : territory->getAdjacentTerritories())
                {
                    if (territoryToAttackIndex == 0)
                    {
                        invalid = false;

                        if (toAttack->getOwner() == this)
                        {
                            invalid = true;
                        }

                        for (Territory* attackedTerritory : attackList)
                        {
                            if (attackedTerritory == toAttack)
                            {
                                territoryNeighbourToAttackIndex++;
                                territoriesToAttack++;
                                invalid = true;
                                break;
                            }
                        }

                        if (!invalid)
                        {
                            attackList.push_back(territory);
                        }

                        break;
                    }

                    territoryToAttackIndex--;
                }

                if (invalid)
                {
                    break;
                }
            }

            territoryNeighbourToAttackIndex--;
        }

        territoriesToAttack--;
    }

    return attackList;
}

void Player::issueOrder(Order* order)
{
    ordersList->add(order); //Adds the order to the orders list
}

bool Player::issueOrder(Deck* deck)
{
    // deploy phase and if player has armies, issue deploy orders first
    if (armies > 0)
    {
        list<Territory*> defendList = toDefend(); // get territories to defend
		int amountToDefend = defendList.size();

        if (defendList.empty())
        {
            return false;
        }

        Territory* deployTerritory = nullptr;

        while (!defendList.empty())
        {
            deployTerritory = defendList.front(); // choose first territory to defend for deployment

	        int armiesToDeploy = armies / defendList.size(); // deploy armies evenly among territories to defend
            Order* deployOrder = new Deploy(this, deployTerritory, armiesToDeploy);
            issueOrder(deployOrder);  // issue deploy order
			armies -= armiesToDeploy; // reduce available armies

			defendList.pop_front(); // remove deployed territory from list
        }

        return true;
    }

    // after deployment, issue advance orders to defend own territories or attack enemy territories
    list<Territory*> defendList = toDefend();
    list<Territory*> attackList = toAttack();

    // issue advance orders to defend own territories
    if (!defendList.empty()) {
        Territory* source = nullptr;
        Territory* target = defendList.front();

        // find another own territory to move armies to defend
        for (auto& territory : territories) {   // iterate through owned territories
            if (territory != target && territory->getArmies() > 1) {
                bool validSource = true;

                for (Order* order : ordersList->getOrders()) {
                    if (order->getName() == "Advance") {
                        if(dynamic_cast<Advance*>(order)->getSource() == territory || dynamic_cast<Advance*>(order)->getTarget() == territory) {
							validSource = false;
                            break;
						}
					}
                }

                if (validSource)
                {
                    source = territory; // choose first different territory with more than one army as source
                    break;
                }
            }
        }

        if(source != nullptr) {
            int sourceArmies = source->getArmies();   // get armies in source territory

            if (sourceArmies > 1) {
                int numArmies = (sourceArmies > 1) ? (sourceArmies - 1) : 0; // keeping 1 army behind
                Order* advanceOrder = new Advance(this, source, target, numArmies);
                issueOrder(advanceOrder);    // issuing advancde order
                return true;
            }
        }
    }

    // issue advance orders to attack enemy territories from own territories
    if(!attackList.empty() && !defendList.empty()) {
        Territory* source = defendList.front();
        Territory* enemyTarget = attackList.front();
        int sourceArmies = source->getArmies();

        if (sourceArmies > 1) {
            int numArmies = (sourceArmies > 1) ? (sourceArmies - 1) : 0;

            Order* attackOrder = new Advance(this, source, enemyTarget, numArmies);
            issueOrder(attackOrder);   // issuing advance order to attack
            return true;
        }
    }

    // Issue card-based orders here if any cards exist in hand

    for (const auto& card : hand->getCards())
    {
        CardPlayContext& context = CardPlayContext();
        context.target = toDefend().front();
        context.armies = 1;

        card->play(this, deck, hand, context);
    }

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