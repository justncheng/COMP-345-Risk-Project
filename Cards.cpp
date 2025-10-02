#include "Cards.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

// --------------------- Card Implementation -----------------------

Card::Card(CardType type) : type(type) {}

CardType Card::getType() const {
    return type;
}

string Card::getTypeAsString() const {
    switch (type) {
    case CardType::Bomb: return "Bomb";
    case CardType::Reinforcement: return "Reinforcement";
    case CardType::Blockade: return "Blockade";
    case CardType::Airlift: return "Airlift";
    case CardType::Diplomacy: return "Diplomacy";
    default: return "Unknown";
    }
}

void Card::play(Player* player, Deck* deck, Hand* hand, const CardPlayContext& ctx) {
    cout << "[Card] Played: " << getTypeAsString() << endl;
    cout << "  → Would create an Order and add it to player's OrdersList here." << endl;

    // Remove from hand and return to deck
    hand->removeCard(this);
    deck->addCard(make_unique<Card>(type));
}

// --------------------- Deck Implementation -----------------------

Deck::Deck() {
    srand(static_cast<unsigned>(time(nullptr)));
}

void Deck::addCard(unique_ptr<Card> card) {
    cards.push_back(move(card));
}

unique_ptr<Card> Deck::draw() {
    if (cards.empty()) return nullptr;

    int index = rand() % cards.size();
    auto card = move(cards[index]);
    cards.erase(cards.begin() + index);
    return card;
}

int Deck::size() const {
    return static_cast<int>(cards.size());
}

// --------------------- Hand Implementation -----------------------

Hand::Hand(const Hand& other) {
    for (const auto& card : other.getCards()) {
        cards.push_back(make_unique<Card>(card->getType()));
    }
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        cards.clear();
        for (const auto& card : other.getCards()) {
            cards.push_back(make_unique<Card>(card->getType()));
        }
    }
    return *this;
}

void Hand::addCard(unique_ptr<Card> card) {
    cards.push_back(move(card));
}

void Hand::removeCard(const Card* card) {
    cards.erase(remove_if(cards.begin(), cards.end(),
        [card](const unique_ptr<Card>& c) { return c.get() == card; }), cards.end());
}

void Hand::playAll(Player* player, Deck* deck, const vector<CardPlayContext>& contexts) {
    for (size_t i = 0; i < cards.size() && i < contexts.size(); ++i) {
        cards[i]->play(player, deck, this, contexts[i]);
    }
}

const vector<unique_ptr<Card>>& Hand::getCards() const {
    return cards;
}


