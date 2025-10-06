#include "Map.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

// Territory implementation
Territory::Territory(const std::string& name, int x, int y, const std::string& continent) 
    : name(name), x(x), y(y), continent(continent), owner(nullptr), armyUnits(0) {}

Territory::Territory(const Territory& other) 
    : name(other.name), x(other.x), y(other.y), continent(other.continent), 
      owner(other.owner), armyUnits(other.armyUnits) {
    // Deep copy adjacent territories (pointers only, not the actual territories)
    adjacentTerritories = other.adjacentTerritories;
}

Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        name = other.name;
        x = other.x;
        y = other.y;
        continent = other.continent;
        owner = other.owner;
        armyUnits = other.armyUnits;
        adjacentTerritories = other.adjacentTerritories;
    }
    return *this;
}

std::string Territory::getName() const { return name; }
int Territory::getX() const { return x; }
int Territory::getY() const { return y; }
std::string Territory::getContinent() const { return continent; }
std::vector<Territory*> Territory::getAdjacentTerritories() const { return adjacentTerritories; }
Player* Territory::getOwner() const { return owner; }
int Territory::getArmyUnits() const { return armyUnits; }

void Territory::setOwner(Player* newOwner) { owner = newOwner; }
void Territory::setArmyUnits(int units) { armyUnits = units; }

void Territory::addAdjacentTerritory(Territory* territory) {
    adjacentTerritories.push_back(territory);
}

std::ostream& operator<<(std::ostream& os, const Territory& territory) {
    os << "Territory: " << territory.name 
       << " (Continent: " << territory.continent 
       << ", Position: " << territory.x << "," << territory.y 
       << ", Armies: " << territory.armyUnits << ")";
    return os;
}

// Continent implementation
Continent::Continent(const std::string& name, int bonusValue) 
    : name(name), bonusValue(bonusValue) {}

Continent::Continent(const Continent& other) 
    : name(other.name), bonusValue(other.bonusValue) {
    territories = other.territories;
}

Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        name = other.name;
        bonusValue = other.bonusValue;
        territories = other.territories;
    }
    return *this;
}

std::string Continent::getName() const { return name; }
int Continent::getBonusValue() const { return bonusValue; }
std::vector<Territory*> Continent::getTerritories() const { return territories; }

void Continent::addTerritory(Territory* territory) {
    territories.push_back(territory);
}

std::ostream& operator<<(std::ostream& os, const Continent& continent) {
    os << "Continent: " << continent.name 
       << " (Bonus: " << continent.bonusValue 
       << ", Territories: " << continent.territories.size() << ")";
    return os;
}

// Map implementation
Map::Map() {}

Map::Map(const Map& other) {
    // Deep copy territories and continents
    for (Territory* territory : other.territories) {
        territories.push_back(new Territory(*territory));
    }
    for (Continent* continent : other.continents) {
        continents.push_back(new Continent(*continent));
    }
}

Map& Map::operator=(const Map& other) {
    if (this != &other) {
        // Clean up existing data
        for (Territory* territory : territories) {
            delete territory;
        }
        for (Continent* continent : continents) {
            delete continent;
        }
        territories.clear();
        continents.clear();
        
        // Deep copy
        for (Territory* territory : other.territories) {
            territories.push_back(new Territory(*territory));
        }
        for (Continent* continent : other.continents) {
            continents.push_back(new Continent(*continent));
        }
    }
    return *this;
}

Map::~Map() {
    for (Territory* territory : territories) {
        delete territory;
    }
    for (Continent* continent : continents) {
        delete continent;
    }
}

void Map::addTerritory(Territory* territory) {
    territories.push_back(territory);
}

void Map::addContinent(Continent* continent) {
    continents.push_back(continent);
}

std::vector<Territory*> Map::getTerritories() const {
    return territories;
}

std::vector<Continent*> Map::getContinents() const {
    return continents;
}

Territory* Map::getTerritoryByName(const std::string& name) const {
    for (Territory* territory : territories) {
        if (territory->getName() == name) {
            return territory;
        }
    }
    return nullptr;
}

Continent* Map::getContinentByName(const std::string& name) const {
    for (Continent* continent : continents) {
        if (continent->getName() == name) {
            return continent;
        }
    }
    return nullptr;
}

bool Map::validate() {
    return isConnectedGraph() && continentsAreConnectedSubgraphs() && territoriesBelongToOneContinent();
}

bool Map::isConnectedGraph() {
    if (territories.empty()) return false;
    
    std::vector<Territory*> visited;
    depthFirstSearch(territories[0], visited);
    
    return visited.size() == territories.size();
}

bool Map::continentsAreConnectedSubgraphs() {
    for (Continent* continent : continents) {
        std::vector<Territory*> continentTerritories = continent->getTerritories();
        if (continentTerritories.empty()) continue;
        
        std::vector<Territory*> visited;
        continentDFS(continentTerritories[0], continent->getName(), visited);
        
        if (visited.size() != continentTerritories.size()) {
            return false;
        }
    }
    return true;
}

bool Map::territoriesBelongToOneContinent() {
    for (Territory* territory : territories) {
        int count = 0;
        for (Continent* continent : continents) {
            for (Territory* contTerritory : continent->getTerritories()) {
                if (contTerritory == territory) {
                    count++;
                    break;
                }
            }
        }
        if (count != 1) {
            return false;
        }
    }
    return true;
}

void Map::depthFirstSearch(Territory* start, std::vector<Territory*>& visited) {
    if (std::find(visited.begin(), visited.end(), start) != visited.end()) {
        return;
    }
    
    visited.push_back(start);
    
    for (Territory* adjacent : start->getAdjacentTerritories()) {
        depthFirstSearch(adjacent, visited);
    }
}

void Map::continentDFS(Territory* start, const std::string& continentName, std::vector<Territory*>& visited) {
    if (std::find(visited.begin(), visited.end(), start) != visited.end()) {
        return;
    }
    
    visited.push_back(start);
    
    for (Territory* adjacent : start->getAdjacentTerritories()) {
        if (adjacent->getContinent() == continentName) {
            continentDFS(adjacent, continentName, visited);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Map& map) {
    os << "Map contains:" << std::endl;
    os << "- " << map.continents.size() << " continents" << std::endl;
    os << "- " << map.territories.size() << " territories" << std::endl;
    
    for (Continent* continent : map.continents) {
        os << "  " << *continent << std::endl;
    }
    
    return os;
}

// MapLoader implementation
MapLoader::MapLoader() {}

MapLoader::MapLoader(const MapLoader& other) {
    // Copy constructor - no dynamic data to copy
}

MapLoader& MapLoader::operator=(const MapLoader& other) {
    if (this != &other) {
        // Assignment operator - no dynamic data to copy
    }
    return *this;
}

Map* MapLoader::loadMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return nullptr;
    }
    
    Map* map = new Map();
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';') continue;
        
        // Check for section headers
        if (line[0] == '[') {
            currentSection = line;
            continue;
        }
        
        // Process different sections
        if (currentSection == "[Continents]") {
            // Parse continent line: Name=BonusValue
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string name = line.substr(0, equalPos);
                int bonusValue = std::stoi(line.substr(equalPos + 1));
                map->addContinent(new Continent(name, bonusValue));
            }
        }
        else if (currentSection == "[Territories]") {
            // Parse territory line: Name,X,Y,Continent,Adjacent1,Adjacent2,...
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 4) {
                std::string name = tokens[0];
                int x = std::stoi(tokens[1]);
                int y = std::stoi(tokens[2]);
                std::string continent = tokens[3];
                
                Territory* territory = new Territory(name, x, y, continent);
                map->addTerritory(territory);
                
                // Add territory to its continent
                Continent* cont = map->getContinentByName(continent);
                if (cont) {
                    cont->addTerritory(territory);
                }
            }
        }
    }
    
    file.close();
    
    // Now process adjacencies (we need all territories created first)
    file.open(filename);
    currentSection = "";
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') continue;
        
        if (line[0] == '[') {
            currentSection = line;
            continue;
        }
        
        if (currentSection == "[Territories]") {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                std::string currentTerritoryName = tokens[0];
                Territory* currentTerritory = map->getTerritoryByName(currentTerritoryName);
                
                if (currentTerritory) {
                    // Add adjacent territories (starting from index 4)
                    for (size_t i = 4; i < tokens.size(); i++) {
                        Territory* adjacent = map->getTerritoryByName(tokens[i]);
                        if (adjacent) {
                            currentTerritory->addAdjacentTerritory(adjacent);
                        }
                    }
                }
            }
        }
    }
    
    file.close();
    return map;
}

std::ostream& operator<<(std::ostream& os, const MapLoader& loader) {
    os << "MapLoader object";
    return os;
}
