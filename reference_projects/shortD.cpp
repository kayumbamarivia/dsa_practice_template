#include <iostream>      // For input/output operations (cin, cout, cerr)
#include <vector>        // For dynamic arrays (std::vector)
#include <string>        // For string manipulation (std::string, std::getline)
#include <fstream>       // For file stream operations (ifstream, ofstream)
#include <sstream>       // For string stream operations (istringstream, ostringstream)
#include <map>           // For associative arrays (std::map)
#include <iomanip>       // For output formatting (std::setw, std::left)
#include <limits>        // For numeric_limits (used in input validation)
#include <algorithm>     // For std::remove_if, std::swap, std::reverse, std::transform
#include <set>           // For std::set (used to track written pairs in graph saving, and visited nodes in DFS)
#include <tuple>         // For std::tuple (used in predefined scenario)
#include <queue>         // For std::priority_queue (used in Dijkstra's algorithm)

// --- Helper Functions ---

/**
 * @brief Trims leading and trailing whitespace from a string.
 * @param str The string to trim.
 * @return The trimmed string.
 */
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str; // No non-whitespace characters found
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief Splits a string by a given delimiter.
 * @param s The string to split.
 * @param delimiter The character to split by.
 * @return A vector of strings, where each element is a token.
 */
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token)); // Trim each token
    }
    return tokens;
}

/**
 * @brief Converts a string to lowercase.
 * @param s The string to convert.
 * @return The lowercase version of the string.
 */
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// --- Location Class ---

/**
 * @brief Represents a single location or district entity in the transport system.
 */
class Location {
public:
    std::string id;          ///< Unique identifier for the location (e.g., D1, P1)
    std::string name;        ///< Name of the location (e.g., "Central District")
    std::string description; ///< Description or type of location (e.g., "Commercial Hub", "Residential Area")
    int population;          ///< Population or estimated daily visitors for the location

    /**
     * @brief Constructor for the Location class.
     * @param id Location ID.
     * @param name Location Name.
     * @param description Location Description/Type.
     * @param population Population/Visitors.
     */
    Location(std::string id = "", std::string name = "", std::string description = "", int population = 0)
        : id(std::move(id)), name(std::move(name)), description(std::move(description)), population(population) {}

    /**
     * @brief Converts the Location object's data into a CSV formatted string.
     * @return A string representing the location in CSV format.
     */
    std::string toCsvString() const {
        std::ostringstream oss;
        oss << id << "," << name << "," << description << "," << population;
        return oss.str();
    }

    /**
     * @brief Static method to create a Location object from a CSV formatted string.
     * @param csvLine The CSV string representing a location.
     * @return A Location object parsed from the CSV string. Returns a default Location if parsing fails.
     */
    static Location fromCsvString(const std::string& csvLine) {
        std::vector<std::string> parts = split(csvLine, ',');
        if (parts.size() == 4) {
            try {
                return Location(parts[0], parts[1], parts[2], std::stoi(parts[3]));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error parsing population number (invalid argument): " << parts[3] << " in line: '" << csvLine << "'\n";
            } catch (const std::out_of_range& e) {
                std::cerr << "Error parsing population number (out of range): " << parts[3] << " in line: '" << csvLine << "'\n";
            }
        } else {
            std::cerr << "Error: Malformed CSV line for Location: '" << csvLine << "' (Expected 4 parts, got " << parts.size() << ")\n";
        }
        return Location(); // Return a default/invalid location if parsing fails
    }
};

// --- RouteDetail Struct ---

/**
 * @brief Represents the details of a route from one location to another.
 * Used within the adjacency list of the graph.
 */
struct RouteDetail {
    std::string targetLocationId; ///< The ID of the location connected to.
    std::string description;      ///< Description of the route (e.g., "expressway", "local road").
    double lengthKm;              ///< Length of the route in kilometers.

    /**
     * @brief Constructor for RouteDetail.
     * @param targetId The ID of the target location.
     * @param desc The description of the route.
     * @param len The length of the route in kilometers.
     */
    RouteDetail(std::string targetId = "", std::string desc = "", double len = 0.0)
        : targetLocationId(std::move(targetId)), description(std::move(desc)), lengthKm(len) {}

    /**
     * @brief Overloads the equality operator to compare two RouteDetail objects.
     * Two routes are considered equal if they have the same target location ID and description.
     * @param other The other RouteDetail object to compare against.
     * @return True if the routes are equal, false otherwise.
     */
    bool operator==(const RouteDetail& other) const {
        return targetLocationId == other.targetLocationId && description == other.description;
    }
};

// --- PathResult Struct ---
/**
 * @brief Stores the result of a pathfinding operation (path and total length).
 */
struct PathResult {
    std::vector<std::string> path; ///< List of location IDs forming the path.
    double totalLength;            ///< Total length of the path in kilometers.

    PathResult() : totalLength(0.0) {}
};

// --- LocationManager Class ---

/**
 * @brief Manages CRUD operations for locations and handles file I/O for locations.csv.
 */
class LocationManager {
private:
    std::vector<Location> locations; ///< Stores all registered locations in memory.
    const std::string filename;      ///< The name of the CSV file for location data.

    /**
     * @brief Loads location data from the CSV file into the 'locations' vector.
     * If the file does not exist, it simply returns without error.
     */
    void loadLocations() {
        locations.clear(); // Clear existing data before loading
        std::ifstream file(filename);
        if (!file.is_open()) {
            // File might not exist yet, which is fine for the first run or if empty
            return;
        }

        std::string line;
        std::getline(file, line); // Read and discard the header line
        while (std::getline(file, line)) {
            if (!line.empty()) {
                locations.push_back(Location::fromCsvString(line));
            }
        }
        file.close();
    }

    /**
     * @brief Saves the current 'locations' vector data to the CSV file.
     * Overwrites the existing file content.
     */
    void saveLocations() const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open '" << filename << "' for writing.\n";
            return;
        }

        file << "ID,Name,Description,Population\n"; // Write CSV header
        for (const auto& l : locations) {
            file << l.toCsvString() << "\n";
        }
        file.close();
    }

public:
    /**
     * @brief Constructor for LocationManager.
     * @param fname The name of the location data file (defaults to "locations.csv").
     */
    LocationManager(const std::string& fname = "locations.csv") : filename(fname) {
        loadLocations(); // Load existing data upon initialization
    }

    /**
     * @brief Adds a new location to the system.
     * Checks for duplicate location IDs before adding.
     * @param newLocation The Location object to add.
     */
    void addLocation(const Location& newLocation) {
        if (newLocation.id.empty() || newLocation.name.empty() || newLocation.description.empty()) {
            std::cout << "Error: All location fields (ID, Name, Description) must not be empty.\n";
            return;
        }
        if (locationExists(newLocation.id)) {
            std::cout << "Error: Location with ID '" << newLocation.id << "' already exists. Please use a unique ID.\n";
            return;
        }
        locations.push_back(newLocation);
        saveLocations(); // Persist changes to file
        std::cout << "Success: Location '" << newLocation.name << "' (ID: " << newLocation.id << ") added.\n";
    }

    /**
     * @brief Displays all registered locations in a tabular format.
     */
    void displayLocations() const {
        if (locations.empty()) {
            std::cout << "No locations registered yet.\n";
            return;
        }

        std::cout << "\n--- Registered Locations ---\n";
        std::cout << std::left << std::setw(10) << "ID"
                  << std::setw(25) << "Name"
                  << std::setw(20) << "Description"
                  << std::setw(15) << "Population" << "\n";
        std::cout << std::string(70, '-') << "\n"; // Separator line

        for (const auto& l : locations) {
            std::cout << std::left << std::setw(10) << l.id
                      << std::setw(25) << l.name
                      << std::setw(20) << l.description
                      << std::setw(15) << l.population << "\n";
        }
        std::cout << std::string(70, '-') << "\n"; // Separator line
    }

    /**
     * @brief Updates the information of an existing location identified by its ID.
     * @param id The ID of the location to update.
     */
    void updateLocation(const std::string& id) {
        if (id.empty()) {
            std::cout << "Error: Location ID cannot be empty.\n";
            return;
        }
        bool found = false;
        for (auto& l : locations) {
            if (l.id == id) {
                found = true;
                std::cout << "Found Location: " << l.name << " (ID: " << l.id << ")\n";
                
                std::string newName, newDescription;
                int newPopulation;

                // Input validation for new name
                while (true) {
                    std::cout << "Enter new Name (current: " << l.name << "): ";
                    std::getline(std::cin, newName);
                    if (newName.empty()) {
                        std::cout << "Error: Name cannot be empty. Please try again.\n";
                    } else {
                        l.name = newName;
                        break;
                    }
                }

                // Input validation for new description
                while (true) {
                    std::cout << "Enter new Description (current: " << l.description << "): ";
                    std::getline(std::cin, newDescription);
                    if (newDescription.empty()) {
                        std::cout << "Error: Description cannot be empty. Please try again.\n";
                    } else {
                        l.description = newDescription;
                        break;
                    }
                }

                // Input validation for new population
                while (true) {
                    std::cout << "Enter new Population (current: " << l.population << "): ";
                    std::cin >> newPopulation;
                    if (std::cin.fail()) {
                        std::cout << "Invalid input. Please enter a number.\n";
                        std::cin.clear(); // Clear error flags
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    } else {
                        l.population = newPopulation;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear remaining buffer
                        break; // Exit loop on valid input
                    }
                }
                saveLocations(); // Persist changes to file
                std::cout << "Success: Location '" << id << "' updated.\n";
                break; // Exit loop after finding and updating
            }
        }
        if (!found) {
            std::cout << "Error: Location ID '" << id << "' not found.\n";
        }
    }

    /**
     * @brief Deletes a location from the system by its ID.
     * Note: This only removes the location from the `locations.csv`.
     * The `RouteManager` must be informed separately to handle associated routes.
     * @param id The ID of the location to delete.
     */
    void deleteLocation(const std::string& id) {
        if (id.empty()) {
            std::cout << "Error: Location ID cannot be empty.\n";
            return;
        }
        // Use std::remove_if to logically remove elements, then erase to physically remove them.
        auto it = std::remove_if(locations.begin(), locations.end(),
                                 [&](const Location& l) { return l.id == id; });

        if (it != locations.end()) {
            locations.erase(it, locations.end()); // Erase the removed element(s)
            saveLocations(); // Persist changes to file
            std::cout << "Success: Location '" << id << "' deleted.\n";
            // Important: RouteManager will be notified by main() to update routes.
        } else {
            std::cout << "Error: Location ID '" << id << "' not found.\n";
        }
    }

    /**
     * @brief Checks if a location with the given ID already exists in the system.
     * @param id The ID to check.
     * @return True if the location exists, false otherwise.
     */
    bool locationExists(const std::string& id) const {
        for (const auto& l : locations) {
            if (l.id == id) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Retrieves a location by its ID.
     * @param id The ID of the location to search for.
     * @return A pointer to the Location object if found, nullptr otherwise.
     */
    Location* searchLocationById(const std::string& id) {
        if (id.empty()) {
            std::cout << "Error: Search ID cannot be empty.\n";
            return nullptr;
        }
        for (auto& l : locations) {
            if (l.id == id) {
                return &l;
            }
        }
        return nullptr;
    }

    /**
     * @brief Searches for locations by name (case-insensitive, partial match).
     * @param nameQuery The name or part of the name to search for.
     * @return A vector of Location objects that match the query.
     */
    std::vector<Location> searchLocationsByName(const std::string& nameQuery) const {
        std::vector<Location> foundLocations;
        if (nameQuery.empty()) {
            std::cout << "Error: Search query cannot be empty.\n";
            return foundLocations;
        }
        std::string lowerQuery = toLower(nameQuery);
        for (const auto& l : locations) {
            std::string lowerName = toLower(l.name);
            if (lowerName.find(lowerQuery) != std::string::npos) {
                foundLocations.push_back(l);
            }
        }
        return foundLocations;
    }

    /**
     * @brief Retrieves a list of all location IDs currently in the system.
     * @return A vector of strings containing all location IDs.
     */
    std::vector<std::string> getAllLocationIds() const {
        std::vector<std::string> ids;
        for (const auto& l : locations) {
            ids.push_back(l.id);
        }
        return ids;
    }
};

// --- RouteManager Class ---

/**
 * @brief Manages the network routes (graph) between locations and handles file I/O for routes.txt.
 */
class RouteManager {
private:
    // Adjacency list: map from source location ID to a vector of its routes.
    // Each route includes the target location ID, description, and length.
    std::map<std::string, std::vector<RouteDetail>> adjacencyList;
    const std::string filename;        ///< The name of the plain text file for route data.
    LocationManager* locationManager;  ///< Pointer to LocationManager for validating location IDs.

    /**
     * @brief Loads route data from the plain text file into the 'adjacencyList'.
     * The file format is: SourceLocationID,DestinationLocationID,Description,Length_km
     */
    void loadRoutes() {
        adjacencyList.clear(); // Clear existing data before loading
        std::ifstream file(filename);
        if (!file.is_open()) {
            // File might not exist yet, which is fine for the first run
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::vector<std::string> parts = split(line, ',');
            if (parts.size() == 4) { // Expecting Source, Dest, Desc, Len
                std::string loc1_id = parts[0];
                std::string loc2_id = parts[1];
                std::string desc = parts[2];
                double length;
                try {
                    length = std::stod(parts[3]);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Warning: Invalid length '" << parts[3] << "' in line: '" << line << "'. Skipping.\n";
                    continue;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Warning: Length out of range '" << parts[3] << "' in line: '" << line << "'. Skipping.\n";
                    continue;
                }

                // Add bidirectional route to in-memory adjacency list.
                // Validation of location existence is done when adding via menu,
                // but for loading, we just populate the graph. Invalid IDs will
                // show up as disconnected nodes or be handled by display logic.
                addRouteInternal(loc1_id, loc2_id, desc, length);
            } else {
                std::cerr << "Warning: Malformed line in '" << filename << "': '" << line << "' (Expected 4 parts, got " << parts.size() << "). Skipping.\n";
            }
        }
        file.close();
    }

    /**
     * @brief Saves the current 'adjacencyList' data to the plain text file.
     * To avoid writing duplicate bidirectional links (e.g., D1,D2 and D2,D1),
     * it uses a set to keep track of pairs already written in a canonical order.
     */
    void saveRoutes() const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open '" << filename << "' for writing.\n";
            return;
        }

        // Use a set to store pairs of location IDs (in canonical order) that have already been written
        // to prevent writing both directions of a bidirectional link to the file.
        std::set<std::pair<std::string, std::string>> writtenPairs;

        for (const auto& entry : adjacencyList) {
            const std::string& sourceId = entry.first;
            for (const auto& route : entry.second) {
                const std::string& targetId = route.targetLocationId;

                // Create a canonical pair (lexicographically smallest ID first)
                std::string id1 = sourceId;
                std::string id2 = targetId;
                if (id1 > id2) std::swap(id1, id2); // Ensure id1 is always <= id2

                // If this pair hasn't been written yet, write it and add to the set
                if (writtenPairs.find({id1, id2}) == writtenPairs.end()) {
                    file << sourceId << "," << targetId << "," << route.description << "," << route.lengthKm << "\n";
                    writtenPairs.insert({id1, id2});
                }
            }
        }
        file.close();
    }

    /**
     * @brief Internal helper to add a bidirectional route to the 'adjacencyList' in memory.
     * Does not perform validation or file saving.
     * @param loc1_id The ID of the first location.
     * @param loc2_id The ID of the second location.
     * @param desc The description of the route.
     * @param length The length of the route in kilometers.
     */
    void addRouteInternal(const std::string& loc1_id, const std::string& loc2_id, const std::string& desc, double length) {
        // Add route from Loc1 to Loc2
        adjacencyList[loc1_id].emplace_back(loc2_id, desc, length);
        // Add route from Loc2 to Loc1 (for bidirectionality)
        adjacencyList[loc2_id].emplace_back(loc1_id, desc, length);
    }

    /**
     * @brief Internal helper to remove a bidirectional route from the 'adjacencyList' in memory.
     * Does not perform validation or file saving.
     * @param loc1_id The ID of the first location.
     * @param loc2_id The ID of the second location.
     * @param desc The description of the route to remove.
     */
    void removeRouteInternal(const std::string& loc1_id, const std::string& loc2_id, const std::string& desc) {
        // Remove Loc1 -> Loc2 route
        auto& routes1 = adjacencyList[loc1_id];
        routes1.erase(std::remove_if(routes1.begin(), routes1.end(),
                                    [&](const RouteDetail& c) {
                                        return c.targetLocationId == loc2_id && c.description == desc;
                                    }),
                      routes1.end());

        // Remove Loc2 -> Loc1 route
        auto& routes2 = adjacencyList[loc2_id];
        routes2.erase(std::remove_if(routes2.begin(), routes2.end(),
                                    [&](const RouteDetail& c) {
                                        return c.targetLocationId == loc1_id && c.description == desc;
                                    }),
                      routes2.end());
    }

    /**
     * @brief Recursive helper function for finding the longest simple path using DFS.
     * @param currentId The ID of the current location.
     * @param endId The ID of the destination location.
     * @param currentPath The current path of location IDs.
     * @param currentLength The current length of the path.
     * @param visited A set of visited location IDs to ensure a simple path.
     * @param longestPathResult Reference to the PathResult object to store the longest path found.
     */
    void findLongestRouteDFS(const std::string& currentId, const std::string& endId,
                             std::vector<std::string>& currentPath, double currentLength,
                             std::set<std::string>& visited, PathResult& longestPathResult) const {
        currentPath.push_back(currentId);
        visited.insert(currentId);

        if (currentId == endId) {
            if (currentLength > longestPathResult.totalLength) {
                longestPathResult.totalLength = currentLength;
                longestPathResult.path = currentPath;
            }
        } else {
            auto it = adjacencyList.find(currentId);
            if (it != adjacencyList.end()) {
                for (const auto& route : it->second) {
                    if (visited.find(route.targetLocationId) == visited.end()) { // If not visited (ensures simple path)
                        findLongestRouteDFS(route.targetLocationId, endId, currentPath,
                                            currentLength + route.lengthKm, visited, longestPathResult);
                    }
                }
            }
        }

        // Backtrack: remove current node from path and visited set
        visited.erase(currentId);
        currentPath.pop_back();
    }


public:
    /**
     * @brief Constructor for RouteManager.
     * @param lm A pointer to the LocationManager instance (for ID validation).
     * @param fname The name of the route data file (defaults to "routes.txt").
     */
    RouteManager(LocationManager* lm, const std::string& fname = "routes.txt")
        : filename(fname), locationManager(lm) {
        loadRoutes(); // Load existing graph data upon initialization
    }

    /**
     * @brief Adds a new bidirectional route between two locations.
     * Validates that both location IDs exist and prevents duplicate routes.
     * @param loc1_id The ID of the first location.
     * @param loc2_id The ID of the second location.
     * @param desc The description of the route.
     * @param length The length of the route in kilometers.
     */
    void addRoute(const std::string& loc1_id, const std::string& loc2_id, const std::string& desc, double length) {
        if (loc1_id.empty() || loc2_id.empty() || desc.empty()) {
            std::cout << "Error: All route fields (Location IDs, Description) must not be empty.\n";
            return;
        }
        // Validate location IDs
        if (!locationManager->locationExists(loc1_id)) {
            std::cout << "Error: Start location ID '" << loc1_id << "' not found. Cannot create route.\n";
            return;
        }
        if (!locationManager->locationExists(loc2_id)) {
            std::cout << "Error: End location ID '" << loc2_id << "' not found. Cannot create route.\n";
            return;
        }
        if (loc1_id == loc2_id) {
            std::cout << "Error: Cannot create a route from a location to itself.\n";
            return;
        }

        // Check for existing route with the same description
        // We only need to check one direction (e.g., Loc1 -> Loc2) because it's bidirectional
        for (const auto& route : adjacencyList[loc1_id]) {
            if (route.targetLocationId == loc2_id && route.description == desc) {
                std::cout << "Error: Route '" << desc << "' between " << loc1_id << " and " << loc2_id << " already exists.\n";
                return;
            }
        }

        addRouteInternal(loc1_id, loc2_id, desc, length);
        saveRoutes(); // Persist changes to file
        std::cout << "Success: Bidirectional route between " << loc1_id << " and " << loc2_id
                  << " with description '" << desc << "' (Length: " << length << " km) added.\n";
    }

    /**
     * @brief Displays all location relationships (routes) in a tabular format.
     * Includes locations even if they have no routes.
     */
    void displayRelationships() const {
        std::cout << "\n--- Location Routes ---\n";
        std::cout << std::left << std::setw(20) << "Source Location"
                  << std::setw(25) << "Connected Locations"
                  << std::setw(40) << "Route Description" << "\n"; // Increased width for description
        std::cout << std::string(85, '-') << "\n"; // Separator line

        std::vector<std::string> allLocationIds = locationManager->getAllLocationIds();
        if (allLocationIds.empty()) {
            std::cout << "No locations registered, so no routes to display.\n";
            return;
        }

        for (const std::string& locationId : allLocationIds) {
            bool firstRoute = true;
            auto it = adjacencyList.find(locationId); // Find routes for this location
            
            // Check if location exists in adjacency list and has routes
            if (it != adjacencyList.end() && !it->second.empty()) {
                // Sort routes by target location ID for consistent display
                std::vector<RouteDetail> sortedRoutes = it->second;
                std::sort(sortedRoutes.begin(), sortedRoutes.end(),
                          [](const RouteDetail& a, const RouteDetail& b) {
                              return a.targetLocationId < b.targetLocationId;
                          });

                for (const auto& route : sortedRoutes) {
                    if (firstRoute) {
                        std::cout << std::left << std::setw(20) << locationId;
                        firstRoute = false;
                    } else {
                        std::cout << std::left << std::setw(20) << ""; // Empty for subsequent lines for the same source location
                    }
                    std::cout << std::setw(25) << route.targetLocationId
                              << std::setw(40) << (route.description + " (" + std::to_string(route.lengthKm) + " km)") << "\n";
                }
            } else {
                // Location has no routes, display "N/A"
                std::cout << std::left << std::setw(20) << locationId
                          << std::setw(25) << "N/A"
                          << std::setw(40) << "No direct routes" << "\n";
            }
        }
        std::cout << std::string(85, '-') << "\n"; // Separator line
    }

    /**
     * @brief Updates the description and/or length of an existing route.
     * Requires the IDs of both locations and the old description to identify the route.
     * @param loc1_id The ID of the first location in the route.
     * @param loc2_id The ID of the second location in the route.
     * @param old_desc The current description of the route.
     * @param new_desc The new description for the route.
     * @param new_length The new length for the route in kilometers.
     */
    void updateRoute(const std::string& loc1_id, const std::string& loc2_id,
                          const std::string& old_desc, const std::string& new_desc, double new_length) {
        if (loc1_id.empty() || loc2_id.empty() || old_desc.empty() || new_desc.empty()) {
            std::cout << "Error: All route fields (Location IDs, Descriptions) must not be empty.\n";
            return;
        }
        if (!locationManager->locationExists(loc1_id) || !locationManager->locationExists(loc2_id)) {
            std::cout << "Error: One or both location IDs not found. Cannot update route.\n";
            return;
        }

        bool found = false;
        // Update Loc1 -> Loc2 direction
        auto it1 = adjacencyList.find(loc1_id);
        if (it1 != adjacencyList.end()) {
            for (auto& route : it1->second) {
                if (route.targetLocationId == loc2_id && route.description == old_desc) {
                    route.description = new_desc;
                    route.lengthKm = new_length;
                    found = true;
                    break;
                }
            }
        }

        // Update Loc2 -> Loc1 direction
        auto it2 = adjacencyList.find(loc2_id);
        if (it2 != adjacencyList.end()) {
            for (auto& route : it2->second) {
                if (route.targetLocationId == loc1_id && route.description == old_desc) {
                    route.description = new_desc;
                    route.lengthKm = new_length;
                    found = true; // Still true, but ensures both sides are updated
                    break;
                }
            }
        }

        if (found) {
            saveRoutes(); // Persist changes to file
            std::cout << "Success: Route between " << loc1_id << " and " << loc2_id
                      << " (description '" << old_desc << "') updated to '" << new_desc
                      << "' (Length: " << new_length << " km).\n";
        } else {
            std::cout << "Error: Connection with description '" << old_desc << "' between "
                      << loc1_id << " and " << loc2_id << " not found.\n";
        }
    }

    /**
     * @brief Deletes a specific route between two locations by its description.
     * @param loc1_id The ID of the first location in the route.
     * @param loc2_id The ID of the second location in the route.
     * @param desc The description of the route to delete.
     */
    void deleteRoute(const std::string& loc1_id, const std::string& loc2_id, const std::string& desc) {
        if (loc1_id.empty() || loc2_id.empty() || desc.empty()) {
            std::cout << "Error: All route fields (Location IDs, Description) must not be empty.\n";
            return;
        }
        if (!locationManager->locationExists(loc1_id) || !locationManager->locationExists(loc2_id)) {
            std::cout << "Error: One or both location IDs not found. Cannot delete route.\n";
            return;
        }

        // Check if the route actually exists before attempting removal
        bool exists = false;
        auto it_loc1 = adjacencyList.find(loc1_id);
        if (it_loc1 != adjacencyList.end()) {
            for (const auto& route : it_loc1->second) {
                if (route.targetLocationId == loc2_id && route.description == desc) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            std::cout << "Error: Route with description '" << desc << "' between "
                      << loc1_id << " and " << loc2_id << " not found.\n";
            return;
        }

        // Remove the route from both directions in the adjacency list
        removeRouteInternal(loc1_id, loc2_id, desc);
        saveRoutes(); // Persist changes to file
        std::cout << "Success: Bidirectional route with description '" << desc << "' between "
                  << loc1_id << " and " << loc2_id << " deleted.\n";
    }

    /**
     * @brief Handles the scenario when a location is deleted from the system.
     * Removes all routes involving the deleted location.
     * @param deletedLocationId The ID of the location that was deleted.
     */
    void handleLocationDeletion(const std::string& deletedLocationId) {
        // 1. Remove all entries where deletedLocationId is the source
        adjacencyList.erase(deletedLocationId);

        // 2. Iterate through all other locations and remove routes where deletedLocationId is the target
        for (auto& entry : adjacencyList) {
            auto& routes = entry.second; // Get the vector of routes for the current source location
            // Use erase-remove idiom to remove elements matching the condition
            routes.erase(std::remove_if(routes.begin(), routes.end(),
                                             [&](const RouteDetail& c) {
                                                 return c.targetLocationId == deletedLocationId;
                                             }),
                              routes.end());
        }
        saveRoutes(); // Persist changes after removing routes
        std::cout << "Info: All routes involving location '" << deletedLocationId << "' have been removed.\n";
    }

    /**
     * @brief Exports the location relationship table to a new CSV file.
     * The output format is: Source Location,Connected Locations,Description
     * @param exportFilename The name of the CSV file to export to (defaults to "route_relationships.csv").
     */
    void exportRelationshipsToCsv(const std::string& exportFilename = "route_relationships.csv") const {
        std::ofstream file(exportFilename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open '" << exportFilename << "' for writing.\n";
            return;
        }

        file << "Source Location,Connected Locations,Description\n"; // Write CSV header

        std::vector<std::string> allLocationIds = locationManager->getAllLocationIds();
        // Use a set to store pairs of location IDs (in canonical order) that have already been written
        // to prevent writing both directions of a bidirectional link to the CSV.
        std::set<std::pair<std::string, std::string>> writtenPairs;

        for (const std::string& locationId : allLocationIds) {
            auto it = adjacencyList.find(locationId);
            if (it != adjacencyList.end()) {
                // Sort routes for consistent output
                std::vector<RouteDetail> sortedRoutes = it->second;
                std::sort(sortedRoutes.begin(), sortedRoutes.end(),
                          [](const RouteDetail& a, const RouteDetail& b) {
                              return a.targetLocationId < b.targetLocationId;
                          });

                for (const auto& route : sortedRoutes) {
                    // Create a canonical pair (lexicographically smallest ID first)
                    std::string id1 = locationId;
                    std::string id2 = route.targetLocationId;
                    if (id1 > id2) std::swap(id1, id2);

                    // If this pair hasn't been written yet, write it and add to the set
                    if (writtenPairs.find({id1, id2}) == writtenPairs.end()) {
                        // Quote the description field in case it contains commas or spaces
                        file << locationId << ","
                             << route.targetLocationId << ","
                             << "\"" << route.description << " (" << route.lengthKm << " km)\"\n";
                        writtenPairs.insert({id1, id2});
                    }
                }
            }
        }
        file.close();
        std::cout << "Success: Route table exported to '" << exportFilename << "'.\n";
    }

    /**
     * @brief Finds the shortest route between two locations using Dijkstra's algorithm.
     * @param startId The ID of the starting location.
     * @param endId The ID of the destination location.
     * @return A PathResult object containing the shortest path and its total length.
     */
    PathResult findShortestRoute(const std::string& startId, const std::string& endId) const {
        PathResult result;
        if (startId.empty() || endId.empty()) {
            std::cout << "Error: Start and end location IDs cannot be empty.\n";
            return result;
        }
        if (!locationManager->locationExists(startId)) {
            std::cout << "Error: Start location ID '" << startId << "' not found.\n";
            return result;
        }
        if (!locationManager->locationExists(endId)) {
            std::cout << "Error: End location ID '" << endId << "' not found.\n";
            return result;
        }
        if (startId == endId) {
            result.path.push_back(startId);
            result.totalLength = 0.0;
            std::cout << "Shortest route from " << startId << " to " << endId << ": " << startId << " (Length: 0 km)\n";
            return result;
        }

        // Dijkstra's algorithm implementation
        std::map<std::string, double> distances;
        std::map<std::string, std::string> predecessors;
        // Priority queue: stores pairs of (distance, location_id)
        // std::greater<std::pair<double, std::string>> makes it a min-priority queue
        std::priority_queue<std::pair<double, std::string>,
                            std::vector<std::pair<double, std::string>>,
                            std::greater<std::pair<double, std::string>>> pq;

        // Initialize distances
        for (const auto& entry : adjacencyList) {
            distances[entry.first] = std::numeric_limits<double>::infinity();
        }
        // Also initialize distances for locations that might not have any outgoing routes
        for (const auto& locId : locationManager->getAllLocationIds()) {
            if (distances.find(locId) == distances.end()) {
                distances[locId] = std::numeric_limits<double>::infinity();
            }
        }

        distances[startId] = 0.0;
        pq.push({0.0, startId});

        while (!pq.empty()) {
            double current_dist = pq.top().first;
            std::string current_loc = pq.top().second;
            pq.pop();

            // If we've already found a shorter path to current_loc, skip
            if (current_dist > distances[current_loc]) {
                continue;
            }

            // If current_loc is the endId, we found the shortest path
            if (current_loc == endId) {
                break;
            }

            auto it = adjacencyList.find(current_loc);
            if (it != adjacencyList.end()) {
                for (const auto& route : it->second) {
                    std::string neighbor = route.targetLocationId;
                    double weight = route.lengthKm;

                    if (distances[current_loc] + weight < distances[neighbor]) {
                        distances[neighbor] = distances[current_loc] + weight;
                        predecessors[neighbor] = current_loc;
                        pq.push({distances[neighbor], neighbor});
                    }
                }
            }
        }

        // Reconstruct path
        if (distances[endId] == std::numeric_limits<double>::infinity()) {
            std::cout << "No route found from " << startId << " to " << endId << ".\n";
            return result;
        }

        std::string current = endId;
        while (current != startId) {
            result.path.push_back(current);
            current = predecessors[current];
        }
        result.path.push_back(startId);
        std::reverse(result.path.begin(), result.path.end());
        result.totalLength = distances[endId];

        std::cout << "Shortest route from " << startId << " to " << endId << ": ";
        for (size_t i = 0; i < result.path.size(); ++i) {
            std::cout << result.path[i] << (i == result.path.size() - 1 ? "" : " -> ");
        }
        std::cout << " (Length: " << result.totalLength << " km)\n";
        return result;
    }

    /**
     * @brief Finds the longest simple route between two locations using a DFS-based approach.
     * A simple path means no repeated locations.
     * @param startId The ID of the starting location.
     * @param endId The ID of the destination location.
     * @return A PathResult object containing the longest simple path and its total length.
     */
    PathResult findLongestRoute(const std::string& startId, const std::string& endId) const {
        PathResult longestPathResult; // Stores the best path found
        longestPathResult.totalLength = -1.0; // Initialize with an impossible length

        if (startId.empty() || endId.empty()) {
            std::cout << "Error: Start and end location IDs cannot be empty.\n";
            return longestPathResult;
        }
        if (!locationManager->locationExists(startId)) {
            std::cout << "Error: Start location ID '" << startId << "' not found.\n";
            return longestPathResult;
        }
        if (!locationManager->locationExists(endId)) {
            std::cout << "Error: End location ID '" << endId << "' not found.\n";
            return longestPathResult;
        }
        if (startId == endId) {
            longestPathResult.path.push_back(startId);
            longestPathResult.totalLength = 0.0;
            std::cout << "Longest route from " << startId << " to " << endId << ": " << startId << " (Length: 0 km)\n";
            return longestPathResult;
        }

        std::vector<std::string> currentPath;
        std::set<std::string> visited;
        double currentLength = 0.0;

        // Call the recursive DFS helper
        findLongestRouteDFS(startId, endId, currentPath, currentLength, visited, longestPathResult);

        if (longestPathResult.totalLength == -1.0) { // No path found
            std::cout << "No simple route found from " << startId << " to " << endId << ".\n";
        } else {
            std::cout << "Longest simple route from " << startId << " to " << endId << ": ";
            for (size_t i = 0; i < longestPathResult.path.size(); ++i) {
                std::cout << longestPathResult.path[i] << (i == longestPathResult.path.size() - 1 ? "" : " -> ");
            }
            std::cout << " (Length: " << longestPathResult.totalLength << " km)\n";
        }
        return longestPathResult;
    }

    /**
     * @brief Searches for routes based on location IDs or description.
     * @param query The search string.
     * @param searchByDescription If true, searches by route description.
     * @param searchByLocationId If true, searches by source or target location ID.
     */
    void searchRoutes(const std::string& query, bool searchByDescription, bool searchByLocationId) const {
        if (query.empty()) {
            std::cout << "Error: Search query cannot be empty.\n";
            return;
        }
        std::vector<std::tuple<std::string, std::string, RouteDetail>> foundRoutes;
        std::set<std::pair<std::string, std::string>> addedPairs; // To avoid duplicate bidirectional display

        std::string lowerQuery = toLower(query);

        for (const auto& entry : adjacencyList) {
            const std::string& sourceId = entry.first;
            for (const auto& route : entry.second) {
                std::string lowerDesc = toLower(route.description);
                std::string lowerTargetId = toLower(route.targetLocationId);
                std::string lowerSourceId = toLower(sourceId);

                bool match = false;
                if (searchByDescription && lowerDesc.find(lowerQuery) != std::string::npos) {
                    match = true;
                }
                if (searchByLocationId && (lowerSourceId.find(lowerQuery) != std::string::npos || lowerTargetId.find(lowerQuery) != std::string::npos)) {
                    match = true;
                }

                if (match) {
                    // Canonical pair for uniqueness
                    std::string id1 = sourceId;
                    std::string id2 = route.targetLocationId;
                    if (id1 > id2) std::swap(id1, id2);

                    if (addedPairs.find({id1, id2}) == addedPairs.end()) {
                        foundRoutes.emplace_back(sourceId, route.targetLocationId, route);
                        addedPairs.insert({id1, id2});
                    }
                }
            }
        }

        if (foundRoutes.empty()) {
            std::cout << "No routes found matching '" << query << "'.\n";
            return;
        }

        std::cout << "\n--- Found Routes ---\n";
        std::cout << std::left << std::setw(20) << "Source Location"
                  << std::setw(25) << "Connected Locations"
                  << std::setw(40) << "Route Description" << "\n";
        std::cout << std::string(85, '-') << "\n";

        for (const auto& item : foundRoutes) {
            const std::string& source = std::get<0>(item);
            const std::string& target = std::get<1>(item);
            const RouteDetail& route = std::get<2>(item);
            std::cout << std::left << std::setw(20) << source
                      << std::setw(25) << target
                      << std::setw(40) << (route.description + " (" + std::to_string(route.lengthKm) + " km)") << "\n";
        }
        std::cout << std::string(85, '-') << "\n";
    }
};

// --- Main Program Menu and Input Handling Functions ---

/**
 * @brief Prompts the user for a string input and validates it to be non-empty.
 * @param prompt The message to display to the user.
 * @return A non-empty string entered by the user.
 */
std::string getStringInput(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        value = trim(value); // Trim whitespace
        if (value.empty()) {
            std::cout << "Error: Input cannot be empty. Please try again.\n";
        } else {
            return value;
        }
    }
}

/**
 * @brief Displays the main menu options to the user.
 */
void displayMainMenu() {
    std::cout << "\n--- Transport Management System ---\n";
    std::cout << "1. Location Management (CRUD)\n";
    std::cout << "2. Route Management (Graph)\n";
    std::cout << "3. Predefined Scenario Setup\n";
    std::cout << "4. Export Route Relationships to CSV\n";
    std::cout << "5. Exit\n";
    std::cout << "Enter your choice: ";
}

/**
 * @brief Displays the menu options for Location Management.
 */
void displayLocationMenu() {
    std::cout << "\n--- Location Management ---\n";
    std::cout << "1. Add Location\n";
    std::cout << "2. Display All Locations\n";
    std::cout << "3. Update Location\n";
    std::cout << "4. Delete Location\n";
    std::cout << "5. Search Location by ID\n";   // New search option
    std::cout << "6. Search Location by Name\n"; // New search option
    std::cout << "7. Back to Main Menu\n";
    std::cout << "Enter your choice: ";
}

/**
 * @brief Displays the menu options for Route Management.
 */
void displayRouteMenu() {
    std::cout << "\n--- Route Management ---\n";
    std::cout << "1. Add Route\n";
    std::cout << "2. Display All Routes\n";
    std::cout << "3. Update Route\n";
    std::cout << "4. Delete Route\n";
    std::cout << "5. Find Shortest Route\n";
    std::cout << "6. Find Longest Route\n";
    std::cout << "7. Search Routes\n"; // New search option
    std::cout << "8. Back to Main Menu\n";
    std::cout << "Enter your choice: ";
}

/**
 * @brief Prompts the user for an integer input and validates it.
 * @param prompt The message to display to the user.
 * @return A valid integer entered by the user.
 */
int getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) { // Check if input failed (e.g., non-numeric)
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear remaining buffer
            return value; // Return valid input
        }
    }
}

/**
 * @brief Prompts the user for a double input and validates it.
 * @param prompt The message to display to the user.
 * @return A valid double entered by the user.
 */
double getDoubleInput(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) { // Check if input failed (e.g., non-numeric)
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear remaining buffer
            return value; // Return valid input
        }
    }
}

/**
 * @brief Sets up a predefined scenario with initial locations and routes.
 * This function allows users to manually enter the setup via the menu system.
 * @param lm Reference to the LocationManager instance.
 * @param rm Reference to the RouteManager instance.
 */
void setupPredefinedScenario(LocationManager& lm, RouteManager& rm) {
    std::cout << "\n--- Setting up Predefined Scenario ---\n";

    // Predefined Locations (Districts and Places)
    std::vector<Location> predefinedLocations = {
        Location("D1", "Central District", "Commercial Hub", 50000),
        Location("D2", "Residential Zone A", "High-Density Housing", 30000),
        Location("P1", "Market Square", "Shopping & Leisure", 10000),
        Location("P2", "Industrial Park", "Manufacturing Area", 5000),
        Location("D3", "Green Valley", "Suburban Area", 20000),
        Location("P3", "Airport Terminal", "Transportation Hub", 15000)
    };

    // Add predefined locations if they don't already exist
    for (const auto& l : predefinedLocations) {
        if (!lm.locationExists(l.id)) {
            lm.addLocation(l);
        } else {
            std::cout << "Info: Location " << l.id << " already exists. Skipping addition.\n";
        }
    }

    // Predefined Routes (with sensible lengths)
    // Using std::tuple to store (Loc1_ID, Loc2_ID, Description, Length_KM)
    std::vector<std::tuple<std::string, std::string, std::string, double>> predefinedRoutes = {
        {"D1", "D2", "Main Boulevard", 8.5},
        {"D1", "P1", "City Center Access", 2.1},
        {"D2", "P2", "Industrial Link Road", 12.0},
        {"P1", "P3", "Expressway Connector", 20.0},
        {"D3", "P1", "Scenic Route", 7.5}
    };

    // Add predefined routes
    for (const auto& route : predefinedRoutes) {
        // The addRoute method handles bidirectionality and checks for duplicates.
        rm.addRoute(std::get<0>(route), std::get<1>(route), std::get<2>(route), std::get<3>(route));
    }
    std::cout << "Predefined scenario setup complete.\n";
}

// --- Main Function ---

/**
 * @brief The entry point of the Transport Management System program.
 * Initializes managers and runs the main menu loop.
 */
int main() {
    // Create instances of LocationManager and RouteManager
    LocationManager locationManager;
    // Pass the address of locationManager to routeManager for location ID validation
    RouteManager routeManager(&locationManager);

    int choice;
    std::string id, name, description, loc1_id, loc2_id;
    int population;
    double length_km;

    // Main program loop
    while (true) {
        displayMainMenu();
        choice = getIntInput(""); // Get main menu choice

        switch (choice) {
            case 1: // Location Management
                while (true) {
                    displayLocationMenu();
                    choice = getIntInput(""); // Get location menu choice
                    switch (choice) {
                        case 1: // Add Location
                            id = getStringInput("Enter Location ID (e.g., D1, P1): ");
                            name = getStringInput("Enter Location Name: ");
                            description = getStringInput("Enter Description (e.g., Commercial Hub, Residential Area): ");
                            population = getIntInput("Enter Population/Daily Visitors: "); // Robust integer input
                            locationManager.addLocation(Location(id, name, description, population));
                            break;
                        case 2: // Display All Locations
                            locationManager.displayLocations();
                            break;
                        case 3: // Update Location
                            id = getStringInput("Enter Location ID to update: ");
                            locationManager.updateLocation(id);
                            break;
                        case 4: // Delete Location
                            id = getStringInput("Enter Location ID to delete: ");
                            if (locationManager.locationExists(id)) {
                                locationManager.deleteLocation(id);
                                // Crucial: Inform routeManager that a location has been deleted
                                routeManager.handleLocationDeletion(id);
                            } else {
                                std::cout << "Error: Location ID '" << id << "' not found. Cannot delete.\n";
                            }
                            break;
                        case 5: { // Search Location by ID
                            std::string searchId = getStringInput("Enter Location ID to search: ");
                            Location* foundLoc = locationManager.searchLocationById(searchId);
                            if (foundLoc) {
                                std::cout << "\n--- Found Location ---\n";
                                std::cout << std::left << std::setw(10) << "ID"
                                          << std::setw(25) << "Name"
                                          << std::setw(20) << "Description"
                                          << std::setw(15) << "Population" << "\n";
                                std::cout << std::string(70, '-') << "\n";
                                std::cout << std::left << std::setw(10) << foundLoc->id
                                          << std::setw(25) << foundLoc->name
                                          << std::setw(20) << foundLoc->description
                                          << std::setw(15) << foundLoc->population << "\n";
                                std::cout << std::string(70, '-') << "\n";
                            } else {
                                std::cout << "Location with ID '" << searchId << "' not found.\n";
                            }
                            break;
                        }
                        case 6: { // Search Location by Name
                            std::string searchName = getStringInput("Enter Location Name (partial match allowed): ");
                            std::vector<Location> foundLocations = locationManager.searchLocationsByName(searchName);
                            if (!foundLocations.empty()) {
                                std::cout << "\n--- Found Locations by Name ---\n";
                                std::cout << std::left << std::setw(10) << "ID"
                                          << std::setw(25) << "Name"
                                          << std::setw(20) << "Description"
                                          << std::setw(15) << "Population" << "\n";
                                std::cout << std::string(70, '-') << "\n";
                                for (const auto& loc : foundLocations) {
                                    std::cout << std::left << std::setw(10) << loc.id
                                              << std::setw(25) << loc.name
                                              << std::setw(20) << loc.description
                                              << std::setw(15) << loc.population << "\n";
                                }
                                std::cout << std::string(70, '-') << "\n";
                            } else {
                                std::cout << "No locations found matching name '" << searchName << "'.\n";
                            }
                            break;
                        }
                        case 7: // Back to Main Menu
                            goto end_location_menu; // Use goto to break out of nested loop
                        default:
                            std::cout << "Invalid choice. Please try again.\n";
                    }
                }
                end_location_menu:; // Label for goto
                break;

            case 2: // Route Management
                while (true) {
                    displayRouteMenu();
                    choice = getIntInput(""); // Get route menu choice
                    switch (choice) {
                        case 1: // Add Route
                            loc1_id = getStringInput("Enter first Location ID (e.g., D1): ");
                            loc2_id = getStringInput("Enter second Location ID (e.g., P1): ");
                            description = getStringInput("Enter Route Description (e.g., Main Boulevard, Expressway): ");
                            length_km = getDoubleInput("Enter Length in km: "); // Robust double input
                            routeManager.addRoute(loc1_id, loc2_id, description, length_km);
                            break;
                        case 2: // Display All Routes
                            routeManager.displayRelationships();
                            break;
                        case 3: {// Update Route
                            loc1_id = getStringInput("Enter first Location ID of the route to update: ");
                            loc2_id = getStringInput("Enter second Location ID of the route to update: ");
                            description = getStringInput("Enter OLD Route Description: "); // This is the old description to find the route
                            std::string new_description = getStringInput("Enter NEW Route Description: ");
                            double new_length_km = getDoubleInput("Enter NEW Length in km: ");
                            routeManager.updateRoute(loc1_id, loc2_id, description, new_description, new_length_km);
                            break;
                        }
                        case 4: {// Delete Route
                            loc1_id = getStringInput("Enter first Location ID of the route to delete: ");
                            loc2_id = getStringInput("Enter second Location ID of the route to delete: ");
                            description = getStringInput("Enter Route Description to delete: ");
                            routeManager.deleteRoute(loc1_id, loc2_id, description);
                            break;
                        }
                        case 5: { // Find Shortest Route
                            std::string start_loc_id_path = getStringInput("Enter Start Location ID: ");
                            std::string end_loc_id_path = getStringInput("Enter End Location ID: ");
                            routeManager.findShortestRoute(start_loc_id_path, end_loc_id_path);
                            break;
                        }
                        case 6: { // Find Longest Route (Simple Path)
                            std::string start_loc_id_path = getStringInput("Enter Start Location ID: ");
                            std::string end_loc_id_path = getStringInput("Enter End Location ID: ");
                            routeManager.findLongestRoute(start_loc_id_path, end_loc_id_path);
                            break;
                        }
                        case 7: { // Search Routes
                            std::string searchQuery = getStringInput("Enter search query (Location ID or Route Description): ");
                            int searchTypeChoice = getIntInput("Search by: 1. Description, 2. Location ID, 3. Both: ");
                            bool byDesc = false;
                            bool byLocId = false;
                            if (searchTypeChoice == 1) byDesc = true;
                            else if (searchTypeChoice == 2) byLocId = true;
                            else if (searchTypeChoice == 3) { byDesc = true; byLocId = true; }
                            else std::cout << "Invalid search type choice. Searching by both by default.\n";

                            routeManager.searchRoutes(searchQuery, byDesc, byLocId);
                            break;
                        }
                        case 8: // Back to Main Menu
                            goto end_route_menu; // Use goto to break out of nested loop
                        default:
                            std::cout << "Invalid choice. Please try again.\n";
                    }
                }
                end_route_menu:; // Label for goto
                break;

            case 3: // Predefined Scenario Setup
                setupPredefinedScenario(locationManager, routeManager);
                break;

            case 4: // Export Relationships to CSV (Bonus)
                routeManager.exportRelationshipsToCsv();
                break;

            case 5: // Exit
                std::cout << "Exiting Transport Management System. Goodbye!\n";
                return 0; // Exit the program
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0; // Should not be reached but good practice
}
