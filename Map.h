#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

class Player; // Forward declaration

// Territory class
class Territory {
public:
    Territory(const std::string& name, int x, int y, const std::string& continent);
    
    // Rule of Three
    Territory(const Territory& other);
    Territory& operator=(const Territory& other);
    ~Territory();
    
    //Comparison Operator Overloading
    bool operator == (const Territory& territory);
    bool operator != (const Territory& territory);

    // Getters
    std::string getName() const;
    std::string getContinent() const;
    Player* getOwner() const;
    int getArmies() const;
    
    // Setters
    void setOwner(Player* owner);
    void setArmies(int armies);
    
    // Adjacency management
    void addAdjacentTerritory(Territory* territory);
    const std::vector<Territory*>& getAdjacentTerritories() const;
    
    // Stream insertion
    friend std::ostream& operator<<(std::ostream& os, const Territory& territory);

private:
    std::string* name;
    int x;
    int y;
    std::string* continent;
    Player* owner;
    int armies;
    std::vector<Territory*> adjacentTerritories;
};

// Continent class
class Continent {
public:
    Continent(const std::string& name, int bonus);
    
    // Rule of Three
    Continent(const Continent& other);
    Continent& operator=(const Continent& other);
    ~Continent();
    
    // Getters
    std::string getName() const;
    int getBonus() const;
    
    // Territory management
    void addTerritory(Territory* territory);
    const std::vector<Territory*>& getTerritories() const;
    
    // Stream insertion
    friend std::ostream& operator<<(std::ostream& os, const Continent& continent);

private:
    std::string* name;
    int bonus;
    std::vector<Territory*> territories;
};

// Map class
class Map {
public:
    Map();
    
    // Rule of Three
    Map(const Map& other);
    Map& operator=(const Map& other);
    ~Map();
    
    // Territory and Continent management
    void addTerritory(Territory* territory);
    void addContinent(Continent* continent);
    
    // Getters
    const std::vector<Territory*>& getTerritories() const;
    const std::vector<Continent*>& getContinents() const;
    Territory* getTerritoryByName(const std::string& name) const;
    
    // Validation methods
    bool validate();
    bool isConnectedGraph();
    bool continentsAreConnectedSubgraphs();
    bool eachCountryInOneContinent();
    
    // Stream insertion
    friend std::ostream& operator<<(std::ostream& os, const Map& map);

private:
    std::vector<Territory*> territories;
    std::vector<Continent*> continents;
    
    // Helper methods for graph traversal
    void dfs(Territory* territory, std::vector<bool>& visited);
    void dfsContinent(Continent* continent, Territory* territory, std::vector<bool>& visited);
};

// MapLoader class
class MapLoader {
public:
    static Map* loadMap(const std::string& filename);
    
private:
    static void parseContinents(Map* map, std::ifstream& file);
    static void parseTerritories(Map* map, std::ifstream& file);
};

// Free function for testing
void testLoadMaps();

#endif
