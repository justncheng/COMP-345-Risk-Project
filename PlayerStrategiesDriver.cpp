#include "PlayerStrategies.h"
#include "Map.h"
#include "Cards.h"
#include "Player.h"
#include "GameEngine.h"
#include "CommandProcessing.h"
#include "LoggingObserver.h"

void testPlayerStrategies()
{
	cout << "=== Player Strategies Testing ===\n\n";

	//Declare necessary objects
	GameEngine gEngine;
	CommandProcessor* commandProcessor = new CommandProcessor();
	Map* map = MapLoader::loadMap("TestMap.map");
	Deck* deck = new Deck();
	vector<Player*>* players = new vector<Player*>;

	//Add cards to the deck
	deck->addCard(make_unique<Card>(CardType::Reinforcement));
	deck->addCard(make_unique<Card>(CardType::Bomb));
	deck->addCard(make_unique<Card>(CardType::Blockade));
	deck->addCard(make_unique<Card>(CardType::Airlift));
	deck->addCard(make_unique<Card>(CardType::Diplomacy));

	deck->addCard(make_unique<Card>(CardType::Reinforcement));
	deck->addCard(make_unique<Card>(CardType::Bomb));
	deck->addCard(make_unique<Card>(CardType::Blockade));
	deck->addCard(make_unique<Card>(CardType::Airlift));
	deck->addCard(make_unique<Card>(CardType::Diplomacy));

	deck->addCard(make_unique<Card>(CardType::Reinforcement));
	deck->addCard(make_unique<Card>(CardType::Bomb));
	deck->addCard(make_unique<Card>(CardType::Blockade));
	deck->addCard(make_unique<Card>(CardType::Airlift));
	deck->addCard(make_unique<Card>(CardType::Diplomacy));

	deck->addCard(make_unique<Card>(CardType::Reinforcement));
	deck->addCard(make_unique<Card>(CardType::Bomb));
	deck->addCard(make_unique<Card>(CardType::Blockade));
	deck->addCard(make_unique<Card>(CardType::Airlift));
	deck->addCard(make_unique<Card>(CardType::Diplomacy));

	//Add players with different strategies
	players->push_back(new Player("Human Player", new HumanPlayerStrategy(nullptr)));
	players->push_back(new Player("Aggressive Player", new AggressivePlayerStrategy(nullptr)));
	players->push_back(new Player("Benevolent Player", new BenevolentPlayerStrategy(nullptr)));
	players->push_back(new Player("Neutral Player", new NeutralPlayerStrategy(nullptr)));
	players->push_back(new Player("Cheater Player", new CheaterPlayerStrategy(nullptr)));

	//Run the startup phase process (assign territories, determine order of play, etc.)
	gEngine.startupPhaseProcess(commandProcessor, map, players, deck);

	//Run the main game loop
	gEngine.mainGameLoop(commandProcessor, map, players, deck);

	cout << "\n=== Player Strategies Testing Complete ===\n\n";

	for (Player* player : *players)
	{
		delete player;
	}

	delete commandProcessor;
	delete map;
	delete players;
	delete deck;
}