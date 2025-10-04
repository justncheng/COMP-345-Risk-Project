#pragma once
#include "Cards.h"

/**
 * @brief Driver function to test card draw, play, and recycle behaviour.
 */
void testCards() {
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
        contexts[i].source = new Territory();
        contexts[i].target = new Territory();
        contexts[i].targetPlayer = new Player();
        contexts[i].armies = i + 3;
    }

    // Display state before playing cards
    cout << "\nInitial Hand: " << *player->getHand() << endl;
    cout << "Deck Before Play: " << *deck << endl << endl;

    // Play all cards from the hand
    player->getHand()->playAll(player, deck, contexts);

    // Display state after playing cards
    cout << "\nDeck After Play: " << *deck << endl;
    cout << "Hand After Play: " << *player->getHand() << endl;

    // Cleanup
    for (auto& ctx : contexts) {
        delete ctx.source;
        delete ctx.target;
        delete ctx.targetPlayer;
    }

    delete player;
    delete deck;
}