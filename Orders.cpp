

#include "Orders.h"
#include "LoggingObserver.h"   
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>  
#include <typeinfo>
#include <random>

using namespace std;

// Orders Base Class Implementation ----------------------------------------------------------------

Order::Order() : name("Order"), executed(false), effect("None") {} // Default constructor
Order::Order(const std::string& name) : name(name), executed(false), effect("None") {} // Parameterized constructor

Order::Order(const Order& other) : name(other.name), executed(other.executed), effect(other.effect) {} // Copy constructor

Order& Order::operator=(const Order& other) // Assignment operator
{
    if (this != &other)
    {
        name = other.name;
        executed = other.executed;
        effect = other.effect;
    }
    return *this;
}

Order::~Order() {} // Destructor

std::string Order::toString() const // Converts order details to string
{
    return "Order: " + name + ", Executed: " + (executed ? "Yes" : "No") + ", Effect: " + effect;
}

bool Order::isExecuted() const // Checks if order has been executed
{
    return executed;
}

void Order::setEffect(const std::string& eff) // Sets the effect description
{
    this->effect = eff;
}

void Order::setName(const std::string& name) // Sets the name of the order
{
    this->name = name;
}

void Order::setExecuted(bool executed) // Sets the executed status
{
    this->executed = executed;
}

std::ostream& operator<<(std::ostream& os, const Order& order) // Stream insertion operator
{
    os << order.toString();
    if (order.executed)
    {
        os << " Effect: " << order.effect;
    }

    return os;
};

std::string Order::stringToLog() const {
    return "Order: " + name + " | Executed: " + (executed ? "Yes" : "No") + " | Effect: " + effect;
}

string Order::getName() const {
    return name;
}

// Deploy Class Implementation ---------------------------------------------------------------------

Deploy::Deploy() : Order("Deploy"), issuer(nullptr), target(nullptr), armies(0) {} // Default constructor

Deploy::Deploy(Player* issuer, Territory* target, int armies)
    : Order("Deploy"), issuer(issuer), target(target), armies(armies) {
}; // Parameterized constructor

Deploy::Deploy(const Deploy& other)
    : Order(other), issuer(other.issuer), target(other.target), armies(other.armies) {
}; // Copy constructor

Deploy& Deploy::operator=(const Deploy& other) // Assignment operator
{
    if (this != &other)
    {
        Order::operator=(other);
        issuer = other.issuer;
        target = other.target;
        armies = other.armies;
    }
    return *this;
}

Deploy::~Deploy() {} // Destructor

bool Deploy::validate() const // Validates the Deploy order
{
    if (!(issuer && target) || armies <= 0)
        return false;

    // must deploy on your own territory
    return (target->getOwner() == issuer);
}

void Deploy::execute() // Executes the Deploy order
{
    if (!validate()) {
        setEffect("Deploy invalid (must own target / bad armies).");
        setExecuted(false);
        std::cout << *this << std::endl;
        Notify(this); 
        return;
    }

    target->setArmies(target->getArmies() + armies);
    setEffect("deployed " + std::to_string(armies) + " to " + target->getName());
    setName("Deploy (executed)");
    setExecuted(true);
    std::cout << *this << std::endl;
    Notify(this); 
}

Order* Deploy::clone() const // Virtual constructor
{
    return new Deploy(*this);
}

std::string Deploy::toString() const //Converts Deploy order details to string
{
    return Order::toString() + ", Issuer: " + (issuer ? issuer->getName() : "None") +
", Target: " + (target ? target->getName() : "None") + ", Armies: " + to_string(armies);
}

std::ostream& operator<<(std::ostream& os, const Deploy& order)    
{ 
    return os << static_cast<const Order&>(order); 
}

// Advance Class Implementation --------------------------------------------------------------------

Advance::Advance() : Order("Advance"), issuer(nullptr), source(nullptr), target(nullptr), armies(0) {} // Default constructor

Advance::Advance(Player* issuer, Territory* source, Territory* target, int armies)
    : Order("Advance"), issuer(issuer), source(source), target(target), armies(armies) {
} // Parameterized constructor

Advance::Advance(const Advance& other)
    : Order(other), issuer(other.issuer), source(other.source), target(other.target), armies(other.armies) {
} // Copy constructor

Advance& Advance::operator=(const Advance& other) // Assignment operator
{
    if (this != &other)
    {
        Order::operator=(other);
        issuer = other.issuer;
        source = other.source;
        target = other.target;
        armies = other.armies;
    }
    return *this;
}

Advance::~Advance() {} // Destructor

bool Advance::validate() const // Validates the Advance order
{
    if (!(issuer && source && target) || source == target || armies <= 0)
        return false;

    // must own the source territory
    if (source->getOwner() != issuer)
        return false;

    // must have enough armies in source
    if (source->getArmies() < armies)
        return false;

    // must be adjacent
    if (!source->isAdjacentTo(target))
        return false;

    return true;
}

void Advance::execute() // Executes the Advance order
{
    if (!validate()) {
        setEffect("Advance invalid (ownership/armies/adjacency).");
        setExecuted(false);
        std::cout << *this << std::endl;
        Notify(this);
        return;
    }

    Player* defender = target->getOwner();

    // Friendly move (same owner)
    if (defender == issuer) {
        source->setArmies(source->getArmies() - armies);
        target->setArmies(target->getArmies() + armies);
        setEffect("moved " + std::to_string(armies) + " from " + source->getName() + " to " + target->getName());
        setName("Advance (executed)");
        setExecuted(true);
        std::cout << *this << std::endl;
        Notify(this);
        return;
    }

    // Hostile move — battle simulation
    int atkSent = armies;
    source->setArmies(source->getArmies() - atkSent);

    int atk = atkSent;
    int def = target->getArmies();

    std::random_device rd; // Generate random numbers
    std::mt19937 gen(rd()); // Mersenne Twister RNG
    std::binomial_distribution<int> atkKillsDist(atk, 0.60); // Attacker kills 60% of defending armies
    std::binomial_distribution<int> defKillsDist(def, 0.70); // Defender kills 70% of attacking armies

    int atkKills = atkKillsDist(gen);
    int defKills = defKillsDist(gen);

    int defAfter = std::max(0, def - atkKills);
    int atkAfter = std::max(0, atk - defKills);

    if (defAfter == 0 && atkAfter > 0) {
        // conquer: survivors occupy, transfer ownership
        target->setArmies(atkAfter);
        target->setOwner(issuer);
        setEffect("attacked " + target->getName() + ": conquered with " + std::to_string(atkAfter) + " surviving.");
    }
    else {
        // defender holds
        target->setArmies(defAfter);
        setEffect("attacked " + target->getName() + ": failed (def " + std::to_string(defAfter) + " left).");
    }

    setName("Advance (executed)");
    setExecuted(true);
    std::cout << *this << std::endl;
    Notify(this);
}

Order* Advance::clone() const // Virtual constructor
{
    return new Advance(*this);
}

std::string Advance::toString() const // Converts Advance order details to string
{
    return Order::toString() + ", Issuer: " + (issuer ? issuer->getName() : "None") +
        ", Source: " + (source ? source->getName() : "None") +
        ", Target: " + (target ? target->getName() : "None") +
        ", Armies: " + to_string(armies);
}

std::ostream& operator<<(std::ostream& os, const Advance& order) 
{ 
    return os << static_cast<const Order&>(order); 
}

// Bomb Class Implementation -----------------------------------------------------------------------

Bomb::Bomb() : Order("Bomb"), issuer(nullptr), target(nullptr) {} // Default constructor

Bomb::Bomb(Player* issuer, Territory* target)
    : Order("Bomb"), issuer(issuer), target(target) {
} // Parameterized constructor

Bomb::Bomb(const Bomb& other)
    : Order(other), issuer(other.issuer), target(other.target) {
} // Copy constructor

Bomb& Bomb::operator=(const Bomb& other) // Assignment operator
{
    if (this != &other)
    {
        Order::operator=(other);
        issuer = other.issuer;
        target = other.target;
    }
    return *this;
}

Bomb::~Bomb() {} // Destructor

bool Bomb::validate() const // Validates the Bomb order
{
    if (!(issuer && target))
        return false;

    Player* tgtOwner = target->getOwner();
    if (!tgtOwner || tgtOwner == issuer)
        return false; // must target an enemy

    // must be adjacent to at least one issuer-owned territory
    bool adjacentToIssuer = false;
    for (Territory* adj : target->getAdjacentTerritories()) {
        if (adj && adj->getOwner() == issuer) {
            adjacentToIssuer = true;
            break;
        }
    }
    if (!adjacentToIssuer)
        return false;

    return true;
}

void Bomb::execute() // Executes the Bomb order
{
    if (!validate()) {
        setEffect("Bomb invalid (must target adjacent enemy).");
        setExecuted(false);
        std::cout << *this << std::endl;
        Notify(this);
        return;
    }

    int cur = target->getArmies();
    int removed = cur / 2; // remove half (floor)
    target->setArmies(cur - removed);

    setEffect("bombed " + target->getName() + " removing " + std::to_string(removed));
    setName("Bomb (executed)");
    setExecuted(true);
    std::cout << *this << std::endl;
    Notify(this);
}

Order* Bomb::clone() const // Virtual constructor
{
    return new Bomb(*this);
}

std::string Bomb::toString() const // Converts Bomb order details to string
{
    return Order::toString() + ", Issuer: " + (issuer ? issuer->getName() : "None") +
        ", Target: " + (target ? target->getName() : "None");
}

std::ostream& operator<<(std::ostream& os, const Bomb& order) 
{ 
    return os << static_cast<const Order&>(order); 
}

// Blockade Class Implementation -------------------------------------------------------------------

Blockade::Blockade() : Order("Blockade"), issuer(nullptr), target(nullptr) {} // Default constructor

Blockade::Blockade(Player* issuer, Territory* target)
    : Order("Blockade"), issuer(issuer), target(target) {
} // Parameterized constructor

Blockade::Blockade(const Blockade& other)
    : Order(other), issuer(other.issuer), target(other.target) {
} // Copy constructor

Blockade& Blockade::operator=(const Blockade& other) // Assignment operator
{
    if (this != &other)
    {
        Order::operator=(other);
        issuer = other.issuer;
        target = other.target;
    }
    return *this;
}

Blockade::~Blockade() {} // Destructor

bool Blockade::validate() const // Validates the Blockade order
{
    if (!(issuer && target))
        return false;
    return (target->getOwner() == issuer);
}

void Blockade::execute() // Executes the Blockade order
{
    if (!validate()) {
        setEffect("Blockade invalid (must target own territory).");
        setExecuted(false);
        std::cout << *this << std::endl;
        Notify(this);
        return;
    }

    target->setArmies(target->getArmies() * 2);
    target->setOwner(nullptr); // neutral player placeholder

    setEffect("blockaded " + target->getName() + " (doubled, transferred to Neutral)");
    setName("Blockade (executed)");
    setExecuted(true);
    std::cout << *this << std::endl;
    Notify(this);
}

Order* Blockade::clone() const // Virtual constructor
{
    return new Blockade(*this);
}

std::string Blockade::toString() const // Converts Blockade order details to string
{
    return Order::toString() + ", Issuer: " + (issuer ? issuer->getName() : "None") +
        ", Target: " + (target ? target->getName() : "None");
}

std::ostream& operator<<(std::ostream& os, const Blockade& order) 
{ 
    return os << static_cast<const Order&>(order); 
}

// Airlift Class Implementation --------------------------------------------------------------------

Airlift::Airlift() : Order("Airlift"), issuer(nullptr), source(nullptr), target(nullptr), armies(0) {} // Default constructor

Airlift::Airlift(Player* issuer, Territory* source, Territory* target, int armies)
    : Order("Airlift"), issuer(issuer), source(source), target(target), armies(armies) {
} // Parameterized constructor

Airlift::Airlift(const Airlift& other)
    : Order(other), issuer(other.issuer), source(other.source), target(other.target), armies(other.armies) {
} // Copy constructor

Airlift& Airlift::operator=(const Airlift& other) // Assignment operator
{
    if (this != &other)
    {
        Order::operator=(other);
        issuer = other.issuer;
        source = other.source;
        target = other.target;
        armies = other.armies;
    }
    return *this;
}

Airlift::~Airlift() {} // Destructor

bool Airlift::validate() const // Validates the Airlift order
{
    if (!(issuer && source && target) || source == target || armies <= 0)
        return false;

    // must own both territories
    if (source->getOwner() != issuer || target->getOwner() != issuer)
        return false;

    if (source->getArmies() < armies)
        return false;

    return true;
}

void Airlift::execute()
{
    if (!validate()) {
        setEffect("Airlift invalid (ownership/armies).");
        setExecuted(false);
        std::cout << *this << std::endl;
        Notify(this);
        return;
    }

    source->setArmies(source->getArmies() - armies);
    target->setArmies(target->getArmies() + armies);

    setEffect("airlifted " + std::to_string(armies) + " from " + source->getName() + " to " + target->getName());
    setName("Airlift (executed)");
    setExecuted(true);
    std::cout << *this << std::endl;
    Notify(this);
}

Order* Airlift::clone() const // Virtual constructor
{
    return new Airlift(*this);
}

std::string Airlift::toString() const // Converts Airlift order details to string
{
    return Order::toString() + ", Issuer: " + (issuer ? issuer->getName() : "None") +
        ", Source: " + (source ? source->getName() : "None") +
        ", Target: " + (target ? target->getName() : "None") +
        ", Armies: " + to_string(armies);
}

std::ostream& operator<<(std::ostream& os, const Airlift& order) 
{ 
    return os << static_cast<const Order&>(order); 
}

// Negotiate Class Implementation --------------------------------------------------------------------

Negotiate::Negotiate() : Order("Negotiate"), issuer(nullptr), targetPlayer(nullptr) {} // Default constructor

Negotiate::Negotiate(Player* issuer, Player* targetPlayer)
    : Order("Negotiate"), issuer(issuer), targetPlayer(targetPlayer) {
} // Parameterized constructor

Negotiate::Negotiate(const Negotiate& other)
    : Order(other), issuer(other.issuer), targetPlayer(other.targetPlayer) {
} // Copy constructor

Negotiate& Negotiate::operator=(const Negotiate& other) // Assignment operator
{
    if (this != &other)
    {
        Order::operator=(other);
        issuer = other.issuer;
        targetPlayer = other.targetPlayer;
    }
    return *this;
}

Negotiate::~Negotiate() {} // Destructor

bool Negotiate::validate() const // Validates the Negotiate order
{
    return issuer && targetPlayer && (issuer != targetPlayer);
}

void Negotiate::execute() // Executes the Negotiate order
{
    if (!validate())
    {
        setEffect("Negotiate order execution failed: Invalid order.");
        cout << *this << endl;
        Notify(this);
        return;
    }
    setEffect("negotiated temporary peace with " + targetPlayer->getName());
    setName("Negotiate (executed)");
    setExecuted(true);
    cout << *this << endl;
    Notify(this);
}

Order* Negotiate::clone() const // Virtual constructor
{
    return new Negotiate(*this);
}

std::string Negotiate::toString() const // Converts Negotiate order details to string
{
    return Order::toString() + ", Issuer: " + (issuer ? issuer->getName() : "None") +
        ", Target Player: " + (targetPlayer ? targetPlayer->getName() : "None");
}

std::ostream& operator<<(std::ostream& os, const Negotiate& order) 
{ 
    return os << static_cast<const Order&>(order); 
}

// OrdersList Class Implementation -----------------------------------------------------------------

OrdersList::OrdersList() : orders() {} // Default constructor

OrdersList::OrdersList(const OrdersList& other) : orders() // Copy constructor
{
    deepCopy(other);
}

OrdersList& OrdersList::operator=(const OrdersList& other) // Assignment operator
{
    if (this != &other)
    {
        clear();
        deepCopy(other);
    }
    return *this;
}

OrdersList::~OrdersList() // Destructor
{
    clear();
}

void OrdersList::add(const Order& order) // Adds an order to the list (by reference)
{
    orders.push_back(order.clone());
    Notify(this); 
}

void OrdersList::add(Order* order) // Adds an order to the list (by pointer)
{
    if (!order) throw std::invalid_argument("Cannot add null order");
    orders.push_back(order->clone());
    Notify(this); 
}

void OrdersList::remove(int index) // Removes an order from the list by index
{
    if(index < 0 || index >= orders.size())
    {
        throw std::out_of_range("Index out of range");
    }
    delete orders[index];
    orders.erase(orders.begin() + index);
    Notify(this); 
}

void OrdersList::remove(Order* order) // Removes an order from the list by pointer
{
    if (!order) return;
    auto it = std::find(orders.begin(), orders.end(), order);
    if (it != orders.end())
    {
        delete* it;
        orders.erase(it);
        Notify(this); 
    }
}

void OrdersList::move(int fromIndex, int toIndex) // Moves an order within the list
{
    if (fromIndex < 0 || fromIndex >= orders.size() || toIndex < 0 || toIndex >= orders.size())
    {
        throw std::out_of_range("Index out of range");
    }
    if (fromIndex == toIndex) return; // No need to move if indices are the same

    Order* tmp = orders[fromIndex];
    orders.erase(orders.begin() + fromIndex);
    orders.insert(orders.begin() + toIndex, tmp);
    Notify(this); 
}

int OrdersList::size() const // Returns the number of orders in the list
{
    return orders.size();
}

Order& OrdersList::at(int index) // Returns the order at a specific index
{
    if (index < 0 || index >= orders.size())
    {
        throw std::out_of_range("Index out of range");
    }
    return *orders[index];
}

void OrdersList::clear() // Clears all orders from the list
{
    for(Order* order : orders)
    {
        delete order;
    }
    orders.clear();
    Notify(this); 
}

const Order& OrdersList::at(int index) const // Const version of at()
{
    if (index < 0 || index >= orders.size())
    {
        throw std::out_of_range("Index out of range");
    }
    return *orders[index];
}

void OrdersList::deepCopy(const OrdersList& other) // Deep copies from another OrdersList
{
    orders.reserve(other.orders.size());
    for (Order* order : other.orders)
    {
        orders.push_back(order->clone());
    }
}

bool OrdersList::operator==(const OrdersList& other) const
{
    if (orders.size() != other.orders.size())
        return false;

    for (size_t i = 0; i < orders.size(); ++i)
    {
        const Order* a = orders[i];
        const Order* b = other.orders[i];

        // same dynamic type?
        if (typeid(*a) != typeid(*b))
            return false;

        // same textual representation?
        std::ostringstream osa, osb;
        osa << *a;
        osb << *b;
        if (osa.str() != osb.str())
            return false;
    }
    return true;
}


std::ostream& operator<<(std::ostream& os, const OrdersList& ordersList) // Stream insertion operator, essentially adds itself into the stream, similar to toString()
{
    os << "OrdersList with " << ordersList.size() << " orders:\n";
    for (int i = 0; i < ordersList.size(); ++i)
    {
        os << i + 1 << ". " << ordersList.at(i) << "\n";
    }
    return os;
}


std::string OrdersList::stringToLog() const {
    return "OrdersList updated: now contains " + std::to_string(orders.size()) + " orders.";
}

