#pragma once
#include "Orders.h"

using namespace std;

void testOrderLists() {

	// Create dummy players and territories for testing
	Player* player1 = new Player();
	Player* player2 = new Player();

	Territory* territoryA = new Territory("TerritoryA", player1);
	Territory* territoryB = new Territory("TerritoryB", player2);

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

	std::cout << "\nOrders List after executing some orders: " << list << std::endl;

	// Move an order
	list.move(1, 4); // Move Advance to index 4
	std::cout << "\nOrders List after moving an order: " << list << std::endl;

	// Remove an order
	list.remove(1); // Remove the order now at index 1
	std::cout << "\nOrders List after removing an order: " << list << std::endl;

	// Copy and assignment
	OrdersList copy(list); // Copy constructor
	std::cout << "\nCopied Orders List: " << copy << std::endl;

	OrdersList assigned;
	assigned = list; // Assignment operator
	std::cout << "\nAssigned Orders List: " << assigned << std::endl;

	// Clean up
	delete player1;
	delete player2;
	delete territoryA;
	delete territoryB;
}