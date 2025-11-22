#include "PlayerStrategies.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include "Player.h"
#include <set>

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
	output << "Strategy Type: " << playerStrategy.getStrategyString() << "\n";
	return output;
}

void PlayerStrategy::setPlayer(Player* player)
{
	this->player = player;
}

Player* PlayerStrategy::getPlayer()
{
	return player;
}

string PlayerStrategy::getStrategyString() const
{
	return "Abstract";
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
	output << "Strategy Type: Human\n";
	return output;
}

HumanPlayerStrategy* HumanPlayerStrategy::clone() //Clone function
{
	return new HumanPlayerStrategy(*this);
}

bool HumanPlayerStrategy::issueOrder(Deck* deck)
{
	// Basic menu for human interaction
	cout << "=== Human Player (" << player->getName() << ") - Issue Order ===\n";

	cout << "Reinforcement armies available: " << player->getArmies() << "\n";

	cout << "Choose an action:\n";
	cout << "  1) Deploy armies\n";
	cout << "  2) Advance armies\n";
	cout << "  3) Play a card\n";
	cout << "  4) Done issuing orders\n";
	cout << "Enter choice: ";

	int choice;
	cin >> choice;

	// Clear input errors if any
	if (!cin) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return false;
	}

	// Helper lambdas to show territories/cards
	auto showTerritories = [this]() {
		cout << "\nYour territories:\n";
		int idx = 0;
		for (Territory* t : player->getTerritories()) {
			cout << "  [" << idx << "] "
				<< t->getName()
				<< " (armies: " << t->getArmies() << ")\n";
			++idx;
		}
	};

	auto getTerritoryByIndex = [this](int index) -> Territory* {
		if (index < 0) return nullptr;
		int i = 0;
		for (Territory* t : player->getTerritories()) {
			if (i == index) return t;
			++i;
		}
		return nullptr;
	};

	switch (choice) {
		case 1: { // Deploy
			if (player->getArmies() <= 0) {
				cout << "You have no reinforcement armies to deploy.\n";
				return false;
			}

			showTerritories();
			cout << "Enter index of territory to deploy to: ";
			int idx;
			cin >> idx;

			Territory* target = getTerritoryByIndex(idx);
			if (!target) {
				cout << "Invalid territory index.\n";
				return false;
			}

			cout << "Enter number of armies to deploy (max "
				<< player->getArmies() << "): ";
			int num;
			cin >> num;

			if (num <= 0 || num > player->getArmies()) {
				cout << "Invalid army amount.\n";
				return false;
			}

			player->issueOrder(new Deploy(player, target, num));
			player->setArmies(player->getArmies() - num);
			cout << "Issued Deploy(" << num << " -> " << target->getName() << ")\n";
			return true;
		}

		case 2: { // Advance
			showTerritories();
			cout << "Enter index of source territory: ";
			int srcIdx;
			cin >> srcIdx;
			Territory* src = getTerritoryByIndex(srcIdx);
			if (!src) {
				cout << "Invalid source index.\n";
				return false;
			}

			if (src->getArmies() <= 1) {
				cout << "Source territory must have more than 1 army to advance.\n";
				return false;
			}

			cout << "\nAdjacent territories from " << src->getName() << ":\n";
			int idx = 0;
			std::vector<Territory*> adjList;
			for (Territory* adj : src->getAdjacentTerritories()) {
				cout << "  [" << idx << "] "
					<< adj->getName()
					<< " (owner: "
					<< (adj->getOwner() ? adj->getOwner()->getName() : "None")
					<< ", armies: " << adj->getArmies() << ")\n";
				adjList.push_back(adj);
				++idx;
			}

			if (adjList.empty()) {
				cout << "No adjacent territories to advance to.\n";
				return false;
			}

			cout << "Enter index of destination territory: ";
			int dstIdx;
			cin >> dstIdx;
			if (dstIdx < 0 || dstIdx >= static_cast<int>(adjList.size())) {
				cout << "Invalid destination index.\n";
				return false;
			}

			Territory* dst = adjList[dstIdx];

			cout << "Enter number of armies to advance (max "
				<< (src->getArmies() - 1) << "): ";
			int num;
			cin >> num;
			if (num <= 0 || num >= src->getArmies()) {
				cout << "Invalid army amount.\n";
				return false;
			}

			player->issueOrder(new Advance(player, src, dst, num));
			cout << "Issued Advance(" << num << " from "
				<< src->getName() << " to " << dst->getName() << ")\n";
			return true;
		}

		case 3: { // Play card
			Hand* hand = player->getHand();
			auto& cards = hand->getCards();

			if (cards.empty()) {
				cout << "You have no cards to play.\n";
				return false;
			}

			cout << "\nYour cards:\n";
			int idx = 0;

			for (const auto& uptr : cards) {
				Card* c = uptr.get();
				cout << "  [" << idx << "] " << c->getTypeAsString() << "\n";
				++idx;
			}

			cout << "Enter index of card to play: ";
			int cardIdx;
			cin >> cardIdx;
			if (cardIdx < 0 || cardIdx >= static_cast<int>(cards.size())) {
				cout << "Invalid card index.\n";
				return false;
			}

			Card* card = cards[cardIdx].get();
			std::string type = card->getTypeAsString();
			CardPlayContext context; // default context

			if (type == "Bomb") {
				// choose enemy territory to bomb
				cout << "\nChoose enemy territory to bomb:\n";
				std::vector<Territory*> enemyTerr;
				int i = 0;

				list<Territory*> attackList = toAttack();
				for (Territory* t : attackList) { // if you have such helper; otherwise build like toAttack()
					cout << "  [" << i << "] " << t->getName()
						<< " (owner: " << t->getOwner()->getName()
						<< ", armies: " << t->getArmies() << ")\n";
					enemyTerr.push_back(t);
					++i;
				}
				if (enemyTerr.empty()) {
					cout << "No enemy territories in range to bomb.\n";
					return false;
				}
				int tIdx;
				cin >> tIdx;
				if (tIdx < 0 || tIdx >= static_cast<int>(enemyTerr.size())) {
					cout << "Invalid target index.\n";
					return false;
				}
				context.target = enemyTerr[tIdx];
			}
			// For brevity, you can similarly ask inputs for Airlift, Blockade, Negotiate
			// depending on your CardPlayContext structure.

			card->play(player, deck, hand, context);
			cout << "Played card: " << type << "\n";
			return true;
		}

		case 4:
		default:
			cout << "Done issuing orders for this turn.\n\n";
			return false;
	}
}

list<Territory*> HumanPlayerStrategy::toDefend()
{
	// For a human, just return all owned territories; the player decides in issueOrder.
	list<Territory*> defendList;
	for (Territory* t : player->getTerritories()) {
		defendList.push_back(t);
	}
	return defendList;
}

list<Territory*> HumanPlayerStrategy::toAttack()
{
	// All adjacent enemy territories to any of the human player's territories.
	list<Territory*> attackList;
	std::set<Territory*> seen;

	for (Territory* t : player->getTerritories()) {
		for (Territory* adj : t->getAdjacentTerritories()) {
			if (adj->getOwner() != player && adj->getOwner() != nullptr) {
				if (!seen.count(adj)) {
					seen.insert(adj);
					attackList.push_back(adj);
				}
			}
		}
	}

	return attackList;
}

string HumanPlayerStrategy::getStrategyString() const
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
	output << "Strategy Type: Aggressive\n";
	return output;
}

AggressivePlayerStrategy* AggressivePlayerStrategy::clone() //Clone function
{
	return new AggressivePlayerStrategy(*this);
}

bool AggressivePlayerStrategy::issueOrder(Deck* deck)
{
	if (player->getTerritories().empty())
	{
		return false;
	}

	bool orderIssued = false;

	Territory* strongest = toDefend().front(); //Finds the strongest territory owned

	if (player->getArmies() > 0) //Checks if there are armies to deploy
	{
		//Deploys all armies to the strongest territory
		player->issueOrder(new Deploy(player, strongest, player->getArmies()));
		player->setArmies(0);
		orderIssued = true;
	}
	
	//Advances all possible armies from adjacent territories to the strongest territory
	for (Territory* adjacent : strongest->getAdjacentTerritories())
	{
		if (adjacent->getOwner() == player && adjacent->getArmies() > 1)
		{
			player->issueOrder(new Advance(player, adjacent, strongest, (adjacent->getArmies() - 1)));
			orderIssued = true;
		}
	}

	list<Territory*> attackList = toAttack(); //Finds all possible territories to attack from the strongest territory
	
	if (!attackList.empty()) //Checks if there are any territories to attack
	{
		Territory* strongestEnemy = nullptr;

		for (Territory* enemy : attackList) //Finds the strongest enemy territory
		{
			if (strongestEnemy == nullptr || strongestEnemy->getArmies() < enemy->getArmies())
			{
				strongestEnemy = enemy;
			}
		}

		//Plays all possible Bomb cards on the strongest enemy territory
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

		//Calculates armies to attack with
		int armyAmount = strongest->getArmies() / attackList.size();
		int armyRemainder = strongest->getArmies() % attackList.size();

		//Issues Advance orders to all territories in the attack list
		while (!attackList.empty() && strongest->getArmies() > 1 && armyAmount > 0)
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
	if (player->getTerritories().empty())
	{
		return {};
	}

	bool enemyAdjacent = false;
	Territory* strongest = nullptr;

	//Iterates through all territories owned by the player
	for (Territory* territory : player->getTerritories())
	{
		if (strongest == nullptr)
		{
			strongest = territory;
		}

		bool hasEnemyNeighbours = false;

		//Check if territory has any enemy neighbours
		for (Territory* adjacent : territory->getAdjacentTerritories())
		{
			if (adjacent->getOwner() != player)
			{
				hasEnemyNeighbours = true;
				break;
			}
		}

		//Select the strongest territory with enemy neighbours
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
	if (player->getTerritories().empty())
	{
		return {};
	}

	list<Territory*> attackList;

	Territory* attackFrom = toDefend().front();

	//Iterates through all adjacent territories to find possible attack targets
	for (Territory* adjacent : attackFrom->getAdjacentTerritories())
	{
		//Adds to attack list if the adjacent territory is not owned by the player
		if (adjacent->getOwner() != player)
		{
			attackList.push_back(adjacent);
		}
	}

	return attackList;
}

string AggressivePlayerStrategy::getStrategyString() const
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
	output << "Strategy Type: Benevolent\n";
	return output;
}

BenevolentPlayerStrategy* BenevolentPlayerStrategy::clone() //Clone function
{
	return new BenevolentPlayerStrategy(*this);
}

bool BenevolentPlayerStrategy::issueOrder(Deck* deck)
{
	if (player->getTerritories().empty())
	{
		return false;
	}

	bool orderIssued = false;
	
	if (player->getArmies() > 0) //Checks if there are armies to deploy
	{
		//Calculates armies to deploy to each territory in the defend list
		list<Territory*> defendList = toDefend();
		int armyAmount = player->getArmies() / defendList.size();
		int armyRemainder = player->getArmies() % defendList.size();

		//Issues Deploy orders to all territories in the defend list
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

	//Finds the strongest territory owned
	for (Territory* territory : player->getTerritories())
	{
		if (strongest == nullptr || strongest->getArmies() < territory->getArmies())
		{
			strongest = territory;
		}
	}

	list<Territory*> defendList = toDefend();

	//Plays all possible Reinforcement, Airlift, Blockade and Negotiate cards
	for (const auto& card : player->getHand()->getCards())
	{
		if (!defendList.empty() && strongest == defendList.front())
		{
			defendList.pop_front();
		}

		if (card->getTypeAsString() == "Reinforcement")
		{
			//Plays Reinforcement card on the weakest territory
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
			//Plays Airlift card to move armies to the weakest territory
			if (defendList.empty())
			{
				defendList = toDefend();
			}

			if (strongest != nullptr)
			{
				CardPlayContext& context = CardPlayContext();
				context.source = strongest;
				context.target = defendList.front();
				context.armies = strongest->getArmies() / 2;

				card->play(player, deck, player->getHand(), context);

				defendList.pop_front();
				orderIssued = true;
			}
		}
		else if (card->getTypeAsString() == "Blockade")
		{
			//Plays Blockade card on the weakest territory
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
			//Plays Negotiate card with an adjacent enemy player
			CardPlayContext& context = CardPlayContext();
			context.targetPlayer = player;

			bool targetPlayerFound = false;

			//Finds an adjacent enemy player
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

			if (targetPlayerFound)
			{
				card->play(player, deck, player->getHand(), context);

				defendList.pop_front();
				orderIssued = true;
			}
		}
	}

	list<Territory*> attackList = toAttack();
	int armyAmount = player->getArmies() / (attackList.size() + 1);

	//Issues Advance orders to all territories neighbouring the strongest territory
	while (!attackList.empty())
	{
		if (armyAmount == 0)
		{
			break;
		}

		player->issueOrder(new Advance(player, strongest, attackList.front(), armyAmount));
		attackList.pop_front();
		orderIssued = true;
	}

	return orderIssued;
}

list<Territory*> BenevolentPlayerStrategy::toDefend()
{
	if (player->getTerritories().empty())
	{
		return {};
	}

	Territory* weakest = nullptr;

	//Finds the weakest territory owned
	for (Territory* territory : player->getTerritories())
	{
		if (weakest == nullptr || weakest->getArmies() > territory->getArmies())
		{
			weakest = territory;
		}
	}

	list<Territory*> defendList;

	//Finds all territories with the same army count as the weakest territory
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
	if (player->getTerritories().empty())
	{
		return {};
	}

	Territory* strongest = nullptr;

	//Finds the strongest territory owned
	for (Territory* territory : player->getTerritories())
	{
		if (strongest == nullptr || strongest->getArmies() < territory->getArmies())
		{
			strongest = territory;
		}
	}

	Territory* weakestNeighbour = nullptr;

	//Finds the weakest neighbouring territory
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

	//Finds all neighbouring territories with the same army count as the weakest neighbouring territory
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

string BenevolentPlayerStrategy::getStrategyString() const
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
	output << "Strategy Type: Neutral\n";
	return output;
}

NeutralPlayerStrategy* NeutralPlayerStrategy::clone() //Clone function
{
	return new NeutralPlayerStrategy(*this);
}

bool NeutralPlayerStrategy::issueOrder(Deck* deck)
{
	//Issues no orders
	return false;
}

list<Territory*> NeutralPlayerStrategy::toDefend()
{
	//Defends no territories
	return {};
}

list<Territory*> NeutralPlayerStrategy::toAttack()
{
	//Attacks no territories
	return {};
}

string NeutralPlayerStrategy::getStrategyString() const
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
	output << "Strategy Type: Cheater";
	return output;
}

CheaterPlayerStrategy* CheaterPlayerStrategy::clone() //Clone function
{
	return new CheaterPlayerStrategy(*this);
}

bool CheaterPlayerStrategy::issueOrder(Deck* deck)
{
	std::set<Territory*> toConquer;

	// Collect all adjacent enemy territories
	for (Territory* myTerr : player->getTerritories()) {
		for(Territory* adjTerr : myTerr->getAdjacentTerritories()) {
			if (adjTerr->getOwner() != player) {
				toConquer.insert(adjTerr);
			}
		}
	}

	if (toConquer.empty()) {
		return false; // No territories to conquer
	}

	// Perform the conquest
	for(Territory* enemyTerr : toConquer) {
		Player* previousOwner = enemyTerr->getOwner();
		if (previousOwner != nullptr) {
			if(previousOwner->getPlayerStrategy()->getStrategyString() == "Neutral") {
				previousOwner->setPlayerStrategy(new AggressivePlayerStrategy(previousOwner));
			}

			previousOwner->removeTerritory(enemyTerr);
		}
		enemyTerr->setOwner(player);
		player->addTerritory(enemyTerr);
	}

	std::cout << player->getName() << " (Cheater Player) has conquered " << toConquer.size() << " adjacent territories! \n" << std::endl;

	// We don't issue traditional orders, return false
	return false;
}

list<Territory*> CheaterPlayerStrategy::toDefend()
{
	list<Territory*> defendList;
	for(Territory* territory : player->getTerritories())
	{
		defendList.push_back(territory);
	}
	return defendList;
}

list<Territory*> CheaterPlayerStrategy::toAttack()
{
	list<Territory*> attackList;
	std::set<Territory*> uniqueAttackList; // To avoid duplicates

	for(Territory* myTerr : player->getTerritories()) {
		for(Territory* adjTerr : myTerr->getAdjacentTerritories()) {
			if (adjTerr->getOwner() != player && adjTerr->getOwner() != nullptr) {
				if(!uniqueAttackList.count(adjTerr)) {
					uniqueAttackList.insert(adjTerr);
					attackList.push_back(adjTerr);
				}
			}
		}
	}

	return attackList;
}

string CheaterPlayerStrategy::getStrategyString() const
{
	return "Cheater";
}