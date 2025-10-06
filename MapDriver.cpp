#include "Map.h"
#include <iostream>

// Test driver that loads several files and runs validation checks.
// The function required by your assignment: testLoadMaps()
void testLoadMaps() {
    std::cout << "=== testLoadMaps() ===\n";

    // Put the filenames you want to test here.
    // Use the exact filenames you have on disk (e.g., "chicago.map", "england.map", "florida.map")
    std::vector<std::string> filenames = {
        "chicago.map",
        "england.map",
        "florida.map"
    };

    for (const auto& fname : filenames) {
        std::cout << "\n--- Loading file: " << fname << " ---\n";
        Map* m = MapLoader::loadMapFromFile(fname);
        if (!m) {
            std::cout << "Failed to open or parse file: " << fname << "\n";
            continue;
        }
        // Print the map content
        std::cout << *m << "\n";

        // Run validation
        std::string msg;
        bool ok = m->validate(msg);
        std::cout << "Validation result: " << (ok ? "VALID" : "INVALID") << " -> " << msg << "\n";

        // Detailed checks (useful for debugging)
        std::cout << " - isConnectedGraph(): " << (m->isConnectedGraph() ? "yes" : "no") << "\n";
        std::cout << " - continentsAreConnected(): " << (m->continentsAreConnected() ? "yes" : "no") << "\n";
        std::cout << " - eachCountryOneContinent(): " << (m->eachCountryOneContinent() ? "yes" : "no") << "\n";

        delete m; // free memory
    }
}

int main() {
    testLoadMaps();
    return 0;
}
