#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "Map.h"
#include "Player.h"
#include "Orders.h"

using namespace std;

// Forward declarations to avoid circular dependency.
class Territory;
class Player;
class Order;
class OrdersList;

/**
 * @brief The type of a Warzone card.
 */
enum class CardType {
    Bomb,           ///< Destroys half of armies on a target enemy territory
    Reinforcement,  ///< Allows deploying reinforcements
    Blockade,       ///< Triples armies and turns territory neutral
    Airlift,        ///< Moves armies between any two territories
    Diplomacy       ///< Prevents attacks between two players for the turn
};

/**
 * @brief Context data needed when playing a card.
 * This context is passed to the play() function to supply relevant territories, player targets, and army counts.
 */
class CardPlayContext {
public:
    Territory* source = nullptr;       ///< Source territory (used for airlift)
    Territory* target = nullptr;       ///< Target territory
    Player* targetPlayer = nullptr;    ///< Other player (for diplomacy)
    int armies = 0;                    ///< Number of armies involved
};

/**
 * @brief Represents a single card in Warzone.
 * Responsible for generating orders and returning itself to the deck.
 */
class Card {
public:
    explicit Card(CardType type);
    CardType getType() const;
    string getTypeAsString() const;

    /**
     * @brief Plays the card using the given context.
     * Generates the corresponding order and adds it to the player's OrdersList.
     */
    void play(Player* player, class Deck* deck, class Hand* hand, const CardPlayContext& context);

private:
    CardType type;
};

/**
 * @brief Represents a deck of Warzone cards.
 * Allows drawing and returning cards.
 */
class Deck {
public:
    Deck();
    void addCard(unique_ptr<Card> card);
    unique_ptr<Card> draw();
    int size() const;

private:
    vector<unique_ptr<Card>> cards;
};

/**
 * @brief Represents a hand of cards owned by a player.
 * Responsible for storing, copying, and playing cards.
 */
class Hand {
public:
    Hand() = default;
    Hand(const Hand& other);
    Hand& operator=(const Hand& other);

    bool operator == (const Hand& other) {return true;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
    friend ostream& operator << (ostream &output, const Hand& hand) {return output;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT

    ~Hand() = default;

    void addCard(unique_ptr<Card> card);
    void removeCard(const Card* card);

    /**
     * @brief Plays all cards in the hand using the provided contexts.
     * One context per card, in order.
     */
    void playAll(Player* player, Deck* deck, const vector<CardPlayContext>& contexts);

    const vector<unique_ptr<Card>>& getCards() const;

private:
    vector<unique_ptr<Card>> cards;
};

/**
 * @brief Runs a test scenario using cards, hands, and decks.
 */
void testCards();

#endif



