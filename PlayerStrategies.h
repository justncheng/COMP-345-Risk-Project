#pragma once

#include <iostream>
#include <list>

using namespace std;

//Forward declarations to avoid circular dependency
class Territory;
class Player;
class Order;
class Deck;

class PlayerStrategy
{
	public:
		PlayerStrategy(Player* player); //Parameterized Constructor
		PlayerStrategy(const PlayerStrategy& playerStrategy); //Copy Constructor
		~PlayerStrategy() = default; //Destructor
		PlayerStrategy& operator = (const PlayerStrategy& playerStrategy); //Assignment Operator Overloading
		friend ostream& operator << (ostream& output, const PlayerStrategy& playerStrategy); //Stream Insertion Operator Overloading
		virtual PlayerStrategy* clone() = 0; //Pure virtual clone function

		virtual bool issueOrder(Deck* deck) = 0; //Returns if an order was issued
		virtual list<Territory*> toDefend() = 0; //Returns a list of territories to defend
		virtual list<Territory*> toAttack() = 0; //Returns a list of territories to attack

		//Mutator Methods (Setters)
		void setPlayer(Player* player);

		//Accessor Methods (Getters)
		Player* getPlayer(); //Returns player
		virtual string getStrategyString() const; //Returns the strategy as a string

	protected:
		//Data member
		Player* player;
};

class HumanPlayerStrategy : public PlayerStrategy
{
	public:
		HumanPlayerStrategy(Player* player); //Parameterized Constructor
		HumanPlayerStrategy(const HumanPlayerStrategy& playerStrategy); //Copy Constructor
		~HumanPlayerStrategy() = default; //Destructor
		HumanPlayerStrategy& operator = (const HumanPlayerStrategy& playerStrategy); //Assignment Operator Overloading
		friend ostream& operator << (ostream& output, const HumanPlayerStrategy& playerStrategy); //Stream Insertion Operator Overloading
		HumanPlayerStrategy* clone() override; //Clone function

		bool issueOrder(Deck* deck) override; //Returns if an order was issued
		list<Territory*> toDefend() override; //Returns a list of territories to defend
		list<Territory*> toAttack() override; //Returns a list of territories to attack

		string getStrategyString() const override; //Returns the strategy as a string
};

class AggressivePlayerStrategy : public PlayerStrategy
{
	public:
		AggressivePlayerStrategy(Player* player); //Parameterized Constructor
		AggressivePlayerStrategy(const AggressivePlayerStrategy& playerStrategy); //Copy Constructor
		~AggressivePlayerStrategy() = default; //Destructor
		AggressivePlayerStrategy& operator = (const AggressivePlayerStrategy& playerStrategy); //Assignment Operator Overloading
		friend ostream& operator << (ostream& output, const AggressivePlayerStrategy& playerStrategy); //Stream Insertion Operator Overloading
		AggressivePlayerStrategy* clone() override; //Clone function

		bool issueOrder(Deck* deck) override; //Returns if an order was issued
		list<Territory*> toDefend() override; //Returns a list of territories to defend
		list<Territory*> toAttack() override; //Returns a list of territories to attack

		string getStrategyString() const override; //Returns the strategy as a string
};

class BenevolentPlayerStrategy : public PlayerStrategy
{
	public:
		BenevolentPlayerStrategy(Player* player); //Parameterized Constructor
		BenevolentPlayerStrategy(const BenevolentPlayerStrategy& playerStrategy); //Copy Constructor
		~BenevolentPlayerStrategy() = default; //Destructor
		BenevolentPlayerStrategy& operator = (const BenevolentPlayerStrategy& playerStrategy); //Assignment Operator Overloading
		friend ostream& operator << (ostream& output, const BenevolentPlayerStrategy& playerStrategy); //Stream Insertion Operator Overloading
		BenevolentPlayerStrategy* clone() override; //Clone function

		bool issueOrder(Deck* deck) override; //Returns if an order was issued
		list<Territory*> toDefend() override; //Returns a list of territories to defend
		list<Territory*> toAttack() override; //Returns a list of territories to advance towards

		string getStrategyString() const override; //Returns the strategy as a string
};

class NeutralPlayerStrategy : public PlayerStrategy
{
	public:
		NeutralPlayerStrategy(Player* player); //Parameterized Constructor
		NeutralPlayerStrategy(const NeutralPlayerStrategy& playerStrategy); //Copy Constructor
		~NeutralPlayerStrategy() = default; //Destructor
		NeutralPlayerStrategy& operator = (const NeutralPlayerStrategy& playerStrategy); //Assignment Operator Overloading
		friend ostream& operator << (ostream& output, const NeutralPlayerStrategy& playerStrategy); //Stream Insertion Operator Overloading
		NeutralPlayerStrategy* clone() override; //Clone function

		bool issueOrder(Deck* deck) override; //Returns if an order was issued
		list<Territory*> toDefend() override; //Returns a list of territories to defend
		list<Territory*> toAttack() override; //Returns a list of territories to attack

		string getStrategyString() const override; //Returns the strategy as a string
};

class CheaterPlayerStrategy : public PlayerStrategy
{
	public:
		CheaterPlayerStrategy(Player* player); //Parameterized Constructor
		CheaterPlayerStrategy(const CheaterPlayerStrategy& playerStrategy); //Copy Constructor
		~CheaterPlayerStrategy() = default; //Destructor
		CheaterPlayerStrategy& operator = (const CheaterPlayerStrategy& playerStrategy); //Assignment Operator Overloading
		friend ostream& operator << (ostream& output, const CheaterPlayerStrategy& playerStrategy); //Stream Insertion Operator Overloading
		CheaterPlayerStrategy* clone() override; //Clone function

		bool issueOrder(Deck* deck) override; //Returns if an order was issued
		list<Territory*> toDefend() override; //Returns a list of territories to defend
		list<Territory*> toAttack() override; //Returns a list of territories to attack

		string getStrategyString() const override; //Returns the strategy as a string
};