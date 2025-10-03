#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

// Forward declarations
class Player;
class OrdersList;
class Order;
class Territory;

/**
 * @enum CardType
 * @brief Enum representing the available Warzone card types.
 *
 * Each card type corresponds to a unique gameplay action that a player
 * can trigger by playing the card (e.g., bombing a territory, initiating diplomacy).
 */
enum class CardType {
    Bomb,           ///< Destroys half of the armies on a target enemy territory
    Reinforcement,  ///< Grants additional armies to the player's pool
    Blockade,       ///< Triples armies on a territory and turns it neutral
    Airlift,        ///< Transfers armies between any two territories
    Diplomacy       ///< Prevents mutual attacks between two players for the turn
};

/**
 * @class CardPlayContext
 * @brief Encapsulates contextual information used when a card is played.
 *
 * This object is passed when playing a card and contains any relevant
 * input needed by the card logic (e.g., which territory to target).
 */
class CardPlayContext {
public:
    Territory* source = nullptr;        ///< Source territory (used by Advance, Airlift)
    Territory* target = nullptr;        ///< Target territory affected by the card
    Player* targetPlayer = nullptr;     ///< Target player (used in Diplomacy)
    int armies = 0;                     ///< Number of armies to be moved or affected
};

/**
 * @class Card
 * @brief Represents a single card in the game with type and behavior.
 *
 * Cards can be played by players to issue special orders.
 * After being played, cards are returned to the deck.
 */
class Card {
public:
    explicit Card(CardType type);                     ///< Constructs a card with a specified type
    Card(const Card& other);                          ///< Copy constructor
    Card& operator=(const Card& other);               ///< Assignment operator
    ~Card() = default;                                ///< Destructor

    CardType getType() const;                         ///< Returns the type of the card
    string getTypeAsString() const;                   ///< Returns the type as a human-readable string

    /**
     * @brief Plays the card (simulates order creation).
     *
     * The card is removed from the hand and returned to the deck.
     */
    void play(Player* player, class Deck* deck, class Hand* hand, const CardPlayContext& context);

    friend ostream& operator<<(ostream& os, const Card& card);  ///< Stream print of card
private:
    Card() = delete;       ///< Disables default construction
    CardType type;         ///< The functional type of the card
};

/**
 * @class Deck
 * @brief Represents a draw pile of cards available in the game.
 *
 * Cards are drawn from the deck into a player's hand and returned
 * after they are played. Internally uses a vector of unique pointers.
 */
class Deck {
public:
    Deck();                                 ///< Constructs an empty deck
    Deck(const Deck& other);                ///< Copy constructor (deep copies all cards)
    Deck& operator=(const Deck& other);     ///< Assignment operator (deep copies)
    ~Deck() = default;

    void addCard(unique_ptr<Card> card);    ///< Adds a card back into the deck
    unique_ptr<Card> draw();                ///< Draws a random card (removes from deck)
    int size() const;                       ///< Returns the number of cards in the deck

    friend ostream& operator<<(ostream& os, const Deck& deck);  ///< Stream print of deck
private:
    vector<unique_ptr<Card>> cards;         ///< Internal list of cards
};

/**
 * @class Hand
 * @brief Represents a player's collection of cards.
 *
 * Cards are stored as unique pointers and can be added, removed,
 * or played. The hand supports comparison operators and printing.
 */
class Hand {
public:
    Hand() = default;
    Hand(const Hand& other);                      ///< Deep copy constructor
    Hand& operator=(const Hand& other);           ///< Deep assignment operator
    ~Hand() = default;

    void addCard(unique_ptr<Card> card);          ///< Adds a new card to the hand
    void removeCard(const Card* card);            ///< Removes a specific card (by pointer)
    void playAll(Player* player, Deck* deck, const vector<CardPlayContext>& contexts); ///< Plays all cards with given contexts

    const vector<unique_ptr<Card>>& getCards() const; ///< Returns read-only view of cards

    friend ostream& operator<<(ostream& os, const Hand& hand);                   ///< Stream print of hand
    friend bool operator==(const Hand& playerOneHand, const Hand& playerTwoHand);///< Equality comparison
    friend bool operator!=(const Hand& playerOneHand, const Hand& playerTwoHand);///< Inequality comparison
private:
    vector<unique_ptr<Card>> cards;        ///< Internal list of cards
};

/**
 * @brief Standalone function for testing the card-hand-deck system.
 */
void testCards();

#endif





