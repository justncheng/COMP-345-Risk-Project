#include "PlayerStrategies.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include "Player.h"

//PlayerStrategy class implementation

PlayerStrategy::PlayerStrategy(Player* player) //Parameterized Constructor
{
	this->player = player;
}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& playerStrategy) //Copy Constructor
{
	player = new Player(*playerStrategy.player);
}

PlayerStrategy& PlayerStrategy::operator = (const PlayerStrategy& playerStrategy) //Assignment Operator Overloading
{
	if (this != &playerStrategy)
	{
		delete player;
		player = new Player(*playerStrategy.player);
	}

	return *this;
}

ostream& operator << (ostream& output, const PlayerStrategy& playerStrategy) //Stream Insertion Operator Overloading
{
	output << "Strategy Type: AbstractPlayerStrategy\n";
	return output;
}

Player* PlayerStrategy::getPlayer()
{
	return player;
}

//HumanPlayerStrategy class implementation

HumanPlayerStrategy::HumanPlayerStrategy(Player* player) : PlayerStrategy(player) {} //Parameterized Constructor

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& playerStrategy) : PlayerStrategy(playerStrategy) {} //Copy Constructor

HumanPlayerStrategy& HumanPlayerStrategy::operator = (const HumanPlayerStrategy& playerStrategy) //Assignment Operator Overloading
{
	if (this != &playerStrategy)
	{
		PlayerStrategy::operator=(playerStrategy);
	}

	return *this;
}

ostream& operator << (ostream& output, const HumanPlayerStrategy& playerStrategy) //Stream Insertion Operator Overloading
{
	output << "Strategy Type: HumanPlayerStrategy\n";
	return output;
}

HumanPlayerStrategy* HumanPlayerStrategy::clone() //Clone function
{
	return new HumanPlayerStrategy(*this);
}

bool HumanPlayerStrategy::issueOrder(Deck* deck)
{
	return false;
}

list<Territory*> HumanPlayerStrategy::toDefend()
{
	return {};
}

list<Territory*> HumanPlayerStrategy::toAttack()
{
	return {};
}

string HumanPlayerStrategy::getStrategyString()
{
	return "Human";
}

//AggressivePlayerStrategy class implementation

AggressivePlayerStrategy::AggressivePlayerStrategy(Player* player) : PlayerStrategy(player) {} //Parameterized Constructor

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& playerStrategy) : PlayerStrategy(playerStrategy) {} //Copy Constructor

AggressivePlayerStrategy& AggressivePlayerStrategy::operator = (const AggressivePlayerStrategy& playerStrategy) //Assignment Operator Overloading
{
	if (this != &playerStrategy)
	{
		PlayerStrategy::operator=(playerStrategy);
	}

	return *this;
}

ostream& operator << (ostream& output, const AggressivePlayerStrategy& playerStrategy) //Stream Insertion Operator Overloading
{
	output << "Strategy Type: AggressivePlayerStrategy\n";
	return output;
}

AggressivePlayerStrategy* AggressivePlayerStrategy::clone() //Clone function
{
	return new AggressivePlayerStrategy(*this);
}

bool AggressivePlayerStrategy::issueOrder(Deck* deck)
{
	bool orderIssued = false;

	Territory* strongest = toDefend().front();

	if (player->getArmies() > 0)
	{
		player->issueOrder(new Deploy(player, strongest, player->getArmies()));
		player->setArmies(0);
		orderIssued = true;
	}
	
	for (Territory* adjacent : strongest->getAdjacentTerritories())
	{
		if (adjacent->getOwner() == player)
		{
			player->issueOrder(new Advance(player, adjacent, strongest, (adjacent->getArmies() - 1)));
			orderIssued = true;
		}
	}

	list<Territory*> attackList = toAttack();
	
	if (!attackList.empty())
	{
		Territory* strongestEnemy = nullptr;

		for (Territory* enemy : attackList)
		{
			if (strongestEnemy == nullptr || strongestEnemy->getArmies() < enemy->getArmies())
			{
				strongestEnemy = enemy;
			}
		}

		for (const auto& card : player->getHand()->getCards())
		{
			if (card->getTypeAsString() == "Bomb")
			{
				CardPlayContext& context = CardPlayContext();
				context.target = strongestEnemy;

				card->play(player, deck, player->getHand(), context);

				orderIssued = true;
			}
		}

		int armyAmount = strongest->getArmies() / attackList.size();
		int armyRemainder = strongest->getArmies() / attackList.size();

		while (!attackList.empty())
		{
			if (attackList.size() == 1)
			{
				armyAmount += (armyRemainder - 1);
			}

			player->issueOrder(new Advance(player, strongest, attackList.front(), armyAmount));
			attackList.pop_front();
			orderIssued = true;
		}
	}

	return orderIssued;
}

list<Territory*> AggressivePlayerStrategy::toDefend()
{
	bool enemyAdjacent = false;
	Territory* strongest = nullptr;

	for (Territory* territory : player->getTerritories())
	{
		if (strongest == nullptr)
		{
			strongest = territory;
		}

		bool hasEnemyNeighbours = false;

		for (Territory* adjacent : territory->getAdjacentTerritories())
		{
			if (adjacent->getOwner() != player)
			{
				hasEnemyNeighbours = true;
				break;
			}
		}

		if (!enemyAdjacent && hasEnemyNeighbours)
		{
			strongest = territory;
			enemyAdjacent = true;
		}
		else if (strongest->getArmies() < territory->getArmies() && ((enemyAdjacent && hasEnemyNeighbours) || (!enemyAdjacent && !hasEnemyNeighbours)))
		{
			strongest = territory;
		}
	}

	return {strongest};
}

list<Territory*> AggressivePlayerStrategy::toAttack()
{
	list<Territory*> attackList;

	Territory* attackFrom = toDefend().front();

	for (Territory* adjacent : attackFrom->getAdjacentTerritories())
	{
		if (adjacent->getOwner() != player)
		{
			attackList.push_back(adjacent);
		}
	}

	return attackList;
}

string AggressivePlayerStrategy::getStrategyString()
{
	return "Aggressive";
}

//BenevolentPlayerStrategy class implementation

BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player* player) : PlayerStrategy(player) {} //Parameterized Constructor

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& playerStrategy) : PlayerStrategy(playerStrategy) {} //Copy Constructor

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator = (const BenevolentPlayerStrategy& playerStrategy) //Assignment Operator Overloading
{
	if (this != &playerStrategy)
	{
		PlayerStrategy::operator=(playerStrategy);
	}

	return *this;
}

ostream& operator << (ostream& output, const BenevolentPlayerStrategy& playerStrategy) //Stream Insertion Operator Overloading
{
	output << "Strategy Type: BenevolentPlayerStrategy\n";
	return output;
}

BenevolentPlayerStrategy* BenevolentPlayerStrategy::clone() //Clone function
{
	return new BenevolentPlayerStrategy(*this);
}

bool BenevolentPlayerStrategy::issueOrder(Deck* deck)
{
	bool orderIssued = false;

	if (player->getArmies() > 0)
	{
		list<Territory*> defendList = toDefend();
		int armyAmount = player->getArmies() / defendList.size();
		int armyRemainder = player->getArmies() % defendList.size();

		while (!defendList.empty())
		{
			if (defendList.size() == 1)
			{
				armyAmount += armyRemainder;
			}

			player->issueOrder(new Deploy(player, defendList.front(), armyAmount));
			defendList.pop_front();
			orderIssued = true;
		}

		player->setArmies(0);
	}

	Territory* strongest = nullptr;

	for (Territory* territory : player->getTerritories())
	{
		if (strongest == nullptr || strongest->getArmies() < territory->getArmies())
		{
			strongest = territory;
		}
	}

	list<Territory*> defendList = toDefend();

	for (const auto& card : player->getHand()->getCards())
	{
		if (card->getTypeAsString() == "Reinforcement")
		{
			if (defendList.empty())
			{
				defendList = toDefend();
			}

			CardPlayContext& context = CardPlayContext();
			context.target = defendList.front();
			context.armies = 3;

			card->play(player, deck, player->getHand(), context);

			defendList.pop_front();
			orderIssued = true;
		}
		else if (card->getTypeAsString() == "Airlift")
		{
			if (defendList.empty())
			{
				defendList = toDefend();
			}

			CardPlayContext& context = CardPlayContext();
			context.source = strongest;
			context.target = defendList.front();
			context.armies = strongest->getArmies() / 2;

			card->play(player, deck, player->getHand(), context);

			defendList.pop_front();
			orderIssued = true;
		}
		else if (card->getTypeAsString() == "Blockade")
		{
			if (defendList.empty())
			{
				defendList = toDefend();
			}

			CardPlayContext& context = CardPlayContext();
			context.target = defendList.front();

			card->play(player, deck, player->getHand(), context);

			defendList.pop_front();
			orderIssued = true;
		}
		else if (card->getTypeAsString() == "Negotiate")
		{
			CardPlayContext& context = CardPlayContext();
			context.targetPlayer = player;

			bool targetPlayerFound = false;

			for (Territory* territory : player->getTerritories())
			{
				for (Territory* adjacentTerritory : territory->getAdjacentTerritories())
				{
					if (adjacentTerritory->getOwner() != player && adjacentTerritory->getOwner() != nullptr)
					{
						context.targetPlayer = adjacentTerritory->getOwner();
						targetPlayerFound = true;
						break;
					}
				}

				if (targetPlayerFound)
				{
					break;
				}
			}

			card->play(player, deck, player->getHand(), context);

			defendList.pop_front();
			orderIssued = true;
		}
	}

	list<Territory*> attackList = toAttack();
	int armyAmount = player->getArmies() / (attackList.size() + 1);

	while (!attackList.empty())
	{
		player->issueOrder(new Advance(player, strongest, attackList.front(), armyAmount));
		attackList.pop_front();
		orderIssued = true;
	}

	return orderIssued;
}

list<Territory*> BenevolentPlayerStrategy::toDefend()
{
	Territory* weakest = nullptr;

	for (Territory* territory : player->getTerritories())
	{
		if (weakest == nullptr || weakest->getArmies() > territory->getArmies())
		{
			weakest = territory;
		}
	}

	list<Territory*> defendList;

	for (Territory* territory : player->getTerritories())
	{
		if (weakest->getArmies() == territory->getArmies())
		{
			defendList.push_back(territory);
		}
	}

	return defendList;
}

list<Territory*> BenevolentPlayerStrategy::toAttack()
{
	Territory* strongest = nullptr;

	for (Territory* territory : player->getTerritories())
	{
		if (strongest == nullptr || strongest->getArmies() < territory->getArmies())
		{
			strongest = territory;
		}
	}

	Territory* weakestNeighbour = nullptr;

	for (Territory* territory : strongest->getAdjacentTerritories())
	{
		if (territory->getOwner() != player)
		{
			continue;
		}

		if (weakestNeighbour == nullptr || weakestNeighbour->getArmies() > territory->getArmies())
		{
			weakestNeighbour = territory;
		}
	}

	list<Territory*> advanceList;

	if (weakestNeighbour != nullptr)
	{
		for (Territory* territory : strongest->getAdjacentTerritories())
		{
			if (territory->getOwner() != player)
			{
				continue;
			}

			if (weakestNeighbour->getArmies() == territory->getArmies())
			{
				advanceList.push_back(territory);
			}
		}
	}

	return advanceList;
}

string BenevolentPlayerStrategy::getStrategyString()
{
	return "Benevolent";
}

//NeutralPlayerStrategy class implementation

NeutralPlayerStrategy::NeutralPlayerStrategy(Player* player) : PlayerStrategy(player) {} //Parameterized Constructor

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& playerStrategy) : PlayerStrategy(playerStrategy) {} //Copy Constructor

NeutralPlayerStrategy& NeutralPlayerStrategy::operator = (const NeutralPlayerStrategy& playerStrategy) //Assignment Operator Overloading
{
	if (this != &playerStrategy)
	{
		PlayerStrategy::operator=(playerStrategy);
	}

	return *this;
}

ostream& operator << (ostream& output, const NeutralPlayerStrategy& playerStrategy) //Stream Insertion Operator Overloading
{
	output << "Strategy Type: NeutralPlayerStrategy\n";
	return output;
}

NeutralPlayerStrategy* NeutralPlayerStrategy::clone() //Clone function
{
	return new NeutralPlayerStrategy(*this);
}

bool NeutralPlayerStrategy::issueOrder(Deck* deck)
{
	return false;
}

list<Territory*> NeutralPlayerStrategy::toDefend()
{
	return {};
}

list<Territory*> NeutralPlayerStrategy::toAttack()
{
	return {};
}

string NeutralPlayerStrategy::getStrategyString()
{
	return "Neutral";
}

//CheaterPlayerStrategy class implementation

CheaterPlayerStrategy::CheaterPlayerStrategy(Player* player) : PlayerStrategy(player) {} //Parameterized Constructor

CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& playerStrategy) : PlayerStrategy(playerStrategy) {} //Copy Constructor

CheaterPlayerStrategy& CheaterPlayerStrategy::operator = (const CheaterPlayerStrategy& playerStrategy) //Assignment Operator Overloading
{
	if (this != &playerStrategy)
	{
		PlayerStrategy::operator=(playerStrategy);
	}

	return *this;
}

ostream& operator << (ostream& output, const CheaterPlayerStrategy& playerStrategy) //Stream Insertion Operator Overloading
{
	output << "Strategy Type: CheaterPlayerStrategy";
	return output;
}

CheaterPlayerStrategy* CheaterPlayerStrategy::clone() //Clone function
{
	return new CheaterPlayerStrategy(*this);
}

bool CheaterPlayerStrategy::issueOrder(Deck* deck)
{
	return false;
}

list<Territory*> CheaterPlayerStrategy::toDefend()
{
	return {};
}

list<Territory*> CheaterPlayerStrategy::toAttack()
{
	return {};
}

string CheaterPlayerStrategy::getStrategyString()
{
	return "Cheater";
}