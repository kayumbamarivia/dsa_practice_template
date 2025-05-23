#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <climits>
#include <ctime>
#include <queue>
#include <stack>

using namespace std;

// Utility: Safe Integer Input
int readInt(const string &prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            cin.clear();
            continue;
        }
        try {
            int val = stoi(line);
            if (val < minVal || val > maxVal) {
                cout << "Please enter a number";
                if (minVal != INT_MIN && maxVal != INT_MAX)
                    cout << " between " << minVal << " and " << maxVal;
                cout << ".\n";
                continue;
            }
            return val;
        }
        catch (...) {
            cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

// ======== Graph Algorithms ========
class LocationGraph {
private:
    unordered_map<string, vector<pair<string, int>>> adj; // location -> [(neighbor, distance)]

public:
    void addLocation(const string &location) {
        adj[location] = vector<pair<string, int>>();
    }

    void connectLocations(const string &a, const string &b, int distance) {
        adj[a].push_back({b, distance});
        adj[b].push_back({a, distance});
    }

    // Dijkstra's algorithm for shortest path
    vector<string> shortestPath(const string &start, const string &end) {
        unordered_map<string, int> dist;
        unordered_map<string, string> prev;
        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

        for (const auto &kv : adj) {
            dist[kv.first] = INT_MAX;
        }
        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            string u = pq.top().second;
            pq.pop();

            if (u == end) break;

            for (const auto &edge : adj[u]) {
                string v = edge.first;
                int weight = edge.second;

                if (dist[v] > dist[u] + weight) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        // Reconstruct path
        vector<string> path;
        if (dist[end] == INT_MAX) return path;

        for (string at = end; at != ""; at = prev[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        return path;
    }

    void saveToFile(const string &filename) {
        ofstream f(filename);
        f << "from,to,distance\n";
        for (const auto &kv : adj) {
            for (const auto &edge : kv.second) {
                if (kv.first < edge.first) { // Avoid duplicate edges
                    f << kv.first << "," << edge.first << "," << edge.second << "\n";
                }
            }
        }
    }

    void loadFromFile(const string &filename) {
        ifstream f(filename);
        if (!f) return;
        string line;
        getline(f, line); // header
        while (getline(f, line)) {
            stringstream ss(line);
            string from, to, dist;
            getline(ss, from, ',');
            getline(ss, to, ',');
            getline(ss, dist, ',');
            connectLocations(from, to, stoi(dist));
        }
    }
};

// ======== Data Structures ========
struct Vehicle {
    string id;  // License plate
    string make;
    string model;
    string type;  // "Sedan", "SUV", "Truck"
    int year;
    double dailyRate;
    bool isRented;
    Vehicle* next;
};

struct Customer {
    string id;  // Driver's license number
    string name;
    string contact;
    Customer* next;
};

struct Rental {
    int id;
    string vehicleId;
    string customerId;
    string pickupDate;
    string returnDate;
    string pickupLocation;
    string returnLocation;
    double totalCharge;
    Rental* next;
};

// ======== RentalLocation Class ========
class RentalLocation {
public:
    string locationId;
    string name;
    string address;
    Vehicle* vehicles = nullptr;
    Customer* customers = nullptr;
    Rental* rentals = nullptr;
    int nextRentalId = 1;

    RentalLocation(const string &id, const string &nm, const string &addr)
        : locationId(id), name(nm), address(addr) {
        loadData();
        normalizeCounters();
        updateVehicleStatuses();
    }

    // Vehicle management
    bool addVehicle(const string &plate, const string &make, const string &model, 
                   const string &type, int year, double rate) {
        if (findVehicle(plate)) {
            cout << "Vehicle already exists!\n";
            return false;
        }
        vehicles = new Vehicle{plate, make, model, type, year, rate, false, vehicles};
        saveData();
        return true;
    }

    // Customer management
    bool registerCustomer(const string &dlNum, const string &name, const string &contact) {
        if (findCustomer(dlNum)) {
            cout << "Customer already exists!\n";
            return false;
        }
        customers = new Customer{dlNum, name, contact, customers};
        saveData();
        return true;
    }

    // Rental management
    int rentVehicle(const string &customerId, const string &vehicleId, 
                   const string &pickupLoc, int days) {
        Customer* customer = findCustomer(customerId);
        Vehicle* vehicle = findVehicle(vehicleId);
        
        if (!customer || !vehicle) return -1; // Invalid IDs
        if (vehicle->isRented) return -2; // Vehicle unavailable
        
        time_t now = time(0);
        string pickupDate = ctime(&now);
        pickupDate.erase(pickupDate.find('\n'));
        
        // Calculate return date (now + days)
        time_t returnTime = now + (days * 24 * 60 * 60);
        string returnDate = ctime(&returnTime);
        returnDate.erase(returnDate.find('\n'));
        
        int id = nextRentalId++;
        double total = days * vehicle->dailyRate;
        rentals = new Rental{id, vehicleId, customerId, pickupDate, returnDate, 
                            pickupLoc, "", total, rentals};
        vehicle->isRented = true;
        saveData();
        return id;
    }

    bool returnVehicle(int rentalId, const string &returnLoc, bool isDamaged = false) {
        Rental* rental = findRental(rentalId);
        if (!rental || rental->returnLocation != "") return false;
        
        time_t now = time(0);
        rental->returnDate = ctime(&now);
        rental->returnDate.erase(rental->returnDate.find('\n'));
        rental->returnLocation = returnLoc;
        
        // Calculate additional charges if late
        if (isLate(rental)) {
            rental->totalCharge += calculateLateFee(rental);
        }
        if (isDamaged) {
            rental->totalCharge += 500.00; // Damage fee
        }
        
        Vehicle* vehicle = findVehicle(rental->vehicleId);
        if (vehicle) vehicle->isRented = false;
        
        saveData();
        return true;
    }

    // Display functions
    void displayVehicles(bool availableOnly = false) {
        cout << "-- Vehicles at " << name << " --\n";
        for (auto *v = vehicles; v; v = v->next) {
            if (!availableOnly || !v->isRented) {
                cout << v->id << ": " << v->year << " " << v->make << " " << v->model 
                     << " (" << v->type << ") - $" << v->dailyRate << "/day - " 
                     << (v->isRented ? "Rented" : "Available") << "\n";
            }
        }
    }

    void displayCustomers() {
        cout << "-- Customers --\n";
        for (auto *c = customers; c; c = c->next) {
            cout << c->id << ": " << c->name << " (" << c->contact << ")\n";
        }
    }

    void displayRentals(bool currentOnly = false) {
        cout << "-- Rental Agreements --\n";
        for (auto *r = rentals; r; r = r->next) {
            if (!currentOnly || r->returnLocation == "") {
                cout << r->id << ": " << r->vehicleId << " rented by " << r->customerId
                     << "\n  Pickup: " << r->pickupDate << " at " << r->pickupLocation
                     << "\n  Due: " << r->returnDate;
                if (r->returnLocation != "") {
                    cout << "\n  Returned: " << r->returnDate << " at " << r->returnLocation
                         << " (Total: $" << r->totalCharge << ")";
                }
                cout << "\n";
            }
        }
    }

private:
    // Helper functions
    Vehicle* findVehicle(const string &id) {
        for (auto *v = vehicles; v; v = v->next)
            if (v->id == id) return v;
        return nullptr;
    }

    Customer* findCustomer(const string &id) {
        for (auto *c = customers; c; c = c->next)
            if (c->id == id) return c;
        return nullptr;
    }

    Rental* findRental(int id) {
        for (auto *r = rentals; r; r = r->next)
            if (r->id == id) return r;
        return nullptr;
    }

    bool isLate(Rental* rental) {
        // Compare current date with return date
        // Simplified - would parse dates in real implementation
        return false;
    }

    double calculateLateFee(Rental* rental) {
        // Calculate days late
        int daysLate = 2; // Simplified
        return daysLate * 50.00; // $50/day late fee
    }

    void normalizeCounters() {
        for (auto *r = rentals; r; r = r->next)
            nextRentalId = max(nextRentalId, r->id + 1);
    }

    void updateVehicleStatuses() {
        unordered_set<string> rentedVehicles;
        for (auto *r = rentals; r; r = r->next) {
            if (r->returnLocation == "") {
                rentedVehicles.insert(r->vehicleId);
            }
        }
        for (auto *v = vehicles; v; v = v->next) {
            v->isRented = rentedVehicles.count(v->id);
        }
    }

    // File handling functions
    void loadData() {
        loadVehicleList(locationId + "_vehicles.csv");
        loadCustomerList(locationId + "_customers.csv");
        loadRentalList(locationId + "_rentals.csv");
    }

    void saveData() {
        saveVehicles(locationId + "_vehicles.csv");
        saveCustomers(locationId + "_customers.csv");
        saveRentals(locationId + "_rentals.csv");
    }

    void loadVehicleList(const string &filename) {
        ifstream f(filename);
        if (!f) return;
        string line;
        getline(f, line); // header
        while (getline(f, line)) {
            stringstream ss(line);
            string id, make, model, type, year, rate, rented;
            getline(ss, id, ',');
            getline(ss, make, ',');
            getline(ss, model, ',');
            getline(ss, type, ',');
            getline(ss, year, ',');
            getline(ss, rate, ',');
            getline(ss, rented, ',');
            vehicles = new Vehicle{id, make, model, type, stoi(year), stod(rate), rented == "1", vehicles};
        }
    }

    void loadCustomerList(const string &filename) {
        ifstream f(filename);
        if (!f) return;
        string line;
        getline(f, line); // header
        while (getline(f, line)) {
            stringstream ss(line);
            string id, name, contact;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, contact, ',');
            customers = new Customer{id, name, contact, customers};
        }
    }

    void loadRentalList(const string &filename) {
        ifstream f(filename);
        if (!f) return;
        string line;
        getline(f, line); // header
        while (getline(f, line)) {
            stringstream ss(line);
            string id, vehicleId, customerId, pickupDate, returnDate, pickupLoc, returnLoc, total;
            getline(ss, id, ',');
            getline(ss, vehicleId, ',');
            getline(ss, customerId, ',');
            getline(ss, pickupDate, ',');
            getline(ss, returnDate, ',');
            getline(ss, pickupLoc, ',');
            getline(ss, returnLoc, ',');
            getline(ss, total, ',');
            rentals = new Rental{stoi(id), vehicleId, customerId, pickupDate, returnDate, 
                                pickupLoc, returnLoc, stod(total), rentals};
        }
    }

    void saveVehicles(const string &filename) {
        ofstream f(filename);
        f << "id,make,model,type,year,daily_rate,is_rented\n";
        for (auto *v = vehicles; v; v = v->next) {
            f << v->id << ',' << v->make << ',' << v->model << ',' 
              << v->type << ',' << v->year << ',' << v->dailyRate << ','
              << (v->isRented ? 1 : 0) << "\n";
        }
    }

    void saveCustomers(const string &filename) {
        ofstream f(filename);
        f << "id,name,contact\n";
        for (auto *c = customers; c; c = c->next) {
            f << c->id << ',' << c->name << ',' << c->contact << "\n";
        }
    }

    void saveRentals(const string &filename) {
        ofstream f(filename);
        f << "id,vehicle_id,customer_id,pickup_date,return_date,pickup_location,return_location,total_charge\n";
        for (auto *r = rentals; r; r = r->next) {
            f << r->id << ',' << r->vehicleId << ',' << r->customerId << ',' 
              << r->pickupDate << ',' << r->returnDate << ',' 
              << r->pickupLocation << ',' << r->returnLocation << ',' 
              << r->totalCharge << "\n";
        }
    }
};

// ======== RentalNetwork Class ========
class RentalNetwork {
private:
    unordered_map<string, RentalLocation*> locations;
    LocationGraph locationGraph;
    int nextLocationId = 1;

public:
    RentalNetwork() {
        locationGraph.loadFromFile("locations_graph.csv");
    }

    ~RentalNetwork() {
        locationGraph.saveToFile("locations_graph.csv");
        for (auto &kv : locations) {
            delete kv.second;
        }
    }

    void addLocation() {
        cout << "Location Name: ";
        string name;
        getline(cin, name);
        cout << "Address: ";
        string addr;
        getline(cin, addr);
        string id = "LOC" + to_string(nextLocationId++);
        locations[id] = new RentalLocation(id, name, addr);
        locationGraph.addLocation(id);
        cout << "Added Location: " << id << "\n";
    }

    void connectLocations() {
        cout << "From Location ID: ";
        string a;
        getline(cin, a);
        cout << "To Location ID: ";
        string b;
        getline(cin, b);
        if (locations.find(a) == locations.end() || locations.find(b) == locations.end()) {
            cout << "Invalid location IDs\n";
            return;
        }
        int dist = readInt("Distance (km): ", 1);
        locationGraph.connectLocations(a, b, dist);
        cout << "Locations connected\n";
    }

    void findShortestPath() {
        cout << "Start Location ID: ";
        string start;
        getline(cin, start);
        cout << "End Location ID: ";
        string end;
        getline(cin, end);
        vector<string> path = locationGraph.shortestPath(start, end);
        if (path.empty()) {
            cout << "No path exists between these locations\n";
        } else {
            cout << "Shortest path (" << path.size()-1 << " segments):\n";
            for (const string &loc : path) {
                cout << " - " << locations[loc]->name << " (" << loc << ")\n";
            }
        }
    }

    RentalLocation* getLocation(const string &id) {
        if (locations.find(id) != locations.end()) {
            return locations[id];
        }
        return nullptr;
    }

    void listLocations() {
        cout << "-- Rental Locations --\n";
        for (const auto &kv : locations) {
            cout << kv.first << " | " << kv.second->name << " | " << kv.second->address << "\n";
        }
    }
};

// ======== Main Menu ========
int main() {
    RentalNetwork rn;
    while (true) {
        cout << "\n=== Car Rental Management System ===\n"
             << "1. Add Rental Location\n"
             << "2. Connect Locations\n"
             << "3. Find Shortest Path Between Locations\n"
             << "4. Manage Location\n"
             << "5. List Locations\n"
             << "6. Exit\n";
        int choice = readInt("Choose: ", 1, 6);
        
        if (choice == 6) break;
        
        switch (choice) {
            case 1: 
                rn.addLocation(); 
                break;
            case 2:
                rn.connectLocations();
                break;
            case 3:
                rn.findShortestPath();
                break;
            case 4: {
                cout << "Location ID: ";
                string locId;
                getline(cin, locId);
                RentalLocation* loc = rn.getLocation(locId);
                if (!loc) {
                    cout << "Location not found!\n";
                    break;
                }
                
                // Location management submenu
                while (true) {
                    cout << "\n=== Managing " << loc->name << " ===\n"
                         << "1. Add Vehicle\n"
                         << "2. Register Customer\n"
                         << "3. Rent Vehicle\n"
                         << "4. Return Vehicle\n"
                         << "5. View Vehicles\n"
                         << "6. View Customers\n"
                         << "7. View Rentals\n"
                         << "8. Back to Main Menu\n";
                    int subChoice = readInt("Choose: ", 1, 8);
                    
                    if (subChoice == 8) break;
                    
                    switch (subChoice) {
                        case 1: {
                            cout << "License Plate: ";
                            string plate;
                            getline(cin, plate);
                            cout << "Make: ";
                            string make;
                            getline(cin, make);
                            cout << "Model: ";
                            string model;
                            getline(cin, model);
                            cout << "Type: ";
                            string type;
                            getline(cin, type);
                            int year = readInt("Year: ", 1900, 2023);
                            double rate = readInt("Daily Rate: $", 0);
                            loc->addVehicle(plate, make, model, type, year, rate);
                            break;
                        }
                        case 2: {
                            cout << "Driver's License #: ";
                            string dl;
                            getline(cin, dl);
                            cout << "Name: ";
                            string name;
                            getline(cin, name);
                            cout << "Contact: ";
                            string contact;
                            getline(cin, contact);
                            loc->registerCustomer(dl, name, contact);
                            break;
                        }
                        case 3: {
                            cout << "Customer DL #: ";
                            string custId;
                            getline(cin, custId);
                            cout << "Vehicle Plate #: ";
                            string vehId;
                            getline(cin, vehId);
                            cout << "Pickup Location: ";
                            string pickupLoc;
                            getline(cin, pickupLoc);
                            int days = readInt("Rental Days: ", 1);
                            int result = loc->rentVehicle(custId, vehId, pickupLoc, days);
                            if (result == -1) cout << "Invalid customer or vehicle ID\n";
                            else if (result == -2) cout << "Vehicle is already rented\n";
                            else cout << "Vehicle rented successfully. Rental ID: " << result << "\n";
                            break;
                        }
                        case 4: {
                            int rentalId = readInt("Rental ID to return: ", 1);
                            cout << "Return Location: ";
                            string returnLoc;
                            getline(cin, returnLoc);
                            cout << "Is the vehicle damaged? (1 for yes, 0 for no): ";
                            bool damaged = readInt("", 0, 1);
                            if (loc->returnVehicle(rentalId, returnLoc, damaged)) {
                                cout << "Vehicle returned successfully\n";
                            } else {
                                cout << "Invalid rental ID or vehicle already returned\n";
                            }
                            break;
                        }
                        case 5: {
                            cout << "Show only available vehicles? (1 for yes, 0 for no): ";
                            bool availOnly = readInt("", 0, 1);
                            loc->displayVehicles(availOnly);
                            break;
                        }
                        case 6:
                            loc->displayCustomers();
                            break;
                        case 7: {
                            cout << "Show only current rentals? (1 for yes, 0 for no): ";
                            bool currentOnly = readInt("", 0, 1);
                            loc->displayRentals(currentOnly);
                            break;
                        }
                    }
                }
                break;
            }
            case 5:
                rn.listLocations();
                break;
        }
    }
    return 0;
}   