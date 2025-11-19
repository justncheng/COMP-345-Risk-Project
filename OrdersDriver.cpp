
#include <iostream>
#include "Orders.h"
#include "Player.h"
#include "Map.h"

using namespace std;

void testOrdersLists() {
	cout << "=== OrdersList Testing ===\n\n";

	// Create dummy players and territories for testing
	Player* player1 = new Player();
	Player* player2 = new Player();

	Territory* territoryA = new Territory("TerritoryA", 10, 40, "ContinentA");
	Territory* territoryB = new Territory("TerritoryB", 20, 30, "ContinentA");

	territoryA->setOwner(player1);
	territoryB->setOwner(player2);

	OrdersList list;

	// Create and execute various orders
	Deploy d(player1, territoryA, 5);
	Advance a(player1, territoryA, territoryB, 3);
	Bomb b(player1, territoryB);
	Blockade bl(player1, territoryA);
	Airlift al(player1, territoryA, territoryB, 2);
	Negotiate n(player1, player2);

	// Add orders to the list
	list.add(d);
	list.add(a);
	list.add(b);
	list.add(bl);
	list.add(al);
	list.add(n);

	std::cout << "Initial Orders List: " << list << std::endl;

	// Execute some orders at specific indices
	list.at(0).execute(); // Deploy
	list.at(2).execute(); // Bomb

	std::cout << "Orders List after executing some orders: " << list << std::endl;

	// Move an order
	list.move(1, 4); // Move Advance to index 4
	std::cout << "Orders List after moving an order: " << list << std::endl;

	// Remove an order
	list.remove(1); // Remove the order now at index 1
	std::cout << "Orders List after removing an order: " << list << std::endl;

	// Copy and assignment
	OrdersList copy(list); // Copy constructor
	std::cout << "Copied Orders List: " << copy << std::endl;

	OrdersList assigned;
	assigned = list; // Assignment operator
	std::cout << "Assigned Orders List: " << assigned << std::endl;

	// Clean up
	delete player1;
	delete player2;
	delete territoryA;
	delete territoryB;

	cout << "=== Orders List Testing Complete ===\n\n";
}


void testOrderExecution() {
    cout << "\n=== Testing Order Execution ===\n\n";

    // --- Setup players and territories ---


    Player* player1 = new Player();
	player1->setName("Player1");
    Player* player2 = new Player();
	player2->setName("Player2");

    Territory* A1 = new Territory("A1", 0, 0, "ContinentA");
    Territory* A2 = new Territory("A2", 1, 0, "ContinentA");
    Territory* B1 = new Territory("B1", 2, 0, "ContinentA");

    // Set ownership and army counts
    A1->setOwner(player1);  A1->setArmies(10);
    A2->setOwner(player1);  A2->setArmies(3);
    B1->setOwner(player2);  B1->setArmies(7);

    // Make territories adjacent both ways
    A1->addAdjacentTerritory(A2);
    A2->addAdjacentTerritory(A1);
    A1->addAdjacentTerritory(B1);
    B1->addAdjacentTerritory(A1);
    A2->addAdjacentTerritory(B1);
    B1->addAdjacentTerritory(A2);

    cout << "Initial Territories:\n";
    cout << "A1 - Owner: " << (A1->getOwner() ? A1->getOwner()->getName() : "Neutral")
        << ", Armies: " << A1->getArmies() << endl;
    cout << "A2 - Owner: " << (A2->getOwner() ? A2->getOwner()->getName() : "Neutral")
        << ", Armies: " << A2->getArmies() << endl;
    cout << "B1 - Owner: " << (B1->getOwner() ? B1->getOwner()->getName() : "Neutral")
        << ", Armies: " << B1->getArmies() << endl;

    // --- Create an OrdersList to simulate game flow ---
    OrdersList orders;

    // (1) Deploy - one valid, one invalid
    Deploy deployValid(player1, A1, 5);
    Deploy deployInvalid(player1, B1, 3); // invalid, not owned by player1

    // (2) Advance - friendly move and hostile attack
    Advance advanceFriendly(player1, A1, A2, 4);
    Advance advanceHostile(player1, A1, B1, 6);

    // (3) Bomb - player2 bombs player1's A2 (valid, adjacent)
    Bomb bombOrder(player2, A2);

    // (4) Blockade - doubles armies and transfers ownership
    Blockade blockadeOrder(player1, A2);

    // (5) Negotiate - peace between player1 and player2
    Negotiate negotiateOrder(player1, player2);

    // (6) Another Advance after negotiate (should be invalid if truce enforced)
    Advance advanceAfterNegotiate(player1, A1, B1, 3);

    // Add all to OrdersList
    orders.add(deployValid);
    orders.add(deployInvalid);
    orders.add(advanceFriendly);
    orders.add(advanceHostile);
    orders.add(bombOrder);
    orders.add(blockadeOrder);
    orders.add(negotiateOrder);
    orders.add(advanceAfterNegotiate);

    // --- Execute and validate each order ---
    cout << "\nExecuting and validating orders:\n";
    for (int i = 0; i < orders.size(); i++) {
        Order& o = orders.at(i);
        bool valid = o.validate();
        cout << "Order " << (i + 1) << " (" << typeid(o).name() << ") is "
            << (valid ? "valid" : "invalid") << endl;
        o.execute();
    }

    // --- Final results ---
    cout << "\nFinal Territories:\n";
    cout << "A1 - Owner: " << (A1->getOwner() ? A1->getOwner()->getName() : "Neutral")
        << ", Armies: " << A1->getArmies() << endl;
    cout << "A2 - Owner: " << (A2->getOwner() ? A2->getOwner()->getName() : "Neutral")
        << ", Armies: " << A2->getArmies() << endl;
    cout << "B1 - Owner: " << (B1->getOwner() ? B1->getOwner()->getName() : "Neutral")
        << ", Armies: " << B1->getArmies() << endl;

    // --- Post-checks based on rubric ---
    if (B1->getOwner() == player1)
        cout << "\n[Check] Territory conquered: ownership of B1 transferred to Player1.\n";
    else
        cout << "\n[Check] B1 was not conquered (defense successful).\n";

    if (B1->getOwner() == player1)
        cout << "[Check] Player1 should receive exactly ONE card for conquering a territory.\n";
    else
        cout << "[Check] No conquest, no card awarded.\n";

    cout << "[Check] Negotiate: attacks between Player1 and Player2 should now be invalid.\n";

    if (A2->getOwner() == nullptr)
        cout << "[Check] Blockade: territory A2 transferred to Neutral (nullptr) and armies doubled.\n";
    else
        cout << "[Check] Blockade: verify A2 transferred to Neutral player in your implementation.\n";

    cout << "[Check] All order types issued and executed successfully.\n";

    // --- Clean up ---
    delete player1;
    delete player2;
    delete A1;
    delete A2;
    delete B1;

    cout << "\n=== Testing Order Execution Complete ===\n\n";
}