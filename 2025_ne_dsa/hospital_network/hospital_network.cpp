#include "hospital_network.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <set>
#include <functional>

using namespace std;

// Constructor
HospitalNetwork::HospitalNetwork() {
    loadData();
}

// Destructor
HospitalNetwork::~HospitalNetwork() {
    saveData();
}

// CRUD Operations
bool HospitalNetwork::addHospital(const string& id, const string& name, 
                                const string& location, int patientCount) {
    if (!Hospital::isValidId(id)) {
        cout << "Error: Invalid hospital ID format. Must start with 'H' followed by numbers.\n";
        return false;
    }
    
    if (hospitalExists(id)) {
        cout << "Error: Hospital ID already exists.\n";
        return false;
    }
    
    if (!Hospital::isValidPatientCount(patientCount)) {
        cout << "Error: Patient count must be non-negative.\n";
        return false;
    }
    
    hospitals[id] = Hospital(id, name, location, patientCount);
    cout << "Hospital added successfully!\n";
    return true;
}

bool HospitalNetwork::updateHospital(const string& id, const string& name, 
                                   const string& location, int patientCount) {
    if (!hospitalExists(id)) {
        cout << "Error: Hospital ID not found.\n";
        return false;
    }
    
    if (!Hospital::isValidPatientCount(patientCount)) {
        cout << "Error: Patient count must be non-negative.\n";
        return false;
    }
    
    hospitals[id].setName(name);
    hospitals[id].setLocation(location);
    hospitals[id].setPatientCount(patientCount);
    cout << "Hospital updated successfully!\n";
    return true;
}

bool HospitalNetwork::deleteHospital(const string& id) {
    if (!hospitalExists(id)) {
        cout << "Error: Hospital ID not found.\n";
        return false;
    }
    
    // Remove all connections involving this hospital
    for (auto& pair : hospitals) {
        if (pair.second.hasConnection(id)) {
            pair.second.removeConnection(id);
        }
    }
    
    hospitals.erase(id);
    cout << "Hospital deleted successfully!\n";
    return true;
}

Hospital* HospitalNetwork::getHospital(const string& id) {
    auto it = hospitals.find(id);
    return (it != hospitals.end()) ? &(it->second) : nullptr;
}

// Connection Management
bool HospitalNetwork::addConnection(const string& id1, const string& id2, 
                                  const string& description, double distance) {
    if (!hospitalExists(id1) || !hospitalExists(id2)) {
        cout << "Error: One or both hospitals do not exist.\n";
        return false;
    }
    
    // Add connection to both hospitals
    hospitals[id1].addConnection(id2, description);
    hospitals[id2].addConnection(id1, description);
    
    // Store distance
    distances[make_pair(id1, id2)] = distance;
    distances[make_pair(id2, id1)] = distance;
    
    return true;
}

bool HospitalNetwork::removeConnection(const string& id1, const string& id2) {
    if (!connectionExists(id1, id2)) {
        cout << "Error: Connection does not exist.\n";
        return false;
    }
    
    // Remove bidirectional connection
    hospitals[id1].removeConnection(id2);
    hospitals[id2].removeConnection(id1);
    cout << "Connection removed successfully!\n";
    return true;
}

// Display Methods
void HospitalNetwork::displayAllHospitals() const {
    cout << "\n=== Hospital List ===\n";
    cout << left << setw(5) << "ID" << " | "
              << setw(20) << "Name" << " | "
              << setw(20) << "Location" << " | "
              << setw(10) << "Patients" << "\n";
    cout << string(60, '-') << "\n";
    
    for (const auto& pair : hospitals) {
        cout << pair.second.toString() << "\n";
    }
}

void HospitalNetwork::displayConnections() const {
    cout << "\n=== Hospital Connections ===\n";
    cout << left << setw(15) << "Hospital" << " | "
              << setw(30) << "Connected To" << " | "
              << setw(20) << "Description" << "\n";
    cout << string(70, '-') << "\n";
    
    for (const auto& pair : hospitals) {
        const Hospital& hospital = pair.second;
        const auto& connections = hospital.getConnections();
        
        if (connections.empty()) {
            cout << setw(15) << hospital.getId() << " | "
                      << setw(30) << "None" << " | "
                      << setw(20) << "N/A" << "\n";
        } else {
            bool first = true;
            for (const auto& conn : connections) {
                if (first) {
                    cout << setw(15) << hospital.getId() << " | ";
                    first = false;
                } else {
                    cout << setw(15) << " " << " | ";
                }
                cout << setw(30) << conn.first << " | "
                         << setw(20) << conn.second << "\n";
            }
        }
    }
}

// Validation Methods
bool HospitalNetwork::hospitalExists(const string& id) const {
    return hospitals.find(id) != hospitals.end();
}

bool HospitalNetwork::connectionExists(const string& id1, const string& id2) const {
    auto it = hospitals.find(id1);
    return (it != hospitals.end()) && it->second.hasConnection(id2);
}

// Predefined Scenario Setup
void HospitalNetwork::setupPredefinedScenario() {
    // Add hospitals
    string hospitals[6] = {"H1", "H2", "H3", "H4", "H5", "H6"};
    for (const auto& id : hospitals) {
        addHospital(id, "Hospital " + id, "Location " + id, 100);
    }
    
    // Add connections
    addConnection("H1", "H6", "Emergency backup", 100);
    addConnection("H1", "H4", "Referral support", 50);
    addConnection("H2", "H3", "Standard route", 75);
    addConnection("H6", "H5", "Emergency support", 100);
    addConnection("H5", "H4", "Ambulance path", 50);
    addConnection("H4", "H1", "Referral support", 50);
    addConnection("H2", "H3", "Standard route", 75);
}

// File Operations
bool HospitalNetwork::loadHospitals() {
    ifstream file(HOSPITALS_FILE);
    if (!file.is_open()) return false;
    
    string line;
    // Skip header
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, location, patientCountStr;
        
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, location, ',');
        getline(ss, patientCountStr, ',');
        
        int patientCount = stoi(patientCountStr);
        addHospital(id, name, location, patientCount);
    }
    
    return true;
}

bool HospitalNetwork::saveHospitals() {
    ofstream file(HOSPITALS_FILE);
    if (!file.is_open()) return false;
    
    // Write header
    file << "ID,Name,Location,PatientCount\n";
    
    // Write data
    for (const auto& pair : hospitals) {
        file << pair.second.toCSV() << "\n";
    }
    
    return true;
}

bool HospitalNetwork::loadConnections() {
    ifstream file(GRAPH_FILE);
    if (!file.is_open()) return false;
    
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string hospitalId, connections;
        
        getline(ss, hospitalId, ',');
        getline(ss, connections);
        
        stringstream connStream(connections);
        string connection;
        while (getline(connStream, connection, ',')) {
            stringstream connSS(connection);
            string connectedId, description;
            
            getline(connSS, connectedId, ':');
            getline(connSS, description);
            
            addConnection(hospitalId, connectedId, description);
        }
    }
    
    return true;
}

bool HospitalNetwork::saveConnections() {
    ofstream file(GRAPH_FILE);
    if (!file.is_open()) return false;
    
    for (const auto& pair : hospitals) {
        const Hospital& hospital = pair.second;
        const auto& connections = hospital.getConnections();
        
        file << hospital.getId();
        for (const auto& conn : connections) {
            file << "," << conn.first << ":" << conn.second;
        }
        file << "\n";
    }
    
    return true;
}

void HospitalNetwork::exportRelationships() {
    ofstream file(RELATIONSHIPS_FILE);
    if (!file.is_open()) return;
    
    // Write header
    file << "Hospital Center,Connected Hospitals,Description\n";
    
    // Write data
    for (const auto& pair : hospitals) {
        const Hospital& hospital = pair.second;
        const auto& connections = hospital.getConnections();
        
        if (connections.empty()) {
            file << hospital.getId() << ",None,N/A\n";
        } else {
            for (const auto& conn : connections) {
                file << hospital.getId() << ","
                     << conn.first << ","
                     << conn.second << "\n";
            }
        }
    }
}

bool HospitalNetwork::loadData() {
    return loadHospitals() && loadConnections();
}

bool HospitalNetwork::saveData() {
    return saveHospitals() && saveConnections();
}

vector<string> HospitalNetwork::findShortestPath(const string& start, const string& end) {
    if (!hospitalExists(start) || !hospitalExists(end)) {
        return vector<string>();
    }
    
    map<string, double> dist;
    map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, 
                  greater<pair<double, string>>> pq;
    
    // Initialize distances
    for (const auto& hospital : hospitals) {
        dist[hospital.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        string current = pq.top().second;
        pq.pop();
        
        if (current == end) break;
        
        for (const auto& connection : hospitals[current].getConnections()) {
            string neighbor = connection.first;
            double weight = distances[make_pair(current, neighbor)];
            
            if (dist[current] + weight < dist[neighbor]) {
                dist[neighbor] = dist[current] + weight;
                prev[neighbor] = current;
                pq.push({dist[neighbor], neighbor});
            }
        }
    }
    
    // Reconstruct path
    vector<string> path;
    for (string at = end; at != ""; at = prev[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<string> HospitalNetwork::findLongestPath(const string& start, const string& end) {
    if (!hospitalExists(start) || !hospitalExists(end)) {
        return vector<string>();
    }
    
    // Use DFS to find all possible paths
    vector<string> currentPath;
    vector<string> longestPath;
    set<string> visited;
    
    function<void(const string&)> dfs = [&](const string& current) {
        if (current == end) {
            if (currentPath.size() > longestPath.size()) {
                longestPath = currentPath;
            }
            return;
        }
        
        visited.insert(current);
        for (const auto& connection : hospitals[current].getConnections()) {
            string neighbor = connection.first;
            if (visited.find(neighbor) == visited.end()) {
                currentPath.push_back(neighbor);
                dfs(neighbor);
                currentPath.pop_back();
            }
        }
        visited.erase(current);
    };
    
    currentPath.push_back(start);
    dfs(start);
    return longestPath;
}

void HospitalNetwork::displayShortestPath(const string& start, const string& end) {
    vector<string> path = findShortestPath(start, end);
    if (path.empty()) {
        cout << "No path found between " << start << " and " << end << "\n";
        return;
    }
    
    cout << "Shortest path from " << start << " to " << end << ":\n";
    double totalDistance = 0;
    for (size_t i = 0; i < path.size() - 1; i++) {
        cout << path[i] << " -> ";
        totalDistance += distances[make_pair(path[i], path[i + 1])];
    }
    cout << path.back() << "\n";
    cout << "Total distance: " << totalDistance << " units\n";
}

void HospitalNetwork::displayLongestPath(const string& start, const string& end) {
    vector<string> path = findLongestPath(start, end);
    if (path.empty()) {
        cout << "No path found between " << start << " and " << end << "\n";
        return;
    }
    
    cout << "Longest path from " << start << " to " << end << ":\n";
    double totalDistance = 0;
    for (size_t i = 0; i < path.size() - 1; i++) {
        cout << path[i] << " -> ";
        totalDistance += distances[make_pair(path[i], path[i + 1])];
    }
    cout << path.back() << "\n";
    cout << "Total distance: " << totalDistance << " units\n";
}

void HospitalNetwork::displayDistances() const {
    cout << "\nHospital Distances:\n";
    cout << string(60, '-') << "\n";
    cout << "From\tTo\tDistance\n";
    cout << string(60, '-') << "\n";
    
    for (const auto& dist : distances) {
        cout << dist.first.first << "\t" 
             << dist.first.second << "\t" 
             << dist.second << "\n";
    }
}

void HospitalNetwork::generateGraphImage() {
    ofstream dotFile("hospital_network.dot");
    dotFile << "digraph HospitalNetwork {\n";
    dotFile << "    node [shape=box, style=filled, color=lightblue];\n";
    
    // Add nodes (hospitals)
    for (const auto& hospital : hospitals) {
        dotFile << "    \"" << hospital.first << "\" [label=\"" 
                << hospital.first << "\\n" << hospital.second.getName() << "\"];\n";
    }
    
    // Add edges (connections)
    for (const auto& hospital : hospitals) {
        for (const auto& connection : hospital.second.getConnections()) {
            dotFile << "    \"" << hospital.first << "\" -> \"" 
                    << connection.first << "\" [label=\"" 
                    << distances[make_pair(hospital.first, connection.first)] 
                    << "\"];\n";
        }
    }
    
    dotFile << "}\n";
    dotFile.close();
    
    // Generate image using Graphviz
    system("dot -Tpng hospital_network.dot -o hospital_network.png");
}

void HospitalNetwork::generateNetworkDiagram() {
    generateGraphImage();
    cout << "Network diagram generated as 'hospital_network.png'\n";
}

bool HospitalNetwork::deleteAllHospitals() {
    if (hospitals.empty()) {
        cout << "No hospitals to delete.\n";
        return false;
    }
    
    // Clear all hospitals and their connections
    hospitals.clear();
    distances.clear();
    cout << "All hospitals and their connections have been deleted.\n";
    return true;
}

bool HospitalNetwork::deleteAllConnections() {
    if (hospitals.empty()) {
        cout << "No hospitals exist to delete connections from.\n";
        return false;
    }
    
    // Remove all connections from each hospital
    for (auto& pair : hospitals) {
        pair.second.clearConnections();
    }
    
    // Clear the distances map
    distances.clear();
    cout << "All connections have been deleted.\n";
    return true;
} 