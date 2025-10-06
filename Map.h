#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

// Forward declaration
class Player;

/**
 * @class Territory
 * @brief Represents a territory in the Warzone game map
 */
class Territory {
private:
    std::string name;
    int x, y;
    std::string continent;
    std::vector<Territory*> adjacentTerritories;
    Player* owner;
    int armyUnits;

public:
    // Constructors
    Territory(const std::string& name, int x, int y, const std::string& continent);
    Territory(const Territory& other); // Copy constructor
    Territory& operator=(const Territory& other); // Assignment operator
    
    // Getters and setters
    std::string getName() const;
    int getX() const;
    int getY() const;
    std::string getContinent() const;
    std::vector<Territory*> getAdjacentTerritories() const;
    Player* getOwner() const;
    int getArmyUnits() const;
    
    void setOwner(Player* newOwner);
    void setArmyUnits(int units);
    void addAdjacentTerritory(Territory* territory);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Territory& territory);
};

/**
 * @class Continent
 * @brief Represents a continent containing multiple territories
 */
class Continent {
private:
    std::string name;
    int bonusValue;
    std::vector<Territory*> territories;

public:
    Continent(const std::string& name, int bonusValue);
    Continent(const Continent& other); // Copy constructor
    Continent& operator=(const Continent& other); // Assignment operator
    
    // Getters
    std::string getName() const;
    int getBonusValue() const;
    std::vector<Territory*> getTerritories() const;
    
    void addTerritory(Territory* territory);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Continent& continent);
};

/**
 * @class Map
 * @brief Represents the game map as a connected graph
 */
class Map {
private:
    std::vector<Territory*> territories;
    std::vector<Continent*> continents;

public:
    Map();
    Map(const Map& other); // Copy constructor
    Map& operator=(const Map& other); // Assignment operator
    ~Map();
    
    // Territory and continent management
    void addTerritory(Territory* territory);
    void addContinent(Continent* continent);
    std::vector<Territory*> getTerritories() const;
    std::vector<Continent*> getContinents() const;
    Territory* getTerritoryByName(const std::string& name) const;
    Continent* getContinentByName(const std::string& name) const;
    
    // Validation methods
    bool validate();
    bool isConnectedGraph();
    bool continentsAreConnectedSubgraphs();
    bool territoriesBelongToOneContinent();
    
    // Helper methods for graph traversal
    void depthFirstSearch(Territory* start, std::vector<Territory*>& visited);
    void continentDFS(Territory* start, const std::string& continent, std::vector<Territory*>& visited);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Map& map);
};

/**
 * @class MapLoader
 * @brief Loads map files and creates Map objects
 */
class MapLoader {
public:
    MapLoader();
    MapLoader(const MapLoader& other); // Copy constructor
    MapLoader& operator=(const MapLoader& other); // Assignment operator
    
    Map* loadMap(const std::string& filename);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const MapLoader& loader);
};

#endif // MAP_H
