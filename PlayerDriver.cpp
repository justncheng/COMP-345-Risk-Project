#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

void testPlayers() //Free function that tests the Player class's functionality
{
    cout << "=== Players Testing ===\n\n";

    Player* player1 = new Player(); //Creates new Player
	list<Territory*> testTerritories = { new Territory("TerritoryA", 10, 30, "ContinentB"), new Territory("TerritoryB", 30, 20, "ContinentB") }; //Creates list of territories
	player1->setTerritories(testTerritories); //Adds territories to the player
    player1->getHand()->addCard(make_unique<Card>(CardType::Bomb)); //Adds a card to the player's hand
    player1->getHand()->addCard(make_unique<Card>(CardType::Reinforcement)); //Adds a card to the player's hand

    cout << "Player Data\n----------------------------\n";
    cout << *player1 << endl;

    cout << "Player toDefend() method\n----------------------------\n";
    
    list<Territory*> toDefendTerritories = player1->toDefend(); //Tests the toDefend() method

    //Prints the list of territories
    for(const Territory* territory : toDefendTerritories)
    {
        cout << territory;

        if(toDefendTerritories.back() != territory)
        {
            cout << ", ";
        }
    }

    cout << "\n\nPlayer toAttack() method\n----------------------------\n";
    
    list<Territory*> toAttackTerritories = player1->toAttack(); //Tests the toAttack() method

    //Prints the list of territories
    for(const Territory* territory : toAttackTerritories)
    {
        cout << territory;

        if(toAttackTerritories.back() != territory)
        {
            cout << ", ";
        }
    }

    cout << "\n\nPlayer issueOrder() method\n----------------------------\n";

    cout << "OrdersList before issueOrder(): " << *(player1->getOrdersList()); //Prints the before issueOrder() orders list for comparison
    
	//  Deploy d(player1, nullptr, 5); //Creates a Deploy order
    player1->issueOrder(new Deploy(player1, nullptr, 5)); //Tests the issueOrder() method

    cout << "OrdersList after issueOrder(): " << *(player1->getOrdersList()) << "\n"; //Prints the after issueOrder() orders list for comparison

    //Deletes the player
    delete player1;

    for(Territory* territory : testTerritories)
    {
        delete territory;
	}

    cout << "\n=== Players Testing Complete ===\n\n";
}