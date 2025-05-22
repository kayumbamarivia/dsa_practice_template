# Parking Management System Code Explanation

This document provides a line-by-line explanation of a C++ program implementing a parking management system. The system manages parking facilities, vehicles, slots, and parking records, with features for file I/O, graph-based navigation, and a console-based interface.

## Header Includes
```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <limits>
#include <algorithm>
#include <ctime>
```
- **Explanation**:
  - `<iostream>`: Enables input/output operations (e.g., `cout`, `cin`).
  - `<fstream>`: Supports file input/output for saving/loading data.
  - `<string>`: Provides string manipulation capabilities.
  - `<vector>`: Used for dynamic arrays to store lists of facilities, vehicles, etc.
  - `<unordered_map>`: Implements hash tables for efficient facility and connection lookups.
  - `<unordered_set>`: Used for tracking visited nodes in graph algorithms.
  - `<sstream>`: Facilitates string stream operations for parsing file data.
  - `<limits>`: Provides `numeric_limits` for initializing maximum distances in algorithms.
  - `<algorithm>`: Includes utilities like `reverse` for path reconstruction.
  - `<ctime>`: Used for generating timestamps.

## Namespace Declaration
```cpp
using namespace std;
```
- **Explanation**: Imports the standard namespace to avoid prefixing standard library elements (e.g., `std::cout` becomes `cout`).

## Data Structures
### Vehicle Struct
```cpp
struct Vehicle {
    int vehicle_id;
    string owner_name;
    string license_plate;
    string vehicle_type;
    Vehicle* next;

    Vehicle(int id, string name, string plate, string type)
        : vehicle_id(id), owner_name(name), license_plate(plate), vehicle_type(type), next(nullptr) {}
};
```
- **Explanation**:
  - Defines a `Vehicle` struct to store vehicle information: ID, owner name, license plate, and type (e.g., car, truck).
  - `next`: Pointer for a linked list implementation.
  - Constructor initializes fields and sets `next` to `nullptr`.

### Slot Struct
```cpp
struct Slot {
    int slot_id;
    string location;
    bool is_occupied;
    Slot* next;

    Slot(int id, string loc, bool occupied)
        : slot_id(id), location(loc), is_occupied(occupied), next(nullptr) {}
};
```
- **Explanation**:
  - Represents a parking slot with an ID, location (e.g., "A1"), and occupancy status.
  - `next`: Pointer for linked list.
  - Constructor initializes fields with `next` as `nullptr`.

### ParkingRecord Struct
```cpp
struct ParkingRecord {
    int record_id;
    int vehicle_id;
    int slot_id;
    string parked_time;
    ParkingRecord* next;

    ParkingRecord(int rid, int vid, int sid, string time)
        : record_id(rid), vehicle_id(vid), slot_id(sid), parked_time(time), next(nullptr) {}
};
```
- **Explanation**:
  - Stores parking event details: record ID, vehicle ID, slot ID, and timestamp.
  - `next`: Pointer for linked list.
  - Constructor initializes fields with `next` as `nullptr`.

### ParkingFacility Struct
```cpp
struct ParkingFacility {
    string parking_id;
    string name;
    Vehicle* vehicles_head;
    Slot* slots_head;
    ParkingRecord* records_head;
    unordered_map<string, int> connected_to;

    ParkingFacility(string id, string n)
        : parking_id(id), name(n), vehicles_head(nullptr), slots_head(nullptr), records_head(nullptr) {}
};
```
- **Explanation**:
  - Represents a parking facility with an ID and name.
  - Maintains linked lists for vehicles, slots, and records.
  - `connected_to`: Maps connected facility IDs to distances (for graph navigation).
  - Constructor initializes fields, setting pointers to `nullptr`.

## ParkingGraph Class
```cpp
class ParkingGraph {
private:
    unordered_map<string, ParkingFacility*> facilities;
```
- **Explanation**: Defines a class to manage parking facilities using a hash map for efficient lookup by ID.

### Destructor
```cpp
public:
    ~ParkingGraph() {
        for (auto& pair : facilities) {
            delete pair.second;
        }
    }
```
- **Explanation**: Frees memory for all facilities to prevent memory leaks.

### Add Facility
```cpp
    bool addFacility(string id, string name) {
        if (facilities.find(id) != facilities.end()) {
            return false;
        }
        facilities[id] = new ParkingFacility(id, name);
        return true;
    }
```
- **Explanation**:
  - Adds a new facility if the ID doesn’t exist.
  - Returns `false` if the ID is already in use, `true` on success.

### Remove Facility
```cpp
    bool removeFacility(string id) {
        auto it = facilities.find(id);
        if (it == facilities.end()) {
            return false;
        }
        for (auto& pair : facilities) {
            pair.second->connected_to.erase(id);
        }
        delete it->second;
        facilities.erase(it);
        return true;
    }
```
- **Explanation**:
  - Removes a facility by ID.
  - Removes connections to this facility from others.
  - Deletes the facility and removes it from the map.

### Get Facility
```cpp
    ParkingFacility* getFacility(string id) {
        auto it = facilities.find(id);
        return it == facilities.end() ? nullptr : it->second;
    }
```
- **Explanation**: Retrieves a facility by ID, returning `nullptr` if not found.

### Get All Facility IDs
```cpp
    vector<string> getAllFacilityIds() {
        vector<string> ids;
        for (const auto& pair : facilities) {
            ids.push_back(pair.first);
        }
        return ids;
    }
```
- **Explanation**: Returns a vector of all facility IDs.

### Add Connection
```cpp
    bool addConnection(string from, string to, int distance) {
        if (facilities.find(from) == facilities.end() || facilities.find(to) == facilities.end()) {
            return false;
        }
        facilities[from]->connected_to[to] = distance;
        facilities[to]->connected_to[from] = distance;
        return true;
    }
```
- **Explanation**: Adds a bidirectional connection between two facilities with a specified distance.

### Remove Connection
```cpp
    bool removeConnection(string from, string to) {
        if (facilities.find(from) == facilities.end() || facilities.find(to) == facilities.end()) {
            return false;
        }
        facilities[from]->connected_to.erase(to);
        facilities[to]->connected_to.erase(from);
        return true;
    }
```
- **Explanation**: Removes a bidirectional connection between two facilities.

### Save to File
```cpp
    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }
        file << "Parking_ID,Name,Vehicles,Slots,Records,Connected_To,Distance\n";
        for (const auto& pair : facilities) {
            ParkingFacility* facility = pair.second;
            file << facility->parking_id << "," << facility->name << ",";
            Vehicle* vehicle = facility->vehicles_head;
            while (vehicle) {
                file << vehicle->vehicle_id << "," << vehicle->owner_name << ","
                     << vehicle->license_plate << "," << vehicle->vehicle_type;
                vehicle = vehicle->next;
                if (vehicle) file << ";";
            }
            file << ",";
            Slot* slot = facility->slots_head;
            while (slot) {
                file << slot->slot_id << "," << slot->location << "," << (slot->is_occupied ? "True" : "False");
                slot = slot->next;
                if (slot) file << ";";
            }
            file << ",";
            ParkingRecord* record = facility->records_head;
            while (record) {
                file << record->record_id << "," << record->vehicle_id << ","
                     << record->slot_id << "," << record->parked_time;
                record = record->next;
                if (record) file << ";";
            }
            file << ",";
            bool first = true;
            for (const auto& conn : facility->connected_to) {
                if (!first) file << ";";
                file << conn.first << "," << conn.second;
                first = false;
            }
            file << "\n";
        }
        file.close();
    }
```
- **Explanation**:
  - Saves facility data to a CSV file.
  - Writes a header line.
  - For each facility, writes ID, name, vehicles, slots, records, and connections, using commas and semicolons to separate fields and entries.

### Load from File
```cpp
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for reading: " << filename << endl;
            return;
        }
        facilities.clear();
        string line;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            if (tokens.size() < 6) continue;
            string parking_id = tokens[0];
            string name = tokens[1];
            string vehicles_str = tokens[2];
            string slots_str = tokens[3];
            string records_str = tokens[4];
            string connections_str = tokens.size() > 5 ? tokens[5] : "";
            addFacility(parking_id, name);
            ParkingFacility* facility = getFacility(parking_id);
            if (!vehicles_str.empty()) {
                stringstream vss(vehicles_str);
                string vehicle_token;
                while (getline(vss, vehicle_token, ';')) {
                    stringstream vss2(vehicle_token);
                    string vpart;
                    vector<string> vparts;
                    while (getline(vss2, vpart, ',')) {
                        vparts.push_back(vpart);
                    }
                    if (vparts.size() == 4) {
                        int vid = stoi(vparts[0]);
                        registerVehicle(parking_id, vid, vparts[1], vparts[2], vparts[3]);
                    }
                }
            }
            if (!slots_str.empty()) {
                stringstream sss(slots_str);
                string slot_token;
                while (getline(sss, slot_token, ';')) {
                    stringstream sss2(slot_token);
                    string spart;
                    vector<string> sparts;
                    while (getline(sss2, spart, ',')) {
                        sparts.push_back(spart);
                    }
                    if (sparts.size() == 3) {
                        int sid = stoi(sparts[0]);
                        bool occupied = (sparts[2] == "True");
                        registerSlot(parking_id, sid, sparts[1], occupied);
                    }
                }
            }
            if (!records_str.empty()) {
                stringstream rss(records_str);
                string record_token;
                while (getline(rss, record_token, ';')) {
                    stringstream rss2(record_token);
                    string rpart;
                    vector<string> rparts;
                    while (getline(rss2, rpart, ',')) {
                        rparts.push_back(rpart);
                    }
                    if (rparts.size() == 4) {
                        int rid = stoi(rparts[0]);
                        int vid = stoi(rparts[1]);
                        int sid = stoi(rparts[2]);
                        registerParkingRecord(parking_id, rid, vid, sid, rparts[3]);
                    }
                }
            }
            if (!connections_str.empty()) {
                stringstream css(connections_str);
                string conn_token;
                while (getline(css, conn_token, ';')) {
                    stringstream css2(conn_token);
                    string cpart;
                    vector<string> cparts;
                    while (getline(css2, cpart, ',')) {
                        cparts.push_back(cpart);
                    }
                    if (cparts.size() == 2) {
                        string to_facility = cparts[0];
                        int distance = stoi(cparts[1]);
                        addConnection(parking_id, to_facility, distance);
                    }
                }
            }
        }
        file.close();
    }
```
- **Explanation**:
  - Loads data from a CSV file.
  - Clears existing facilities.
  - Skips header line.
  - Parses each line into tokens for facility ID, name, vehicles, slots, records, and connections.
  - Reconstructs facilities, vehicles, slots, records, and connections.

### Register Vehicle
```cpp
    bool registerVehicle(string facility_id, int vehicle_id, string owner_name, string license_plate, string vehicle_type) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return false;
        Vehicle* current = facility->vehicles_head;
        while (current) {
            if (current->vehicle_id == vehicle_id) {
                return false;
            }
            current = current->next;
        }
        Vehicle* new_vehicle = new Vehicle(vehicle_id, owner_name, license_plate, vehicle_type);
        new_vehicle->next = facility->vehicles_head;
        facility->vehicles_head = new_vehicle;
        return true;
    }
```
- **Explanation**:
  - Registers a vehicle in a facility.
  - Checks if the facility exists and if the vehicle ID is unique.
  - Adds the vehicle to the front of the linked list.

### Get Vehicle
```cpp
    Vehicle* getVehicle(string facility_id, int vehicle_id) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return nullptr;
        Vehicle* current = facility->vehicles_head;
        while (current) {
            if (current->vehicle_id == vehicle_id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
```
- **Explanation**: Retrieves a vehicle by ID from a facility, returning `nullptr` if not found.

### Get All Vehicles
```cpp
    vector<Vehicle*> getAllVehicles(string facility_id) {
        vector<Vehicle*> vehicles;
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return vehicles;
        Vehicle* current = facility->vehicles_head;
        while (current) {
            vehicles.push_back(current);
            current = current->next;
        }
        return vehicles;
    }
```
- **Explanation**: Returns a vector of all vehicles in a facility.

### Register Slot
```cpp
    bool registerSlot(string facility_id, int slot_id, string location, bool is_occupied) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return false;
        Slot* current = facility->slots_head;
        while (current) {
            if (current->slot_id == slot_id) {
                return false;
            }
            current = current->next;
        }
        Slot* new_slot = new Slot(slot_id, location, is_occupied);
        new_slot->next = facility->slots_head;
        facility->slots_head = new_slot;
        return true;
    }
```
- **Explanation**: Registers a slot, ensuring the slot ID is unique, and adds it to the linked list.

### Get Slot
```cpp
    Slot* getSlot(string facility_id, int slot_id) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return nullptr;
        Slot* current = facility->slots_head;
        while (current) {
            if (current->slot_id == slot_id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
```
- **Explanation**: Retrieves a slot by ID, returning `nullptr` if not found.

### Get All Slots
```cpp
    vector<Slot*> getAllSlots(string facility_id) {
        vector<Slot*> slots;
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return slots;
        Slot* current = facility->slots_head;
        while (current) {
            slots.push_back(current);
            current = current->next;
        }
        return slots;
    }
```
- **Explanation**: Returns a vector of all slots in a facility.

### Register Parking Record
```cpp
    bool registerParkingRecord(string facility_id, int record_id, int vehicle_id, int slot_id, string parked_time) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return false;
        ParkingRecord* current_rec = facility->records_head;
        while (current_rec) {
            if (current_rec->record_id == record_id) {
                return false;
            }
            current_rec = current_rec->next;
        }
        Vehicle* vehicle = getVehicle(facility_id, vehicle_id);
        if (!vehicle) return false;
        Slot* slot = getSlot(facility_id, slot_id);
        if (!slot) return false;
        ParkingRecord* new_record = new ParkingRecord(record_id, vehicle_id, slot_id, parked_time);
        new_record->next = facility->records_head;
        facility->records_head = new_record;
        slot->is_occupied = true;
        return true;
    }
```
- **Explanation**:
  - Registers a parking record, ensuring record ID is unique and vehicle/slot exist.
  - Updates slot occupancy to `true`.

### Get All Parking Records
```cpp
    vector<ParkingRecord*> getAllParkingRecords(string facility_id) {
        vector<ParkingRecord*> records;
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return records;
        ParkingRecord* current = facility->records_head;
        while (current) {
            records.push_back(current);
            current = current->next;
        }
        return records;
    }
```
- **Explanation**: Returns a vector of all parking records in a facility.

### Find Shortest Path (Dijkstra’s Algorithm)
```cpp
    vector<string> findShortestPath(string from, string to) {
        unordered_map<string, int> distances;
        unordered_map<string, string> previous;
        unordered_set<string> visited;
        vector<string> path;
        for (const auto& pair : facilities) {
            distances[pair.first] = numeric_limits<int>::max();
        }
        distances[from] = 0;
        while (true) {
            string current;
            int smallest_distance = numeric_limits<int>::max();
            for (const auto& pair : facilities) {
                if (!visited.count(pair.first) && distances[pair.first] < smallest_distance) {
                    smallest_distance = distances[pair.first];
                    current = pair.first;
                }
            }
            if (current.empty() || current == to) break;
            visited.insert(current);
            for (const auto& neighbor : facilities[current]->connected_to) {
                string neighbor_id = neighbor.first;
                int distance = neighbor.second;
                int total_distance = distances[current] + distance;
                if (total_distance < distances[neighbor_id]) {
                    distances[neighbor_id] = total_distance;
                    previous[neighbor_id] = current;
                }
            }
        }
        if (distances[to] == numeric_limits<int>::max()) {
            return path;
        }
        for (string at = to; !at.empty(); at = previous[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        return path;
    }
```
- **Explanation**:
  - Implements Dijkstra’s algorithm to find the shortest path between two facilities.
  - Uses distance and previous node maps to track paths.
  - Returns the path as a vector of facility IDs.

### Find Nearest Available Facility
```cpp
    string findNearestAvailable(string from_facility_id) {
        unordered_map<string, int> distances;
        unordered_set<string> visited;
        string nearest;
        int min_distance = numeric_limits<int>::max();
        for (const auto& pair : facilities) {
            distances[pair.first] = numeric_limits<int>::max();
        }
        distances[from_facility_id] = 0;
        while (true) {
            string current;
            int smallest_distance = numeric_limits<int>::max();
            for (const auto& pair : facilities) {
                if (!visited.count(pair.first) && distances[pair.first] < smallest_distance) {
                    smallest_distance = distances[pair.first];
                    current = pair.first;
                }
            }
            if (current.empty()) break;
            vector<Slot*> slots = getAllSlots(current);
            for (Slot* slot : slots) {
                if (!slot->is_occupied) {
                    if (distances[current] < min_distance) {
                        min_distance = distances[current];
                        nearest = current;
                    }
                    break;
                }
            }
            visited.insert(current);
            for (const auto& neighbor : facilities[current]->connected_to) {
                string neighbor_id = neighbor.first;
                int distance = neighbor.second;
                int total_distance = distances[current] + distance;
                if (total_distance < distances[neighbor_id]) {
                    distances[neighbor_id] = total_distance;
                }
            }
        }
        return nearest;
    }
```
- **Explanation**:
  - Uses a modified Dijkstra’s algorithm to find the nearest facility with available slots.
  - Returns the facility ID or an empty string if none found.

## Helper Functions
### Get Current Time
```cpp
string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    return to_string(1900 + ltm->tm_year) + "-" +
           to_string(1 + ltm->tm_mon) + "-" +
           to_string(ltm->tm_mday) + " " +
           to_string(ltm->tm_hour) + ":" +
           to_string(ltm->tm_min) + ":" +
           to_string(ltm->tm_sec);
}
```
- **Explanation**: Returns the current timestamp in `YYYY-MM-DD HH:MM:SS` format.

### Display Menus
```cpp
void displayMainMenu() { ... }
void displayFacilityMenu() { ... }
void displayVehicleMenu() { ... }
void displaySlotMenu() { ... }
void displayRecordMenu() { ... }
```
- **Explanation**: These functions print console menus for user interaction, guiding through facility, vehicle, slot, and record management options.

## Main Function
```cpp
int main() {
    ParkingGraph parkingSystem;
    const string dataFile = "parking_data.csv";
    parkingSystem.loadFromFile(dataFile);
    int choice;
    do {
        displayMainMenu();
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: { // Facility Management
                int facilityChoice;
                do {
                    displayFacilityMenu();
                    cin >> facilityChoice;
                    cin.ignore();
                    switch (facilityChoice) {
                        case 1: { ... } // Add Facility
                        case 2: { ... } // Remove Facility
                        case 3: { ... } // List Facilities
                        case 4: { ... } // Add Connection
                        case 5: { ... } // Remove Connection
                        case 6: break;
                        default: cout << "Invalid choice. Please try again.\n";
                    }
                } while (facilityChoice != 6);
                break;
            }
            case 2: { // Vehicle Management
                int vehicleChoice;
                do {
                    displayVehicleMenu();
                    cin >> vehicleChoice;
                    cin.ignore();
                    switch (vehicleChoice) {
                        case 1: { ... } // Register Vehicle
                        case 2: { ... } // View All Vehicles
                        case 3: break;
                        default: cout << "Invalid choice. Please try again.\n";
                    }
                } while (vehicleChoice != 3);
                break;
            }
            case 3: { // Slot Management
                int slotChoice;
                do {
                    displaySlotMenu();
                    cin >> slotChoice;
                    cin.ignore();
                    switch (slotChoice) {
                        case 1: { ... } // Register Slot
                        case 2: { ... } // View All Slots
                        case 3: break;
                        default: cout << "Invalid choice. Please try again.\n";
                    }
                } while (slotChoice != 3);
                break;
            }
            case 4: { // Record Management
                int recordChoice;
                do {
                    displayRecordMenu();
                    cin >> recordChoice;
                    cin.ignore();
                    switch (recordChoice) {
                        case 1: { ... } // Register Record
                        case 2: { ... } // View All Records
                        case 3: break;
                        default: cout << "Invalid choice. Please try again.\n";
                    }
                } while (recordChoice != 3);
                break;
            }
            case 5: { ... } // Find Shortest Path
            case 6: { ... } // Find Nearest Available
            case 7: { ... } // Save to File
            case 8: { ... } // Exit
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);
    return 0;
}
```
- **Explanation**:
  - Initializes a `ParkingGraph` object and loads data from `parking_data.csv`.
  - Runs a loop displaying the main menu, handling user input for various operations.
  - Sub-menus manage facilities, vehicles, slots, and records.
  - Saves data to file on exit or when requested.
  - Exits when the user selects option 8.