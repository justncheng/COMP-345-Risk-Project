#include "Map.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Player.h"

//Free Helper Function
bool caseInsenstiveEquals(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (tolower(a[i]) != tolower(b[i])) return false;
    }
    return true;
}

// Territory Implementation
Territory::Territory(const std::string& name, int x, int y, const std::string& continent)
    : name(new std::string(name)), x(x), y(y), continent(new std::string(continent)), 
      owner(nullptr), armies(0) {}

Territory::Territory(const Territory& other)
    : name(new std::string(*other.name)), x(other.x), y(other.y),
      continent(new std::string(*other.continent)), owner(other.owner), armies(other.armies) {
    for (Territory* adj : other.adjacentTerritories) {
        adjacentTerritories.push_back(adj);
    }
}

Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        delete name;
        delete continent;
        
        name = new std::string(*other.name);
        continent = new std::string(*other.continent);
        x = other.x;
        y = other.y;
        owner = other.owner;
        armies = other.armies;
        
        adjacentTerritories.clear();
        for (Territory* adj : other.adjacentTerritories) {
            adjacentTerritories.push_back(adj);
        }
    }
    return *this;
}

Territory::~Territory() {
    delete name;
    delete continent;
}

bool Territory::operator == (const Territory& territory) {
    if ((*name == *territory.name) && (x == territory.x) && (y == territory.y) && (*continent == *territory.continent) && (armies == territory.armies)) {
        if (owner != nullptr && territory.owner != nullptr) {
            if (*owner != *territory.owner) {
                return false;
            }
        }
		else if (owner != territory.owner) { // make sure both are nullptr
			return false;
        }

        if (adjacentTerritories.size() != territory.adjacentTerritories.size()) {
            return false;
        }

        for (int i = 0; i < adjacentTerritories.size(); i++) {
            if (*adjacentTerritories[i] != *territory.adjacentTerritories[i]) {
                return false;
            }
        }
    }
    else {
        return false;
	}

    return true;
}

bool Territory::operator != (const Territory& territory) {
	return !(*this == territory);
}

std::string Territory::getName() const { return *name; }
std::string Territory::getContinent() const { return *continent; }
Player* Territory::getOwner() const { return owner; }
int Territory::getArmies() const { return armies; }

void Territory::setOwner(Player* owner) { this->owner = owner; }
void Territory::setArmies(int armies) { this->armies = armies; }

void Territory::addAdjacentTerritory(Territory* territory) {
    adjacentTerritories.push_back(territory);
}

const std::vector<Territory*>& Territory::getAdjacentTerritories() const {
    return adjacentTerritories;
}

std::ostream& operator<<(std::ostream& os, const Territory& territory) {
    os << *territory.name << " (Continent: " << *territory.continent 
       << ", Armies: " << territory.armies << ")";
    return os;
}

// Continent Implementation
Continent::Continent(const std::string& name, int bonus)
    : name(new std::string(name)), bonus(bonus) {}

Continent::Continent(const Continent& other)
    : name(new std::string(*other.name)), bonus(other.bonus) {
    for (Territory* territory : other.territories) {
        territories.push_back(territory);
    }
}

Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        delete name;
        name = new std::string(*other.name);
        bonus = other.bonus;
        
        territories.clear();
        for (Territory* territory : other.territories) {
            territories.push_back(territory);
        }
    }
    return *this;
}

Continent::~Continent() {
    delete name;
}

std::string Continent::getName() const { return *name; }
int Continent::getBonus() const { return bonus; }

void Continent::addTerritory(Territory* territory) {
    territories.push_back(territory);
}

const std::vector<Territory*>& Continent::getTerritories() const {
    return territories;
}

std::ostream& operator<<(std::ostream& os, const Continent& continent) {
    os << "Continent: " << *continent.name << " (Bonus: " << continent.bonus 
       << ", Territories: " << continent.territories.size() << ")";
    return os;
}

// Map Implementation
Map::Map() = default;

Map::Map(const Map& other) {
    for (Territory* territory : other.territories) {
        territories.push_back(new Territory(*territory));
    }
    for (Continent* continent : other.continents) {
        continents.push_back(new Continent(*continent));
    }
}

Map& Map::operator=(const Map& other) {
    if (this != &other) {
        // Clear existing data
        for (Territory* territory : territories) {
            delete territory;
        }
        for (Continent* continent : continents) {
            delete continent;
        }
        territories.clear();
        continents.clear();
        
        // Copy new data
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

const std::vector<Territory*>& Map::getTerritories() const {
    return territories;
}

const std::vector<Continent*>& Map::getContinents() const {
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

bool Map::validate() {
    return isConnectedGraph() && continentsAreConnectedSubgraphs() && eachCountryInOneContinent();
}

void Map::dfs(Territory* territory, std::vector<bool>& visited) {
    int index = -1;
    for (int i = 0; i < territories.size(); i++) {
        if (territories[i] == territory) {
            index = i;
            break;
        }
    }
    if (index == -1 || visited[index]) return;
    
    visited[index] = true;
    
    for (Territory* adj : territory->getAdjacentTerritories()) {
        int adjIndex = -1;
        for (int i = 0; i < territories.size(); i++) {
            if (territories[i] == adj) {
                adjIndex = i;
                break;
            }
        }
        if (adjIndex != -1 && !visited[adjIndex]) {
            dfs(adj, visited);
        }
    }
}

bool Map::isConnectedGraph() {
    if (territories.empty()) return true;
    
    std::vector<bool> visited(territories.size(), false);
    dfs(territories[0], visited);
    
    for (bool v : visited) {
        if (!v) return false;
    }
    return true;
}

void Map::dfsContinent(Continent* continent, Territory* territory, std::vector<bool>& visited) {
    int index = -1;
    for (int i = 0; i < continent->getTerritories().size(); i++) {
        if (continent->getTerritories()[i] == territory) {
            index = i;
            break;
        }
    }
    if (index == -1 || visited[index]) return;
    
    visited[index] = true;
    
    for (Territory* adj : territory->getAdjacentTerritories()) {
        // Check if adjacent territory is in the same continent
        bool inSameContinent = false;
        for (Territory* contTerritory : continent->getTerritories()) {
            if (contTerritory == adj) {
                inSameContinent = true;
                break;
            }
        }
        
        if (inSameContinent) {
            int adjIndex = -1;
            for (int i = 0; i < continent->getTerritories().size(); i++) {
                if (continent->getTerritories()[i] == adj) {
                    adjIndex = i;
                    break;
                }
            }
            if (adjIndex != -1 && !visited[adjIndex]) {
                dfsContinent(continent, adj, visited);
            }
        }
    }
}

bool Map::continentsAreConnectedSubgraphs() {
    for (Continent* continent : continents) {
        if (continent->getTerritories().empty()) continue;
        
        std::vector<bool> visited(continent->getTerritories().size(), false);
        dfsContinent(continent, continent->getTerritories()[0], visited);
        
        for (bool v : visited) {
            if (!v) return false;
        }
    }
    return true;
}

bool Map::eachCountryInOneContinent() {
    for (Territory* territory : territories) {
        int continentCount = 0;
        for (Continent* continent : continents) {
            for (Territory* contTerritory : continent->getTerritories()) {
                if (contTerritory == territory) {
                    continentCount++;
                    break;
                }
            }
        }
        if (continentCount != 1) return false;
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Map& map) {
    os << "Map with " << map.territories.size() << " territories and " 
       << map.continents.size() << " continents";
    return os;
}

// MapLoader Implementation
Map* MapLoader::loadMap(const std::string& filename) {
    Map* map = new Map();
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        delete map;
        return nullptr;
    }
    
    std::string line;
    bool inContinentsSection = false;
    bool inTerritoriesSection = false;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';') continue;
        
        // Check for section headers
        if (caseInsenstiveEquals(line, "[Continents]")) {
            inContinentsSection = true;
            inTerritoriesSection = false;
            continue;
        } else if (caseInsenstiveEquals(line, "[Territories]")) {
            inContinentsSection = false;
            inTerritoriesSection = true;
            continue;
        } else if (line[0] == '[') {
            // Other sections we don't care about
            inContinentsSection = false;
            inTerritoriesSection = false;
            continue;
        }
        
        if (inContinentsSection) {
            // Parse continent line: Name=Bonus
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string name = line.substr(0, equalsPos);
                int bonus = std::stoi(line.substr(equalsPos + 1));
                map->addContinent(new Continent(name, bonus));
            }
        } else if (inTerritoriesSection) {
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
                for (Continent* cont : map->getContinents()) {
                    if (cont->getName() == continent) {
                        cont->addTerritory(territory);
                        break;
                    }
                }
            }
        }
    }
    
    file.close();
    
    // Second pass to set up adjacencies
    file.open(filename);
    inTerritoriesSection = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') continue;
        
        if (line == "[Territories]") {
            inTerritoriesSection = true;
            continue;
        } else if (line[0] == '[') {
            inTerritoriesSection = false;
            continue;
        }
        
        if (inTerritoriesSection) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                std::string territoryName = tokens[0];
                Territory* territory = map->getTerritoryByName(territoryName);
                
                if (territory) {
                    // Add adjacencies (starting from index 4)
                    for (int i = 4; i < tokens.size(); i++) {
                        Territory* adjacent = map->getTerritoryByName(tokens[i]);
                        if (adjacent) {
                            territory->addAdjacentTerritory(adjacent);
                        }
                    }
                }
            }
        }
    }
    
    file.close();
    return map;
}
