#pragma once

#include <iostream>
#include <list>

#include "Map.h"
#include "Orders.h"
#include "Cards.h"
using namespace std;

class Player
{
    public:
        Player();
        Player(const Player& player);
        Player& operator=(const Player& player);

        list<Territory> toDefend();
        list<Territory> toAttack();
        void issueOrder();
    private:
        list<Territory*> territories;
        Hand* hand;
        OrdersList* orderList;
};