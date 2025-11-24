#include "Cards.h"
#include "Player.h"
#include "Orders.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

// ---------------------- Card ----------------------------

Card::Card(CardType type) : type(type) {}

Card::Card(const Card& other) : type(other.type) {}

Card& Card::operator=(const Card& other) {
    if (this != &other)
        type = other.type;
    return *this;
}

CardType Card::getType() const {
    return type;
}

string Card::getTypeAsString() const {
    if (this == nullptr)
    {
        return "NULLCardType";
    }

    switch (type) {
    case CardType::Bomb: return "Bomb";
    case CardType::Reinforcement: return "Reinforcement";
    case CardType::Blockade: return "Blockade";
    case CardType::Airlift: return "Airlift";
    case CardType::Diplomacy: return "Diplomacy";
    default: return "UnknownCardType";
    }
}

void Card::play(Player* player, Deck* deck, Hand* hand, const CardPlayContext& context) {
    cout << "[Card] Played: " << getTypeAsString() << endl;
    cout << "Generated ";

    switch (type) {
    case CardType::Bomb:
		player->issueOrder(new Bomb(player, context.target));
        cout << "Bomb";
        break;
    case CardType::Reinforcement:
        player->issueOrder( new Deploy(player, context.target, context.armies));
        cout << "Reinforcement";
        break;
    case CardType::Blockade:
		player->issueOrder(new Blockade(player, context.target));
        cout << "Blockade";
        break;
    case CardType::Airlift:
		player->issueOrder(new Airlift(player, context.source, context.target, context.armies));
		cout << "Airlift";
        break;
    case CardType::Diplomacy:
		player->issueOrder(new Negotiate(player, context.targetPlayer));
		cout << "Diplomacy";
        break;
	default:
		cout << "Unknown";
        break;
    }

    cout << " Order and added it to " + player->getName() + "'s OrdersList.\n\n";

    // Capture type before removing this from the hand
    CardType currentType = this->getType();
    hand->removeCard(this);
    deck->addCard(make_unique<Card>(currentType));
}

ostream& operator<<(ostream& os, const Card& card) {
    os << "Card(" << card.getTypeAsString() << ")";
    return os;
}

// ---------------------- Deck ----------------------------

Deck::Deck() {
    srand(static_cast<unsigned>(time(nullptr))); // Randomness for draw()
}

Deck::Deck(const Deck& other) {
    for (const auto& card : other.cards)
        cards.push_back(make_unique<Card>(*card));
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        cards.clear();
        for (const auto& card : other.cards)
            cards.push_back(make_unique<Card>(*card));
    }
    return *this;
}

void Deck::addCard(unique_ptr<Card> card) {
    cards.push_back(move(card));
}

unique_ptr<Card> Deck::draw() {
    if (cards.empty()) return nullptr;
    int i = rand() % static_cast<int>(cards.size());
    auto selected = move(cards[i]);
    cards.erase(cards.begin() + i);
    return selected;
}

int Deck::size() const {
    return static_cast<int>(cards.size());
}

ostream& operator<<(ostream& os, const Deck& deck) {
    os << "Deck[" << deck.size() << "]: ";
    for (const auto& card : deck.cards)
        os << *card << " ";
    return os;
}

bool Deck::isEmpty() const {
    return cards.empty();
}

// ---------------------- Hand ----------------------------

Hand::Hand(const Hand& other) {
    for (const auto& card : other.cards)
        cards.push_back(make_unique<Card>(*card));
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        cards.clear();
        for (const auto& card : other.cards)
            cards.push_back(make_unique<Card>(*card));
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
    vector<Card*> toPlay;
    toPlay.reserve(cards.size());
    for (const auto& c : cards)
        toPlay.push_back(c.get());

    for (size_t i = 0; i < toPlay.size() && i < contexts.size(); ++i) {
        cout << "[Hand] Playing card #" << i + 1 << ": " << toPlay[i]->getTypeAsString() << endl;
        toPlay[i]->play(player, deck, this, contexts[i]);
    }
}

const vector<unique_ptr<Card>>& Hand::getCards() const {
    return cards;
}

ostream& operator<<(ostream& os, const Hand& hand) {
    os << "Hand[" << hand.cards.size() << "]: ";
    for (const auto& card : hand.cards)
        os << *card << " ";
    return os;
}

bool operator==(const Hand& playerOneHand, const Hand& playerTwoHand) {
    const auto& cards1 = playerOneHand.getCards();
    const auto& cards2 = playerTwoHand.getCards();

    if (cards1.size() != cards2.size())
        return false;

    for (size_t i = 0; i < cards1.size(); ++i) {
        if (cards1[i]->getType() != cards2[i]->getType())
            return false;
    }

    return true;
}

bool operator!=(const Hand& playerOneHand, const Hand& playerTwoHand) {
    return !(playerOneHand == playerTwoHand);
}



