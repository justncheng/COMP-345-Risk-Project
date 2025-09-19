#include "Player.h"

Player::Player()
{
    hand = new Hand();
    orderList = new OrdersList();
}

Player::Player(const Player& player)
{
    for(Territory* territory : player.territories)
    {
        territories.push_back(new Territory(*territory));
    }

    hand = new Hand(*player.hand);
    orderList = new OrdersList(*player.orderList);
}

Player& Player::operator=(const Player& player)
{
    if (this != &player) {
        for (Territory* territory : territories) 
        {
            delete territory;
        }

        territories.clear();

        for (Territory* territory : player.territories)
        {
            territories.push_back(new Territory(*territory));
        }

        delete hand;
        hand = new Hand(*player.hand);

        delete orderList;
        orderList = new OrdersList(*player.orderList);
    }

    return *this;
}

list<Territory> Player::toDefend()
{
    return {Territory()};
}

list<Territory> Player::toAttack()
{
    return {Territory()};
}

void Player::issueOrder()
{

}