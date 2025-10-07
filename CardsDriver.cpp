#pragma once
#include "Cards.h"
#include "Map.h"
#include "Player.h"

/**
 * @brief Driver function to test card draw, play, and recycle behaviour.
 */
void testCards() {
    cout << "=== Cards Testing ===\n\n";

    Player* player = new Player();
    Deck* deck = new Deck();

    // Add one of each card type
    deck->addCard(make_unique<Card>(CardType::Reinforcement));
    deck->addCard(make_unique<Card>(CardType::Bomb));
    deck->addCard(make_unique<Card>(CardType::Blockade));
    deck->addCard(make_unique<Card>(CardType::Airlift));
    deck->addCard(make_unique<Card>(CardType::Diplomacy));

    // Draw 5 cards into the player's hand
    for (int i = 0; i < 5; ++i) {
        auto card = deck->draw();
        if (card)
            player->getHand()->addCard(move(card));
    }

    // Create dummy contexts for each card
    vector<CardPlayContext> contexts(5);
    for (int i = 0; i < 5; ++i) {
        contexts[i].source = new Territory("TerritoryA", 10, 20, "ContinentA");
        contexts[i].target = new Territory("TerritoryB", 20, 30, "ContinentA");
        contexts[i].targetPlayer = new Player();
        contexts[i].armies = i + 3;
    }

    // Display state before playing cards
    cout << "Initial Hand: " << *player->getHand() << endl;
    cout << "Deck Before Play: " << *deck << endl << endl;

    // Play all cards from the hand
    player->getHand()->playAll(player, deck, contexts);

    // Display state after playing cards
    cout << "Deck After Play: " << *deck << endl;
    cout << "Hand After Play: " << *player->getHand() << endl;

    // Cleanup
    for (auto& ctx : contexts) {
        delete ctx.source;
        delete ctx.target;
        delete ctx.targetPlayer;
    }

    delete player;
    delete deck;

    cout << "\n=== Cards Testing Complete ===\n\n";
}