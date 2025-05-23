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
#include <iomanip>
#include <cmath>

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

// ======== Enhanced Graph Algorithms ========
class LocationGraph {
private:
    unordered_map<string, vector<pair<string, int>>> adj; // location -> [(neighbor, distance)]
    unordered_map<string, pair<double, double>> coordinates; // location -> (latitude, longitude)
    
public:
    const vector<pair<string, int>>& getAdjacent(const string& location) const {
        static const vector<pair<string, int>> empty; // Return empty if not found
        auto it = adj.find(location);
        return it != adj.end() ? it->second : empty;
    }

    void addLocation(const string &location, double lat = 0.0, double lon = 0.0) {
        adj[location] = vector<pair<string, int>>();
        coordinates[location] = {lat, lon};
    }

    void connectLocations(const string &a, const string &b, int distance) {
        adj[a].push_back({b, distance});
        adj[b].push_back({a, distance});
    }

    // Dijkstra's algorithm for shortest path
    pair<vector<string>, int> shortestPath(const string &start, const string &end) {
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
        if (dist[end] == INT_MAX) return {path, -1};

        for (string at = end; at != ""; at = prev[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        return {path, dist[end]};
    }

    // Prim's algorithm for Minimum Spanning Tree
    vector<pair<pair<string, string>, int>> minimumSpanningTree() {
        unordered_set<string> visited;
        vector<pair<pair<string, string>, int>> mst;
        priority_queue<pair<int, pair<string, string>>, 
                      vector<pair<int, pair<string, string>>>, 
                      greater<pair<int, pair<string, string>>>> pq;

        if (adj.empty()) return mst;

        // Start with first node
        string start = adj.begin()->first;
        visited.insert(start);

        // Add all edges from start node
        for (const auto &edge : adj[start]) {
            pq.push({edge.second, {start, edge.first}});
        }

        while (!pq.empty() && visited.size() < adj.size()) {
            auto current = pq.top();
            pq.pop();

            string u = current.second.first;
            string v = current.second.second;
            int weight = current.first;

            if (visited.count(v)) continue;

            visited.insert(v);
            mst.push_back({{u, v}, weight});

            // Add all edges from v
            for (const auto &edge : adj[v]) {
                if (!visited.count(edge.first)) {
                    pq.push({edge.second, {v, edge.first}});
                }
            }
        }

        return mst;
    }

    // Kruskal's algorithm for Minimum Spanning Tree (alternative implementation)
    vector<pair<pair<string, string>, int>> kruskalMST() {
        vector<pair<int, pair<string, string>>> edges;
        unordered_map<string, string> parent;
        vector<pair<pair<string, string>, int>> mst;

        // Initialize parent for union-find
        for (const auto &kv : adj) {
            parent[kv.first] = kv.first;
            for (const auto &edge : kv.second) {
                if (kv.first < edge.first) { // Avoid duplicates
                    edges.push_back({edge.second, {kv.first, edge.first}});
                }
            }
        }

        // Sort edges by weight
        sort(edges.begin(), edges.end());

        // Union-Find functions
        auto find = [&](string u) {
            while (parent[u] != u) {
                parent[u] = parent[parent[u]];
                u = parent[u];
            }
            return u;
        };

        auto unite = [&](string u, string v) {
            string rootU = find(u);
            string rootV = find(v);
            if (rootU != rootV) {
                parent[rootV] = rootU;
                return true;
            }
            return false;
        };

        // Process edges
        for (const auto &edge : edges) {
            if (unite(edge.second.first, edge.second.second)) {
                mst.push_back({edge.second, edge.first});
                if (mst.size() == adj.size() - 1) break;
            }
        }

        return mst;
    }

    // Floyd-Warshall algorithm for all-pairs shortest paths
    unordered_map<string, unordered_map<string, int>> allPairsShortestPaths() {
        unordered_map<string, unordered_map<string, int>> dist;

        // Initialize distances
        for (const auto &u : adj) {
            for (const auto &v : adj) {
                if (u.first == v.first) {
                    dist[u.first][v.first] = 0;
                } else {
                    dist[u.first][v.first] = INT_MAX;
                }
            }
            for (const auto &edge : u.second) {
                dist[u.first][edge.first] = edge.second;
            }
        }

        // Floyd-Warshall
        for (const auto &k : adj) {
            for (const auto &i : adj) {
                for (const auto &j : adj) {
                    if (dist[i.first][k.first] != INT_MAX && 
                        dist[k.first][j.first] != INT_MAX &&
                        dist[i.first][j.first] > dist[i.first][k.first] + dist[k.first][j.first]) {
                        dist[i.first][j.first] = dist[i.first][k.first] + dist[k.first][j.first];
                    }
                }
            }
        }

        return dist;
    }

    // Betweenness Centrality (Brandes' algorithm)
    unordered_map<string, double> betweennessCentrality() {
        unordered_map<string, double> centrality;
        
        for (const auto &v : adj) {
            centrality[v.first] = 0.0;
        }

        for (const auto &s : adj) {
            stack<string> S;
            unordered_map<string, vector<string>> P;
            unordered_map<string, int> sigma;
            unordered_map<string, int> d;
            unordered_map<string, double> delta;

            for (const auto &v : adj) {
                P[v.first] = vector<string>();
                sigma[v.first] = 0;
                d[v.first] = -1;
            }

            sigma[s.first] = 1;
            d[s.first] = 0;

            queue<string> Q;
            Q.push(s.first);

            while (!Q.empty()) {
                string v = Q.front();
                Q.pop();
                S.push(v);

                for (const auto &w_pair : adj[v]) {
                    string w = w_pair.first;
                    if (d[w] < 0) {
                        Q.push(w);
                        d[w] = d[v] + 1;
                    }
                    if (d[w] == d[v] + 1) {
                        sigma[w] += sigma[v];
                        P[w].push_back(v);
                    }
                }
            }

            for (const auto &v : adj) {
                delta[v.first] = 0.0;
            }

            while (!S.empty()) {
                string w = S.top();
                S.pop();

                for (const auto &v : P[w]) {
                    delta[v] += (sigma[v] * 1.0 / sigma[w]) * (1 + delta[w]);
                }

                if (w != s.first) {
                    centrality[w] += delta[w];
                }
            }
        }

        // Normalize for undirected graph
        int n = adj.size();
        if (n > 2) {
            for (auto &kv : centrality) {
                kv.second /= ((n-1)*(n-2)/2.0);
            }
        }

        return centrality;
    }

    // Transportation Analysis and Flow Forecasting (TAFFS)
    void analyzeTransportationFlow() {
        cout << "\n=== Transportation Analysis and Flow Forecasting ===\n";
        
        // 1. Critical locations (high betweenness centrality)
        auto centrality = betweennessCentrality();
        vector<pair<string, double>> sortedCentrality(centrality.begin(), centrality.end());
        sort(sortedCentrality.begin(), sortedCentrality.end(), 
             [](const pair<string, double> &a, const pair<string, double> &b) {
                 return a.second > b.second;
             });
        
        cout << "\nCritical Locations (Betweenness Centrality):\n";
        for (int i = 0; i < min(3, (int)sortedCentrality.size()); ++i) {
            cout << i+1 << ". " << sortedCentrality[i].first 
                 << " (Centrality: " << fixed << setprecision(4) << sortedCentrality[i].second << ")\n";
        }

        // 2. Network diameter
        auto allPairs = allPairsShortestPaths();
        int diameter = 0;
        for (const auto &u : allPairs) {
            for (const auto &v : u.second) {
                if (u.first != v.first && v.second != INT_MAX) {
                    diameter = max(diameter, v.second);
                }
            }
        }
        cout << "\nNetwork Diameter (longest shortest path): " << diameter << " km\n";

        // 3. Minimum Spanning Tree
        auto mst = minimumSpanningTree();
        int totalMSTWeight = 0;
        for (const auto &edge : mst) {
            totalMSTWeight += edge.second;
        }
        cout << "\nMinimum Spanning Tree Total Distance: " << totalMSTWeight << " km\n";
        cout << "MST Connections:\n";
        for (const auto &edge : mst) {
            cout << "  " << edge.first.first << " -- " << edge.first.second 
                 << " (" << edge.second << " km)\n";
        }

        // 4. Connectivity analysis
        int totalPossibleEdges = adj.size() * (adj.size() - 1) / 2;
        int actualEdges = 0;
        for (const auto &kv : adj) {
            actualEdges += kv.second.size();
        }
        actualEdges /= 2; // Undirected graph
        double connectivityRatio = (double)actualEdges / totalPossibleEdges;
        cout << "\nNetwork Connectivity: " << fixed << setprecision(2) 
             << connectivityRatio * 100 << "% (" << actualEdges 
             << " of possible " << totalPossibleEdges << " connections)\n";

        // 5. Geographic analysis (if coordinates are available)
        bool hasCoords = false;
        for (const auto &loc : coordinates) {
            if (loc.second.first != 0.0 || loc.second.second != 0.0) {
                hasCoords = true;
                break;
            }
        }

        if (hasCoords) {
            cout << "\nGeographic Analysis:\n";
            // Calculate average distance between connected locations
            double totalDistance = 0;
            int count = 0;
            for (const auto &u : adj) {
                for (const auto &v : u.second) {
                    if (u.first < v.first) { // Avoid double counting
                        auto coord1 = coordinates[u.first];
                        auto coord2 = coordinates[v.first];
                        // Simple distance calculation (approximation)
                        double dist = sqrt(pow(coord1.first - coord2.first, 2) + 
                                     pow(coord1.second - coord2.second, 2));
                        totalDistance += dist;
                        count++;
                    }
                }
            }
            if (count > 0) {
                cout << "Average geographic distance between connected locations: "
                     << fixed << setprecision(2) << (totalDistance / count) << " degrees\n";
            }
        }
    }

    void saveToFile(const string &filename) {
        ofstream f(filename);
        f << "from,to,distance,from_lat,from_lon,to_lat,to_lon\n";
        for (const auto &kv : adj) {
            for (const auto &edge : kv.second) {
                if (kv.first < edge.first) { // Avoid duplicate edges
                    auto from_coord = coordinates[kv.first];
                    auto to_coord = coordinates[edge.first];
                    f << kv.first << "," << edge.first << "," << edge.second << ","
                      << from_coord.first << "," << from_coord.second << ","
                      << to_coord.first << "," << to_coord.second << "\n";
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
            string from, to, dist, from_lat, from_lon, to_lat, to_lon;
            getline(ss, from, ',');
            getline(ss, to, ',');
            getline(ss, dist, ',');
            getline(ss, from_lat, ',');
            getline(ss, from_lon, ',');
            getline(ss, to_lat, ',');
            getline(ss, to_lon, ',');

            // Add locations if they don't exist
            if (coordinates.find(from) == coordinates.end()) {
                addLocation(from, stod(from_lat), stod(from_lon));
            }
            if (coordinates.find(to) == coordinates.end()) {
                addLocation(to, stod(to_lat), stod(to_lon));
            }

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
        
        // Get coordinates if available
        double lat = 0.0, lon = 0.0;
        cout << "Enter latitude (0 for unknown): ";
        string latStr;
        getline(cin, latStr);
        if (!latStr.empty()) lat = stod(latStr);
        
        cout << "Enter longitude (0 for unknown): ";
        string lonStr;
        getline(cin, lonStr);
        if (!lonStr.empty()) lon = stod(lonStr);
        
        string id = "LOC" + to_string(nextLocationId++);
        locations[id] = new RentalLocation(id, name, addr);
        locationGraph.addLocation(id, lat, lon);
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
        
        auto result = locationGraph.shortestPath(start, end);
        vector<string> path = result.first;
        int distance = result.second;
        
        if (path.empty()) {
            cout << "No path exists between these locations\n";
        } else {
            cout << "Shortest path (" << distance << " km):\n";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << " " << (i+1) << ". " << locations[path[i]]->name << " (" << path[i] << ")";
                if (i < path.size() - 1) {
                    // Find the distance between this and next location
                    int segment_dist = 0;
                    for (const auto &edge : locationGraph.getAdjacent(path[i])) {
                        if (edge.first == path[i+1]) {
                            segment_dist = edge.second;
                            break;
                        }
                    }
                    cout << " --[" << segment_dist << " km]-->";
                }
                cout << "\n";
            }
        }
    }

    void analyzeTransportationNetwork() {
        locationGraph.analyzeTransportationFlow();
    }

    void displayMinimumSpanningTree() {
        auto mst = locationGraph.minimumSpanningTree();
        if (mst.empty()) {
            cout << "No locations in the network\n";
            return;
        }
        
        cout << "\nMinimum Spanning Tree for Rental Network:\n";
        int totalDistance = 0;
        for (const auto &edge : mst) {
            cout << "  " << locations[edge.first.first]->name << " (" << edge.first.first << ") -- " 
                 << locations[edge.first.second]->name << " (" << edge.first.second << ") "
                 << "[" << edge.second << " km]\n";
            totalDistance += edge.second;
        }
        cout << "Total MST Distance: " << totalDistance << " km\n";
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
             << "4. Analyze Transportation Network\n"
             << "5. Display Minimum Spanning Tree\n"
             << "6. Manage Location\n"
             << "7. List Locations\n"
             << "8. Exit\n";
        int choice = readInt("Choose: ", 1, 8);
        
        if (choice == 8) break;
        
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
            case 4:
                rn.analyzeTransportationNetwork();
                break;
            case 5:
                rn.displayMinimumSpanningTree();
                break;
            case 6: {
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
            case 7:
                rn.listLocations();
                break;
        }
    }
    return 0;
}