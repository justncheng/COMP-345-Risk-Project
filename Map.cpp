#include "Map.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <queue>
#include <set>

/* ---------------------------
 * Helper functions
 * ---------------------------
 */
static inline std::string trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace((unsigned char)s[a])) ++a;
    size_t b = s.size();
    while (b > a && std::isspace((unsigned char)s[b - 1])) --b;
    return s.substr(a, b - a);
}

static inline std::vector<std::string> splitByComma(const std::string& s) {
    std::vector<std::string> tokens;
    std::string cur;
    std::istringstream ss(s);
    while (std::getline(ss, cur, ',')) {
        tokens.push_back(trim(cur));
    }
    return tokens;
}

/* ---------------------------
 * Territory implementation
 * ---------------------------
 */
Territory::Territory(const std::string& name_, int x_, int y_)
    : name(name_), x(x_), y(y_), continent(nullptr) {}

Territory::Territory(const Territory& other)
    : name(other.name), x(other.x), y(other.y), continent(nullptr) {
    // Do not copy adjacency pointers or continent pointer here.
    // Graph-level copying is handled in Map's copy constructor (deep copy).
}

Territory& Territory::operator=(const Territory& other) {
    if (this == &other) return *this;
    name = other.name;
    x = other.x;
    y = other.y;
    continent = nullptr;
    adjacents.clear();
    return *this;
}

Territory::~Territory() {
    // Do not delete continent or adjacent pointers here;
    // Map destructor will delete the objects.
}

const std::string& Territory::getName() const { return name; }
int Territory::getX() const { return x; }
int Territory::getY() const { return y; }
Continent* Territory::getContinent() const { return continent; }
void Territory::setContinent(Continent* c) { continent = c; }
const std::vector<Territory*>& Territory::getAdjacents() const { return adjacents; }

void Territory::addAdjacent(Territory* t) {
    if (t == nullptr) return;
    if (t == this) return; // avoid self-loop
    // prevent duplicates
    for (auto existing : adjacents)
        if (existing == t) return;
    adjacents.push_back(t);
}

std::ostream& operator<<(std::ostream& os, const Territory& t) {
    os << "Territory \"" << t.name << "\" (" << t.x << "," << t.y << ")";
    if (t.continent) os << " Continent: " << t.continent->getName();
    else os << " Continent: [none]";
    os << " Adjacent: [";
    for (size_t i = 0; i < t.adjacents.size(); ++i) {
        os << t.adjacents[i]->getName();
        if (i + 1 < t.adjacents.size()) os << ", ";
    }
    os << "]";
    return os;
}


/* ---------------------------
 * Continent implementation
 * ---------------------------
 */
Continent::Continent(const std::string& name_, int cv)
    : name(name_), controlValue(cv) {}

Continent::Continent(const Continent& other)
    : name(other.name), controlValue(other.controlValue) {
    // territories vector is left empty; Map copy will fill it.
}

Continent& Continent::operator=(const Continent& other) {
    if (this == &other) return *this;
    name = other.name;
    controlValue = other.controlValue;
    territories.clear();
    return *this;
}

Continent::~Continent() {
    // Do not delete territories here; Map destructor will handle deletion.
}

const std::string& Continent::getName() const { return name; }
int Continent::getControlValue() const { return controlValue; }
const std::vector<Territory*>& Continent::getTerritories() const { return territories; }
void Continent::addTerritory(Territory* t) {
    if (!t) return;
    for (auto existing : territories) if (existing == t) return;
    territories.push_back(t);
}

std::ostream& operator<<(std::ostream& os, const Continent& c) {
    os << "Continent \"" << c.name << "\" (value=" << c.controlValue << ") Territories: [";
    for (size_t i = 0; i < c.territories.size(); ++i) {
        os << c.territories[i]->getName();
        if (i + 1 < c.territories.size()) os << ", ";
    }
    os << "]";
    return os;
}


/* ---------------------------
 * Map implementation
 * ---------------------------
 */
Map::Map() {}

Map::Map(const Map& other) {
    // Deep copy: 1) create new continent objects, 2) create new territory objects,
    // 3) set each territory's continent, add territories to continent lists,
    // 4) resolve adjacency pointers using mapping from old -> new
    std::unordered_map<Continent*, Continent*> continentMap;
    std::unordered_map<Territory*, Territory*> territoryMap;

    // 1) copy continents
    for (auto oldC : other.continents) {
        Continent* nc = new Continent(*oldC); // uses Continent copy ctor (which copies basic fields)
        continents.push_back(nc);
        continentMap[oldC] = nc;
    }

    // 2) copy territories (no adjacencies yet)
    for (auto oldT : other.territories) {
        Territory* nt = new Territory(*oldT); // copies name, x, y
        territories.push_back(nt);
        territoryMap[oldT] = nt;
    }

    // 3) set continent pointers and populate continent->territories
    for (auto oldT : other.territories) {
        Territory* nt = territoryMap[oldT];
        Continent* oldC = oldT->getContinent();
        if (oldC) {
            Continent* nc = continentMap[oldC];
            nt->setContinent(nc);
            nc->addTerritory(nt);
        } else {
            nt->setContinent(nullptr);
        }
    }

    // 4) resolve adjacency lists
    for (auto oldT : other.territories) {
        Territory* nt = territoryMap[oldT];
        for (auto oldAdj : oldT->getAdjacents()) {
            Territory* mappedAdj = territoryMap[oldAdj];
            if (mappedAdj) nt->addAdjacent(mappedAdj);
        }
    }
}

Map& Map::operator=(const Map& other) {
    if (this == &other) return *this;
    // copy-swap idiom via temporary
    Map tmp(other);
    // swap internals (vectors of pointers)
    territories.swap(tmp.territories);
    continents.swap(tmp.continents);
    // tmp destructor will free old data
    return *this;
}

Map::~Map() {
    // Map owns the continents and territories; delete them
    for (auto t : territories) delete t;
    territories.clear();
    for (auto c : continents) delete c;
    continents.clear();
}

void Map::addTerritory(Territory* t) {
    if (!t) return;
    for (auto existing : territories) if (existing == t) return;
    territories.push_back(t);
}

void Map::addContinent(Continent* c) {
    if (!c) return;
    for (auto existing : continents) if (existing == c) return;
    continents.push_back(c);
}

const std::vector<Territory*>& Map::getTerritories() const { return territories; }
const std::vector<Continent*>& Map::getContinents() const { return continents; }

/* Validation helpers */

/* isConnectedGraph()
 * - pick the first territory and BFS across adjacency pointers
 * - if all territories are visited, graph is connected
 */
bool Map::isConnectedGraph() const {
    if (territories.empty()) return false; // empty map is invalid
    std::set<Territory*> visited;
    std::queue<Territory*> q;
    q.push(territories[0]);
    visited.insert(territories[0]);
    while (!q.empty()) {
        Territory* cur = q.front(); q.pop();
        for (auto nbr : cur->getAdjacents()) {
            if (visited.insert(nbr).second) q.push(nbr);
        }
    }
    return visited.size() == territories.size();
}

/* continentsAreConnected()
 * For each continent, perform BFS but only traverse edges to territories that belong to the same continent.
 */
bool Map::continentsAreConnected() const {
    for (auto cont : continents) {
        const auto& contTerr = cont->getTerritories();
        if (contTerr.empty()) return false; // continent with no territories -> invalid
        std::set<Territory*> visited;
        std::queue<Territory*> q;
        q.push(contTerr[0]);
        visited.insert(contTerr[0]);
        while (!q.empty()) {
            Territory* cur = q.front(); q.pop();
            for (auto nbr : cur->getAdjacents()) {
                if (nbr->getContinent() == cont) {
                    if (visited.insert(nbr).second) q.push(nbr);
                }
            }
        }
        if (visited.size() != contTerr.size()) return false;
    }
    return true;
}

/* eachCountryOneContinent()
 * Ensure every territory has a non-null continent pointer and that a territory appears in exactly one continent's list.
 */
bool Map::eachCountryOneContinent() const {
    std::unordered_map<Territory*, int> cnt;
    for (auto c : continents) {
        for (auto t : c->getTerritories()) {
            cnt[t] += 1;
            if (cnt[t] > 1) return false;
        }
    }
    // check that all territories are present exactly once
    for (auto t : territories) {
        if (cnt.find(t) == cnt.end()) return false; // missing from all continents
        if (cnt[t] != 1) return false; // either 0 or >1
    }
    return true;
}

bool Map::validate(std::string& errorMessage) const {
    if (!isConnectedGraph()) {
        errorMessage = "Map is not a connected graph (some territories are disconnected).";
        return false;
    }
    if (!continentsAreConnected()) {
        errorMessage = "At least one continent is not a connected subgraph.";
        return false;
    }
    if (!eachCountryOneContinent()) {
        errorMessage = "One or more territories do not belong to exactly one continent.";
        return false;
    }
    errorMessage = "Map is valid.";
    return true;
}

std::ostream& operator<<(std::ostream& os, const Map& m) {
    os << "Map: " << m.territories.size() << " territories, " << m.continents.size() << " continents\n";
    os << "Continents:\n";
    for (auto c : m.continents) {
        os << "  " << *c << "\n";
    }
    os << "Territories:\n";
    for (auto t : m.territories) {
        os << "  " << *t << "\n";
    }
    return os;
}


/* ---------------------------
 * MapLoader implementation
 * ---------------------------
 *
 * Must accept Conquest .map style files similar to the examples you gave.
 * We will parse three sections: [Map], [Continents], [Territories]
 * - Continents: lines like "Name=ControlValue"
 * - Territories: lines like "Name,x,y,Continent,Adj1,Adj2,..."
 *
 * Loader is permissive: it will build a Map object from any text file content it can parse.
 */
Map* MapLoader::loadMapFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "MapLoader: cannot open file '" << filename << "'\n";
        return nullptr;
    }

    enum Section { NONE, MAP_SECTION, CONTINENTS_SECTION, TERRITORIES_SECTION };
    Section section = NONE;

    // temporary storages
    std::unordered_map<std::string, Continent*> continentByName;
    std::unordered_map<std::string, Territory*> territoryByName;
    std::unordered_map<std::string, std::vector<std::string>> pendingAdj; // territory name -> adjacency names

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (trimmed.size() > 0 && trimmed[0] == ';') continue; // comment in some map files

        // detect section headers (case-insensitive)
        std::string lower = trimmed;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return std::tolower(c); });
        if (lower == "[map]") { section = MAP_SECTION; continue; }
        if (lower == "[continents]") { section = CONTINENTS_SECTION; continue; }
        if (lower == "[territories]") { section = TERRITORIES_SECTION; continue; }

        if (section == CONTINENTS_SECTION) {
            // expected: Name=Value
            size_t eq = trimmed.find('=');
            if (eq == std::string::npos) continue;
            std::string cname = trim(trimmed.substr(0, eq));
            std::string val = trim(trimmed.substr(eq + 1));
            int control = 0;
            try { control = std::stoi(val); } catch (...) { control = 0; }
            Continent* c = new Continent(cname, control);
            continentByName[cname] = c;
        } else if (section == TERRITORIES_SECTION) {
            // expected: Name,x,y,Continent,Adj1,Adj2,...
            auto tokens = splitByComma(trimmed);
            if (tokens.size() >= 4) {
                std::string tname = tokens[0];
                int tx = 0, ty = 0;
                try { tx = std::stoi(tokens[1]); } catch (...) { tx = 0; }
                try { ty = std::stoi(tokens[2]); } catch (...) { ty = 0; }
                std::string contName = tokens[3];

                Territory* t = new Territory(tname, tx, ty);
                territoryByName[tname] = t;

                // store adjacency names to resolve after the whole file is read
                std::vector<std::string> adjNames;
                for (size_t i = 4; i < tokens.size(); ++i) {
                    if (!tokens[i].empty()) adjNames.push_back(tokens[i]);
                }
                pendingAdj[tname] = adjNames;

                // attach territory to continent if continent exists (if not yet created, resolve later)
                auto itc = continentByName.find(contName);
                if (itc != continentByName.end()) {
                    t->setContinent(itc->second);
                    itc->second->addTerritory(t);
                } else {
                    // continent not created yet - we'll resolve after full read
                    t->setContinent(nullptr);
                }
            }
        } else {
            // We ignore other sections' lines for now (like [Map] metadata)
        }
    }

    // After file read, some territories may refer to continents that were declared after them.
    // Resolve continent pointers for any territory that still has continent == nullptr but has a continent name in pendingAdj keys? Wait: we didn't store per-territory continent name. To keep code simple,
    // we'll do a second pass: try to assign territories to continents by comparing continent names found in continentByName map with territory names' continent if the territory->getContinent() is null.
    // But we did not store the continent name separately. To keep the loader permissive, we will iterate through all continents and try to find territories that have that continent name via string matching of tokens:
    // (Simpler:) We will re-open the file to re-parse territory lines to get continent names. This is not very efficient but simplifies implementation.

    in.clear();
    in.seekg(0);
    section = NONE;
    while (std::getline(in, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        std::string lower = trimmed;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return std::tolower(c); });
        if (lower == "[map]") { section = MAP_SECTION; continue; }
        if (lower == "[continents]") { section = CONTINENTS_SECTION; continue; }
        if (lower == "[territories]") { section = TERRITORIES_SECTION; continue; }

        if (section == TERRITORIES_SECTION) {
            auto tokens = splitByComma(trimmed);
            if (tokens.size() >= 4) {
                std::string tname = tokens[0];
                std::string contName = tokens[3];
                auto itTerr = territoryByName.find(tname);
                if (itTerr != territoryByName.end()) {
                    Territory* t = itTerr->second;
                    auto itc = continentByName.find(contName);
                    if (itc != continentByName.end()) {
                        t->setContinent(itc->second);
                        itc->second->addTerritory(t);
                    }
                    // else leave it null; validate() will catch missing continent membership
                }
            }
        }
    }

    // Now resolve adjacency lists (add edges both ways, avoiding duplicates)
    for (auto& kv : pendingAdj) {
        const std::string& tname = kv.first;
        Territory* t = (territoryByName.count(tname) ? territoryByName[tname] : nullptr);
        if (!t) continue;
        for (const auto& adjName : kv.second) {
            if (territoryByName.count(adjName)) {
                Territory* adj = territoryByName[adjName];
                t->addAdjacent(adj);
                adj->addAdjacent(t); // ensure undirected edge
            } else {
                // adjacency refers to an unknown territory; loader stays permissive - warn and skip
                std::cerr << "MapLoader: in file '" << filename << "' adjacency '" << adjName
                          << "' referenced by '" << tname << "' not found in file.\n";
            }
        }
    }

    // Build Map object
    Map* m = new Map();
    for (auto& kv : continentByName) m->addContinent(kv.second);
    for (auto& kv : territoryByName) m->addTerritory(kv.second);

    return m;
}
