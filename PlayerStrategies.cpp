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
	using std::cout;
	using std::cin;
	using std::endl;

	// Basic menu for human interaction
	cout << "\n=== Human Player (" << player->getName() << ") - Issue Order ===\n";
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
		using std::cout;
		using std::endl;

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
		cout << "Done issuing orders for this turn.\n";
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
			previousOwner->removeTerritory(enemyTerr);
		}
		enemyTerr->setOwner(player);
		player->addTerritory(enemyTerr);
	}

	std::cout << "Cheater Player " << player->getName() << " has conquered " << toConquer.size() << " adjacent territories! \n" << std::endl;

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

string CheaterPlayerStrategy::getStrategyString()
{
	return "Cheater";
}