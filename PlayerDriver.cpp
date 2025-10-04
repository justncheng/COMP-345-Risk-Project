#include "Player.h"

void testPlayers() //Free function that tests the Player class's functionality
{
    Player* player1 = new Player(); //Creates new Player
    player1->getHand()->addCard(make_unique<Card>(CardType::Bomb)); //Adds a card to the player's hand
    player1->getHand()->addCard(make_unique<Card>(CardType::Reinforcement)); //Adds a card to the player's hand

    cout << "\nPlayer Data\n-------------------------------------------------------------------\n";
    cout << *player1 << endl;

    cout << "\nPlayer toDefend() method\n-------------------------------------------------------------------\n";
    
    list<Territory> toDefendTerritories = player1->toDefend(); //Tests the toDefend() method

    //Prints the list of territories
    for(Territory territory : toDefendTerritories)
    {
        cout << territory;

        if(toDefendTerritories.back() != territory)
        {
            cout << ", ";
        }
    }

    cout << "\n\nPlayer toAttack() method\n-------------------------------------------------------------------\n";
    
    list<Territory> toAttackTerritories = player1->toAttack(); //Tests the toAttack() method

    //Prints the list of territories
    for(Territory territory : toAttackTerritories)
    {
        cout << territory;

        if(toAttackTerritories.back() != territory)
        {
            cout << ", ";
        }
    }

    cout << "\n\nPlayer issueOrder() method\n-------------------------------------------------------------------\n";

    cout << "OrdersList before issueOrder(): " << *(player1->getOrdersList()) << "\n"; //Prints the before issueOrder() orders list for comparison
    
	Deploy d(player1, nullptr, 5); //Creates a Deploy order
    player1->issueOrder(d); //Tests the issueOrder() method

    cout << "OrdersList after issueOrder(): " << *(player1->getOrdersList()) << "\n"; //Prints the after issueOrder() orders list for comparison

    //Deletes the player
    delete player1;
}