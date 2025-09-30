#include "Cards.h"

// Temporary mock classes to allow testing
class Territory {
public:
    string name = "TestLand";
};

class Player {
public:
    OrdersList* orderList = nullptr;
    Hand* hand = new Hand();

    Player() = default;
    ~Player() { delete hand; }
};

/**
 * @brief Simulates drawing, playing, and recycling cards.
 */
void testCards() {
    Player* player = new Player();
    Deck* deck = new Deck();

    // Populate deck with one of each type
    deck->addCard(make_unique<Card>(CardType::Reinforcement));
    deck->addCard(make_unique<Card>(CardType::Bomb));
    deck->addCard(make_unique<Card>(CardType::Blockade));
    deck->addCard(make_unique<Card>(CardType::Airlift));
    deck->addCard(make_unique<Card>(CardType::Diplomacy));

    // Draw all 5 into the player's hand
    for (int i = 0; i < 5; ++i) {
        auto card = deck->draw();
        if (card) player->hand->addCard(move(card));
    }

    // Dummy contexts for each card
    vector<CardPlayContext> contexts(5);
    for (int i = 0; i < 5; ++i) {
        contexts[i].source = new Territory();
        contexts[i].target = new Territory();
        contexts[i].targetPlayer = new Player();
        contexts[i].armies = i + 1;
    }

    cout << "\n=== Playing All Cards ===" << endl;
    player->hand->playAll(player, deck, contexts);
    cout << "\nDeck size after play: " << deck->size() << endl;

    // Cleanup dummy territories/players
    for (auto& ctx : contexts) {
        delete ctx.source;
        delete ctx.target;
        delete ctx.targetPlayer;
    }

    delete player;
    delete deck;
}

/**
 * @brief Main entry point to run card test.
 */
int main() {
    testCards();
    return 0;
}


