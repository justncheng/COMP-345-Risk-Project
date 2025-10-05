#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

// Forward declaration of Player class
class Player;

/**
 * @class Territory
 * @brief Represents a territory/node in the map graph
 */
class Territory {
private:
    std::string name;                   // Name of the territory
    int armyUnits;                      // Number of army units in territory
    Player* owner;                      // Pointer to player who owns this territory
    std::string continent;              // Continent this territory belongs to
    std::vector<Territory*> adjacentTerritories; // List of adjacent territories

public:
    // Constructors & Destructor
    Territory(const std::string& name, const std::string& continent);
    Territory(const Territory& other);  // Copy constructor
    ~Territory();

    // Assignment operator
    Territory& operator=(const Territory& other);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Territory& territory);

    // Getters
    std::string getName() const;
    int getArmyUnits() const;
    Player* getOwner() const;
    std::string getContinent() const;
    const std::vector<Territory*>& getAdjacentTerritories() const;

    // Setters
    void setArmyUnits(int units);
    void setOwner(Player* player);
    void setContinent(const std::string& continent);

    // Territory operations
    void addAdjacentTerritory(Territory* territory);
    bool isAdjacent(const Territory* territory) const;
};

/**
 * @class Continent
 * @brief Represents a continent as a connected subgraph
 */
class Continent {
private:
    std::string name;                   // Name of the continent
    std::vector<Territory*> territories; // Territories in this continent

public:
    // Constructors & Destructor
    Continent(const std::string& name);
    Continent(const Continent& other);  // Copy constructor
    ~Continent();

    // Assignment operator
    Continent& operator=(const Continent& other);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Continent& continent);

    // Getters
    std::string getName() const;
    const std::vector<Territory*>& getTerritories() const;

    // Continent operations
    void addTerritory(Territory* territory);
    bool containsTerritory(const Territory* territory) const;
    
    // Validation methods
    bool isConnectedSubgraph() const;
};

/**
 * @class Map
 * @brief Represents the game map as a connected graph
 */
class Map {
private:
    std::vector<Territory*> territories; // All territories in the map
    std::vector<Continent*> continents;  // All continents in the map

public:
    // Constructors & Destructor
    Map();
    Map(const Map& other);              // Copy constructor
    ~Map();

    // Assignment operator
    Map& operator=(const Map& other);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Map& map);

    // Getters
    const std::vector<Territory*>& getTerritories() const;
    const std::vector<Continent*>& getContinents() const;

    // Map operations
    void addTerritory(Territory* territory);
    void addContinent(Continent* continent);
    Territory* findTerritory(const std::string& name) const;
    Continent* findContinent(const std::string& name) const;

    // Validation methods
    bool validate() const;
    bool isConnectedGraph() const;
    bool continentsAreConnectedSubgraphs() const;
    bool territoriesBelongToOneContinent() const;
    
private:
    // Helper methods for graph traversal
    void depthFirstSearch(Territory* start, std::vector<bool>& visited) const;
    void depthFirstSearchContinent(Continent* continent, Territory* start, 
                                  std::vector<bool>& visited) const;
};

/**
 * @class MapLoader
 * @brief Loads map files and creates Map objects
 */
class MapLoader {
public:
    // Constructors & Destructor
    MapLoader();
    ~MapLoader();

    // Map loading operations
    Map* loadMap(const std::string& filename);
    bool isValidMapFile(const std::string& filename);

private:
    // Helper methods for parsing map files
    bool parseMapFile(const std::string& filename, Map* map);
    bool parseContinents(std::ifstream& file, Map* map);
    bool parseTerritories(std::ifstream& file, Map* map);
    void createAdjacency(Map* map, const std::vector<std::vector<std::string>>& adjacencyData);
};

// Free function for testing maps
void testLoadMaps();

#endif // MAP_H