#pragma once

#include <string>
#include <vector>
#include <iostream>
//#include "GameEngine.h"
#include "Player.h"
#include "Map.h"

using namespace std;

// Forward declarations -------------------------------------------------------------------------------------

class Card;
class Player;
class Territory;

// Orders Class ---------------------------------------------------------------------------------------------

/*
Base Order class representing a generic order in the game.
- validate() and execute() are virtual functions to be implemented by derived classes.
- clone() is a virtual function to create a copy of the order.
*/


class Order
{
public:
	Order(); // Default constructor
	Order(const std::string& name); // Parameterized constructor
	Order(const Order& other); // Copy constructor
	Order& operator=(const Order& other); // Assignment operator
	virtual ~Order(); // Destructor


	virtual bool validate() const = 0;	// Verify if order is valid
	virtual void execute() = 0;	// Execute the order
	
	
	virtual Order* clone() const = 0; // Virtual constructor
	virtual std::string toString() const;
	
	bool isExecuted() const; // Verify if order has been executed

	friend std::ostream& operator<<(std::ostream& os, const Order& order); // Stream insertion operator

protected:
	void setEffect(const std::string& effect); // Set the effect description of the order
	void setName(const std::string& name); // Set the name of the order

private:
	std::string name;
	bool executed;
	std::string effect;
};

// DERIVED ORDER CLASSES ----------------------------------------------------------------------------------------

// Deploy Class -------------------------------------------------------------------------------------------------

class Deploy : public Order
{
public:
	Deploy(); // Default constructor
	Deploy(Player* issuer, Territory* target, int armies); // Parameterized constructor
	Deploy(const Deploy& other); // Copy constructor
	Deploy& operator=(const Deploy& other); // Assignment operator
	~Deploy(); // Destructor

	bool validate() const override; 
	void execute() override;

	Order* clone() const override;
	std::string toString() const override;

	friend std::ostream& operator<<(std::ostream& os, const Deploy& order); // Stream insertion operator

private:
	Player* issuer; // Player issuing the order
	Territory* target; // Target territory
	int armies; // Number of armies to deploy
};

// Advance Class ------------------------------------------------------------------------------------------------

class Advance : public Order
{
public:
	Advance(); // Default constructor
	Advance(Player* issuer, Territory* source, Territory* target, int armies);// Parameterized constructor
	Advance(const Advance& other); // Copy constructor
	Advance& operator=(const Advance& other); // Assignment operator
	~Advance(); // Destructor

	bool validate() const override;
	void execute() override;

	Order* clone() const override;
	std::string toString() const override;

	friend std::ostream& operator<<(std::ostream& os, const Advance& order); // Stream insertion operator

private:
	Player* issuer; // Player issuing the order
	Territory* source; // Source territory
	Territory* target; // Target territory
	int armies; // Number of armies to move/attack with
};

// Bomb Class ---------------------------------------------------------------------------------------------------

class Bomb : public Order
{
public:
	Bomb(); // Default constructor
	Bomb(Player* issuer, Territory* target); // Parameterized constructor
	Bomb(const Bomb& other); // Copy constructor
	Bomb& operator=(const Bomb& other); // Assignment operator
	~Bomb(); // Destructor

	bool validate() const override;
	void execute() override;

	Order* clone() const override;
	std::string toString() const override;

	friend std::ostream& operator<<(std::ostream& os, const Bomb& order); // Stream insertion operator

private:
	Player* issuer; // Player issuing the order
	Territory* target; // Target territory
};

// Blockade Class -----------------------------------------------------------------------------------------------

class Blockade : public Order
{
public:
	Blockade(); // Default constructor
	Blockade(Player* issuer, Territory* target); // Parameterized constructor
	Blockade(const Blockade& other); // Copy constructor
	Blockade& operator=(const Blockade& other); // Assignment operator
	~Blockade(); // Destructor

	bool validate() const override;
	void execute() override;

	Order* clone() const override;
	std::string toString() const override;

	friend std::ostream& operator<<(std::ostream& os, const Blockade& order); // Stream insertion operator

private:
	Player* issuer; // Player issuing the order
	Territory* target; // Target territory
};

// Airlift Class ------------------------------------------------------------------------------------------------

class Airlift : public Order
{
public:
	Airlift(); // Default constructor
	Airlift(Player* issuer, Territory* source, Territory* target, int armies); // Parameterized constructor
	Airlift(const Airlift& other); // Copy constructor
	Airlift& operator=(const Airlift& other); // Assignment operator
	~Airlift(); // Destructor

	bool validate() const override;
	void execute() override;

	Order* clone() const override;
	std::string toString() const override;

	friend std::ostream& operator<<(std::ostream& os, const Airlift& order); // Stream insertion operator

private:
	Player* issuer; // Player issuing the order
	Territory* source; // Source territory
	Territory* target; // Target territory
	int armies; // Number of armies to airlift
};

// Negotiate Class -----------------------------------------------------------------------------------------------

class Negotiate : public Order
{
public:
	Negotiate(); // Default constructor
	Negotiate(Player* issuer, Player* targetPlayer); // Parameterized constructor
	Negotiate(const Negotiate& other); // Copy constructor
	Negotiate& operator=(const Negotiate& other); // Assignment operator
	~Negotiate(); // Destructor

	bool validate() const override;
	void execute() override;

	Order* clone() const override;
	std::string toString() const override;

	friend std::ostream& operator<<(std::ostream& os, const Negotiate& order); // Stream insertion operator

private:
	Player* issuer; // Player issuing the order
	Player* targetPlayer; // Target player to negotiate with
};

// OrdersList Class ---------------------------------------------------------------------------------------------

class OrdersList
{
public:

	OrdersList(); // Default constructor
	OrdersList(const OrdersList& other); // Copy constructor
	OrdersList& operator=(const OrdersList& other); // Assignment operator
	~OrdersList(); // Destructor

	void add(const Order& order); // Add order to the list
	void add(Order* order); // Add order pointer to the list
	void remove(Order* order); // Remove order from the list (by pointer)
	void remove(int index);// Deletes order from the list (by index)
	void move(int fromIndex, int toIndex); // Move order within the list

	int size() const; // Get number of orders in the list
	const Order& at(int index) const; // Get order at specific index
	Order& at(int index); // Get order at specific index (non-const)

	bool operator==(const OrdersList& other) const;

	friend std::ostream& operator<<(std::ostream& os, const OrdersList& ordersList); // Stream insertion operator

	/*
	bool operator == (const OrdersList& ordersList) {return true;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
    friend ostream& operator << (ostream &output, const OrdersList& ordersList) {return output;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
    void add(Order order) {}; //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
	*/

private:
	void clear(); // Clear all orders from the list
	void deepCopy(const OrdersList& other); // Deep copy from another OrdersList

private:
	// Class members
	std::vector<Order*> orders{}; // Vector of pointers to orders
};

void testOrderLists(); // Function to test OrdersList class