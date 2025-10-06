#pragma once

#include <string>
#include <vector>
#include <iostream>

// Forward declarations
class Continent;
class Territory;
class Map;

/*
 * Territory
 * - name, coordinates (x,y)
 * - pointer to continent it belongs to
 * - adjacency list of pointers to neighboring territories
 */
class Territory {
private:
    std::string name;
    int x;
    int y;
    Continent* continent;                 // pointer to continent (requirement)
    std::vector<Territory*> adjacents;    // adjacency pointers

public:
    Territory(const std::string& name = "", int x = 0, int y = 0);
    Territory(const Territory& other);            // copy constructor
    Territory& operator=(const Territory& other); // assignment operator
    ~Territory();

    const std::string& getName() const;
    int getX() const;
    int getY() const;

    Continent* getContinent() const;
    void setContinent(Continent* c);

    const std::vector<Territory*>& getAdjacents() const;
    void addAdjacent(Territory* t);

    friend std::ostream& operator<<(std::ostream& os, const Territory& t);
};


/*
 * Continent
 * - name and control value
 * - list of territories that belong to this continent (pointers)
 */
class Continent {
private:
    std::string name;
    int controlValue;
    std::vector<Territory*> territories;  // pointers

public:
    Continent(const std::string& name = "", int controlValue = 0);
    Continent(const Continent& other);            // copy constructor
    Continent& operator=(const Continent& other); // assignment operator
    ~Continent();

    const std::string& getName() const;
    int getControlValue() const;
    const std::vector<Territory*>& getTerritories() const;
    void addTerritory(Territory* t);

    friend std::ostream& operator<<(std::ostream& os, const Continent& c);
};


/*
 * Map
 * - collection of territories and continents (stored as pointers)
 * - validation methods
 */
class Map {
private:
    std::vector<Territory*> territories; // pointers
    std::vector<Continent*> continents;  // pointers

public:
    Map();
    Map(const Map& other);            // deep copy
    Map& operator=(const Map& other); // deep assignment (copy-swap idiom)
    ~Map();

    void addTerritory(Territory* t);
    void addContinent(Continent* c);

    const std::vector<Territory*>& getTerritories() const;
    const std::vector<Continent*>& getContinents() const;

    // validate: runs the three checks and returns false if any fail,
    // and an explanatory message in errorMessage.
    bool validate(std::string& errorMessage) const;

    // separate checks (public so driver can call them individually)
    bool isConnectedGraph() const;
    bool continentsAreConnected() const;
    bool eachCountryOneContinent() const;

    friend std::ostream& operator<<(std::ostream& os, const Map& m);
};


/*
 * MapLoader
 * - static loader function that reads a .map file (Conquest format)
 * - returns a dynamically allocated Map* (caller should delete it when done).
 * - the loader is permissive: it will try to read any text file and build
 *   a Map object from what it can (as requested).
 */
class MapLoader {
public:
    static Map* loadMapFromFile(const std::string& filename);
};
