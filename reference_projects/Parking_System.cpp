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

using namespace std;

// Data Structures
struct Vehicle {
    int vehicle_id;
    string owner_name;
    string license_plate;
    string vehicle_type;
    Vehicle* next;

    Vehicle(int id, string name, string plate, string type)
        : vehicle_id(id), owner_name(name), license_plate(plate), vehicle_type(type), next(nullptr) {}
};

struct Slot {
    int slot_id;
    string location;
    bool is_occupied;
    Slot* next;

    Slot(int id, string loc, bool occupied)
        : slot_id(id), location(loc), is_occupied(occupied), next(nullptr) {}
};

struct ParkingRecord {
    int record_id;
    int vehicle_id;
    int slot_id;
    string parked_time;
    ParkingRecord* next;

    ParkingRecord(int rid, int vid, int sid, string time)
        : record_id(rid), vehicle_id(vid), slot_id(sid), parked_time(time), next(nullptr) {}
};

struct ParkingFacility {
    string parking_id;
    string name;
    Vehicle* vehicles_head;
    Slot* slots_head;
    ParkingRecord* records_head;
    unordered_map<string, int> connected_to; // facility_id -> distance

    ParkingFacility(string id, string n)
        : parking_id(id), name(n), vehicles_head(nullptr), slots_head(nullptr), records_head(nullptr) {}
};

// Graph to manage parking facilities
class ParkingGraph {
private:
    unordered_map<string, ParkingFacility*> facilities;

public:
    ~ParkingGraph() {
        for (auto& pair : facilities) {
            delete pair.second;
        }
    }

    // CRUD operations for parking facilities
    bool addFacility(string id, string name) {
        if (facilities.find(id) != facilities.end()) {
            return false; // Facility already exists
        }
        facilities[id] = new ParkingFacility(id, name);
        return true;
    }

    bool removeFacility(string id) {
        auto it = facilities.find(id);
        if (it == facilities.end()) {
            return false;
        }

        // Remove all connections to this facility
        for (auto& pair : facilities) {
            pair.second->connected_to.erase(id);
        }

        delete it->second;
        facilities.erase(it);
        return true;
    }

    ParkingFacility* getFacility(string id) {
        auto it = facilities.find(id);
        return it == facilities.end() ? nullptr : it->second;
    }

    vector<string> getAllFacilityIds() {
        vector<string> ids;
        for (const auto& pair : facilities) {
            ids.push_back(pair.first);
        }
        return ids;
    }

    // Connection management
    bool addConnection(string from, string to, int distance) {
        if (facilities.find(from) == facilities.end() || facilities.find(to) == facilities.end()) {
            return false;
        }
        facilities[from]->connected_to[to] = distance;
        facilities[to]->connected_to[from] = distance;
        return true;
    }

    bool removeConnection(string from, string to) {
        if (facilities.find(from) == facilities.end() || facilities.find(to) == facilities.end()) {
            return false;
        }
        facilities[from]->connected_to.erase(to);
        facilities[to]->connected_to.erase(from);
        return true;
    }

    // Save and load from file
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

            // Save vehicles
            Vehicle* vehicle = facility->vehicles_head;
            while (vehicle) {
                file << vehicle->vehicle_id << "," << vehicle->owner_name << ","
                     << vehicle->license_plate << "," << vehicle->vehicle_type;
                vehicle = vehicle->next;
                if (vehicle) file << ";";
            }
            file << ",";

            // Save slots
            Slot* slot = facility->slots_head;
            while (slot) {
                file << slot->slot_id << "," << slot->location << "," << (slot->is_occupied ? "True" : "False");
                slot = slot->next;
                if (slot) file << ";";
            }
            file << ",";

            // Save records
            ParkingRecord* record = facility->records_head;
            while (record) {
                file << record->record_id << "," << record->vehicle_id << ","
                     << record->slot_id << "," << record->parked_time;
                record = record->next;
                if (record) file << ";";
            }
            file << ",";

            // Save connections
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

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for reading: " << filename << endl;
            return;
        }

        // Clear existing data
        facilities.clear();

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;

            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() < 6) continue; // Invalid line

            string parking_id = tokens[0];
            string name = tokens[1];
            string vehicles_str = tokens[2];
            string slots_str = tokens[3];
            string records_str = tokens[4];
            string connections_str = tokens.size() > 5 ? tokens[5] : "";

            // Create facility
            addFacility(parking_id, name);
            ParkingFacility* facility = getFacility(parking_id);

            // Parse vehicles
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

            // Parse slots
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

            // Parse records
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

            // Parse connections
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

    // Vehicle operations
    bool registerVehicle(string facility_id, int vehicle_id, string owner_name, string license_plate, string vehicle_type) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return false;

        // Check for duplicate vehicle_id
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

    // Slot operations
    bool registerSlot(string facility_id, int slot_id, string location, bool is_occupied) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return false;

        // Check for duplicate slot_id
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

    // Parking record operations
    bool registerParkingRecord(string facility_id, int record_id, int vehicle_id, int slot_id, string parked_time) {
        ParkingFacility* facility = getFacility(facility_id);
        if (!facility) return false;

        // Check for duplicate record_id
        ParkingRecord* current_rec = facility->records_head;
        while (current_rec) {
            if (current_rec->record_id == record_id) {
                return false;
            }
            current_rec = current_rec->next;
        }

        // Check if vehicle exists
        Vehicle* vehicle = getVehicle(facility_id, vehicle_id);
        if (!vehicle) return false;

        // Check if slot exists
        Slot* slot = getSlot(facility_id, slot_id);
        if (!slot) return false;

        // Create new record
        ParkingRecord* new_record = new ParkingRecord(record_id, vehicle_id, slot_id, parked_time);
        new_record->next = facility->records_head;
        facility->records_head = new_record;

        // Update slot occupancy
        slot->is_occupied = true;

        return true;
    }

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

    // Graph algorithms
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

            // Find the unvisited node with the smallest distance
            for (const auto& pair : facilities) {
                if (!visited.count(pair.first) && distances[pair.first] < smallest_distance) {
                    smallest_distance = distances[pair.first];
                    current = pair.first;
                }
            }

            if (current.empty() || current == to) break;

            visited.insert(current);

            // Update distances to neighbors
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

        // Reconstruct path
        if (distances[to] == numeric_limits<int>::max()) {
            return path; // No path exists
        }

        for (string at = to; !at.empty(); at = previous[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());

        return path;
    }

    // Find nearest facility with available slots
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

            // Find the unvisited node with the smallest distance
            for (const auto& pair : facilities) {
                if (!visited.count(pair.first) && distances[pair.first] < smallest_distance) {
                    smallest_distance = distances[pair.first];
                    current = pair.first;
                }
            }

            if (current.empty()) break;

            // Check if current facility has available slots
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

            // Update distances to neighbors
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
};

// Helper functions
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

void displayMainMenu() {
    cout << "\nParking Management System\n";
    cout << "1. Manage Parking Facilities\n";
    cout << "2. Manage Vehicles\n";
    cout << "3. Manage Parking Slots\n";
    cout << "4. Manage Parking Records\n";
    cout << "5. Find Shortest Path Between Facilities\n";
    cout << "6. Find Nearest Available Parking\n";
    cout << "7. Save Data to File\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

void displayFacilityMenu() {
    cout << "\nParking Facility Management\n";
    cout << "1. Add Parking Facility\n";
    cout << "2. Remove Parking Facility\n";
    cout << "3. List All Facilities\n";
    cout << "4. Add Connection Between Facilities\n";
    cout << "5. Remove Connection Between Facilities\n";
    cout << "6. Back to Main Menu\n";
    cout << "Enter your choice: ";
}

void displayVehicleMenu() {
    cout << "\nVehicle Management\n";
    cout << "1. Register Vehicle\n";
    cout << "2. View All Vehicles in Facility\n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter your choice: ";
}

void displaySlotMenu() {
    cout << "\nParking Slot Management\n";
    cout << "1. Register Slot\n";
    cout << "2. View All Slots in Facility\n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter your choice: ";
}

void displayRecordMenu() {
    cout << "\nParking Record Management\n";
    cout << "1. Register Parking Record\n";
    cout << "2. View All Records in Facility\n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter your choice: ";
}

int main() {
    ParkingGraph parkingSystem;
    const string dataFile = "parking_data.csv";

    // Load data from file if it exists
    parkingSystem.loadFromFile(dataFile);

    int choice;
    do {
        displayMainMenu();
        cin >> choice;
        cin.ignore(); // Clear newline character

        switch (choice) {
            case 1: { // Facility Management
                int facilityChoice;
                do {
                    displayFacilityMenu();
                    cin >> facilityChoice;
                    cin.ignore();

                    switch (facilityChoice) {
                        case 1: { // Add Facility
                            string id, name;
                            cout << "Enter Facility ID: ";
                            getline(cin, id);
                            cout << "Enter Facility Name: ";
                            getline(cin, name);

                            if (parkingSystem.addFacility(id, name)) {
                                cout << "Facility added successfully.\n";
                            } else {
                                cout << "Error: Facility with this ID already exists.\n";
                            }
                            break;
                        }
                        case 2: { // Remove Facility
                            string id;
                            cout << "Enter Facility ID to remove: ";
                            getline(cin, id);

                            if (parkingSystem.removeFacility(id)) {
                                cout << "Facility removed successfully.\n";
                            } else {
                                cout << "Error: Facility not found.\n";
                            }
                            break;
                        }
                        case 3: { // List Facilities
                            vector<string> facilities = parkingSystem.getAllFacilityIds();
                            cout << "\nParking Facilities:\n";
                            for (const string& id : facilities) {
                                ParkingFacility* facility = parkingSystem.getFacility(id);
                                cout << "ID: " << id << ", Name: " << facility->name << "\n";
                                cout << "Connected to: ";
                                for (const auto& conn : facility->connected_to) {
                                    cout << conn.first << " (" << conn.second << " km), ";
                                }
                                cout << "\n";
                            }
                            break;
                        }
                        case 4: { // Add Connection
                            string from, to;
                            int distance;
                            cout << "Enter From Facility ID: ";
                            getline(cin, from);
                            cout << "Enter To Facility ID: ";
                            getline(cin, to);
                            cout << "Enter Distance (km): ";
                            cin >> distance;
                            cin.ignore();

                            if (parkingSystem.addConnection(from, to, distance)) {
                                cout << "Connection added successfully.\n";
                            } else {
                                cout << "Error: One or both facilities not found.\n";
                            }
                            break;
                        }
                        case 5: { // Remove Connection
                            string from, to;
                            cout << "Enter From Facility ID: ";
                            getline(cin, from);
                            cout << "Enter To Facility ID: ";
                            getline(cin, to);

                            if (parkingSystem.removeConnection(from, to)) {
                                cout << "Connection removed successfully.\n";
                            } else {
                                cout << "Error: One or both facilities not found or no connection exists.\n";
                            }
                            break;
                        }
                        case 6: // Back to main menu
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
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
                        case 1: { // Register Vehicle
                            string facility_id;
                            int vehicle_id;
                            string owner_name, license_plate, vehicle_type;

                            cout << "Enter Facility ID: ";
                            getline(cin, facility_id);
                            cout << "Enter Vehicle ID: ";
                            cin >> vehicle_id;
                            cin.ignore();
                            cout << "Enter Owner Name: ";
                            getline(cin, owner_name);
                            cout << "Enter License Plate: ";
                            getline(cin, license_plate);
                            cout << "Enter Vehicle Type: ";
                            getline(cin, vehicle_type);

                            if (parkingSystem.registerVehicle(facility_id, vehicle_id, owner_name, license_plate, vehicle_type)) {
                                cout << "Vehicle registered successfully.\n";
                            } else {
                                cout << "Error: Facility not found or vehicle ID already exists.\n";
                            }
                            break;
                        }
                        case 2: { // View All Vehicles
                            string facility_id;
                            cout << "Enter Facility ID: ";
                            getline(cin, facility_id);

                            vector<Vehicle*> vehicles = parkingSystem.getAllVehicles(facility_id);
                            if (vehicles.empty()) {
                                cout << "No vehicles found or facility does not exist.\n";
                            } else {
                                cout << "\nVehicles in Facility " << facility_id << ":\n";
                                for (Vehicle* vehicle : vehicles) {
                                    cout << "ID: " << vehicle->vehicle_id << ", Owner: " << vehicle->owner_name
                                         << ", Plate: " << vehicle->license_plate << ", Type: " << vehicle->vehicle_type << "\n";
                                }
                            }
                            break;
                        }
                        case 3: // Back to main menu
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
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
                        case 1: { // Register Slot
                            string facility_id, location;
                            int slot_id;
                            bool is_occupied;

                            cout << "Enter Facility ID: ";
                            getline(cin, facility_id);
                            cout << "Enter Slot ID: ";
                            cin >> slot_id;
                            cin.ignore();
                            cout << "Enter Location: ";
                            getline(cin, location);
                            cout << "Is Occupied? (1 for Yes, 0 for No): ";
                            cin >> is_occupied;
                            cin.ignore();

                            if (parkingSystem.registerSlot(facility_id, slot_id, location, is_occupied)) {
                                cout << "Slot registered successfully.\n";
                            } else {
                                cout << "Error: Facility not found or slot ID already exists.\n";
                            }
                            break;
                        }
                        case 2: { // View All Slots
                            string facility_id;
                            cout << "Enter Facility ID: ";
                            getline(cin, facility_id);

                            vector<Slot*> slots = parkingSystem.getAllSlots(facility_id);
                            if (slots.empty()) {
                                cout << "No slots found or facility does not exist.\n";
                            } else {
                                cout << "\nSlots in Facility " << facility_id << ":\n";
                                for (Slot* slot : slots) {
                                    cout << "ID: " << slot->slot_id << ", Location: " << slot->location
                                         << ", Occupied: " << (slot->is_occupied ? "Yes" : "No") << "\n";
                                }
                            }
                            break;
                        }
                        case 3: // Back to main menu
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
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
                        case 1: { // Register Record
                            string facility_id;
                            int record_id, vehicle_id, slot_id;
                            string parked_time = getCurrentTime();

                            cout << "Enter Facility ID: ";
                            getline(cin, facility_id);
                            cout << "Enter Record ID: ";
                            cin >> record_id;
                            cout << "Enter Vehicle ID: ";
                            cin >> vehicle_id;
                            cout << "Enter Slot ID: ";
                            cin >> slot_id;
                            cin.ignore();

                            if (parkingSystem.registerParkingRecord(facility_id, record_id, vehicle_id, slot_id, parked_time)) {
                                cout << "Parking record registered successfully at " << parked_time << ".\n";
                            } else {
                                cout << "Error: Facility not found, record ID exists, or vehicle/slot not found.\n";
                            }
                            break;
                        }
                        case 2: { // View All Records
                            string facility_id;
                            cout << "Enter Facility ID: ";
                            getline(cin, facility_id);

                            vector<ParkingRecord*> records = parkingSystem.getAllParkingRecords(facility_id);
                            if (records.empty()) {
                                cout << "No records found or facility does not exist.\n";
                            } else {
                                cout << "\nParking Records in Facility " << facility_id << ":\n";
                                for (ParkingRecord* record : records) {
                                    cout << "Record ID: " << record->record_id << ", Vehicle ID: " << record->vehicle_id
                                         << ", Slot ID: " << record->slot_id << ", Time: " << record->parked_time << "\n";
                                }
                            }
                            break;
                        }
                        case 3: // Back to main menu
                            break;
                        default:
                            cout << "Invalid choice. Please try again.\n";
                    }
                } while (recordChoice != 3);
                break;
            }
            case 5: { // Find Shortest Path
                string from, to;
                cout << "Enter Starting Facility ID: ";
                getline(cin, from);
                cout << "Enter Destination Facility ID: ";
                getline(cin, to);

                vector<string> path = parkingSystem.findShortestPath(from, to);
                if (path.empty()) {
                    cout << "No path exists between these facilities.\n";
                } else {
                    cout << "Shortest Path: ";
                    for (size_t i = 0; i < path.size(); ++i) {
                        if (i != 0) cout << " -> ";
                        cout << path[i];
                    }
                    cout << "\n";
                }
                break;
            }
            case 6: { // Find Nearest Available
                string facility_id;
                cout << "Enter Current Facility ID: ";
                getline(cin, facility_id);

                string nearest = parkingSystem.findNearestAvailable(facility_id);
                if (nearest.empty()) {
                    cout << "No available parking found in any connected facilities.\n";
                } else {
                    cout << "Nearest available parking is at facility: " << nearest << "\n";
                }
                break;
            }
            case 7: { // Save to file
                parkingSystem.saveToFile(dataFile);
                cout << "Data saved to file successfully.\n";
                break;
            }
            case 8: // Exit
                cout << "Exiting program. Saving data...\n";
                parkingSystem.saveToFile(dataFile);
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);

    return 0;
}