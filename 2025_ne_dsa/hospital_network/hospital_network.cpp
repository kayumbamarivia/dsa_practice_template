#include "hospital_network.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

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
                                  const string& description) {
    if (!hospitalExists(id1) || !hospitalExists(id2)) {
        cout << "Error: One or both hospital IDs not found.\n";
        return false;
    }
    
    if (connectionExists(id1, id2)) {
        cout << "Error: Connection already exists.\n";
        return false;
    }
    
    // Add bidirectional connection
    hospitals[id1].addConnection(id2, description);
    hospitals[id2].addConnection(id1, description);
    cout << "Connection added successfully!\n";
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
    addConnection("H1", "H6", "Emergency backup");
    addConnection("H1", "H4", "Referral support");
    addConnection("H2", "H3", "Standard route");
    addConnection("H6", "H5", "Emergency support");
    addConnection("H5", "H4", "Ambulance path");
    addConnection("H4", "H1", "Referral support");
    addConnection("H2", "H3", "Standard route");
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