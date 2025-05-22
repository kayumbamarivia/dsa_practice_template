#include "hospital_network.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Constructor
HospitalNetwork::HospitalNetwork() {
    loadData();
}

// Destructor
HospitalNetwork::~HospitalNetwork() {
    saveData();
}

// CRUD Operations
bool HospitalNetwork::addHospital(const std::string& id, const std::string& name, 
                                const std::string& location, int patientCount) {
    if (!Hospital::isValidId(id)) {
        std::cout << "Error: Invalid hospital ID format. Must start with 'H' followed by numbers.\n";
        return false;
    }
    
    if (hospitalExists(id)) {
        std::cout << "Error: Hospital ID already exists.\n";
        return false;
    }
    
    if (!Hospital::isValidPatientCount(patientCount)) {
        std::cout << "Error: Patient count must be non-negative.\n";
        return false;
    }
    
    hospitals[id] = Hospital(id, name, location, patientCount);
    std::cout << "Hospital added successfully!\n";
    return true;
}

bool HospitalNetwork::updateHospital(const std::string& id, const std::string& name, 
                                   const std::string& location, int patientCount) {
    if (!hospitalExists(id)) {
        std::cout << "Error: Hospital ID not found.\n";
        return false;
    }
    
    if (!Hospital::isValidPatientCount(patientCount)) {
        std::cout << "Error: Patient count must be non-negative.\n";
        return false;
    }
    
    hospitals[id].setName(name);
    hospitals[id].setLocation(location);
    hospitals[id].setPatientCount(patientCount);
    std::cout << "Hospital updated successfully!\n";
    return true;
}

bool HospitalNetwork::deleteHospital(const std::string& id) {
    if (!hospitalExists(id)) {
        std::cout << "Error: Hospital ID not found.\n";
        return false;
    }
    
    // Remove all connections involving this hospital
    for (auto& pair : hospitals) {
        if (pair.second.hasConnection(id)) {
            pair.second.removeConnection(id);
        }
    }
    
    hospitals.erase(id);
    std::cout << "Hospital deleted successfully!\n";
    return true;
}

Hospital* HospitalNetwork::getHospital(const std::string& id) {
    auto it = hospitals.find(id);
    return (it != hospitals.end()) ? &(it->second) : nullptr;
}

// Connection Management
bool HospitalNetwork::addConnection(const std::string& id1, const std::string& id2, 
                                  const std::string& description) {
    if (!hospitalExists(id1) || !hospitalExists(id2)) {
        std::cout << "Error: One or both hospital IDs not found.\n";
        return false;
    }
    
    if (connectionExists(id1, id2)) {
        std::cout << "Error: Connection already exists.\n";
        return false;
    }
    
    // Add bidirectional connection
    hospitals[id1].addConnection(id2, description);
    hospitals[id2].addConnection(id1, description);
    std::cout << "Connection added successfully!\n";
    return true;
}

bool HospitalNetwork::removeConnection(const std::string& id1, const std::string& id2) {
    if (!connectionExists(id1, id2)) {
        std::cout << "Error: Connection does not exist.\n";
        return false;
    }
    
    // Remove bidirectional connection
    hospitals[id1].removeConnection(id2);
    hospitals[id2].removeConnection(id1);
    std::cout << "Connection removed successfully!\n";
    return true;
}

// Display Methods
void HospitalNetwork::displayAllHospitals() const {
    std::cout << "\n=== Hospital List ===\n";
    std::cout << std::left << std::setw(5) << "ID" << " | "
              << std::setw(20) << "Name" << " | "
              << std::setw(20) << "Location" << " | "
              << std::setw(10) << "Patients" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& pair : hospitals) {
        std::cout << pair.second.toString() << "\n";
    }
}

void HospitalNetwork::displayConnections() const {
    std::cout << "\n=== Hospital Connections ===\n";
    std::cout << std::left << std::setw(15) << "Hospital" << " | "
              << std::setw(30) << "Connected To" << " | "
              << std::setw(20) << "Description" << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& pair : hospitals) {
        const Hospital& hospital = pair.second;
        const auto& connections = hospital.getConnections();
        
        if (connections.empty()) {
            std::cout << std::setw(15) << hospital.getId() << " | "
                      << std::setw(30) << "None" << " | "
                      << std::setw(20) << "N/A" << "\n";
        } else {
            bool first = true;
            for (const auto& conn : connections) {
                if (first) {
                    std::cout << std::setw(15) << hospital.getId() << " | ";
                    first = false;
                } else {
                    std::cout << std::setw(15) << " " << " | ";
                }
                std::cout << std::setw(30) << conn.first << " | "
                         << std::setw(20) << conn.second << "\n";
            }
        }
    }
}

// Validation Methods
bool HospitalNetwork::hospitalExists(const std::string& id) const {
    return hospitals.find(id) != hospitals.end();
}

bool HospitalNetwork::connectionExists(const std::string& id1, const std::string& id2) const {
    auto it = hospitals.find(id1);
    return (it != hospitals.end()) && it->second.hasConnection(id2);
}

// Predefined Scenario Setup
void HospitalNetwork::setupPredefinedScenario() {
    // Add hospitals
    std::string hospitals[6] = {"H1", "H2", "H3", "H4", "H5", "H6"};
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
    std::ifstream file(HOSPITALS_FILE);
    if (!file.is_open()) return false;
    
    std::string line;
    // Skip header
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, name, location, patientCountStr;
        
        std::getline(ss, id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, location, ',');
        std::getline(ss, patientCountStr, ',');
        
        int patientCount = std::stoi(patientCountStr);
        addHospital(id, name, location, patientCount);
    }
    
    return true;
}

bool HospitalNetwork::saveHospitals() {
    std::ofstream file(HOSPITALS_FILE);
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
    std::ifstream file(GRAPH_FILE);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string hospitalId, connections;
        
        std::getline(ss, hospitalId, ',');
        std::getline(ss, connections);
        
        std::stringstream connStream(connections);
        std::string connection;
        while (std::getline(connStream, connection, ',')) {
            std::stringstream connSS(connection);
            std::string connectedId, description;
            
            std::getline(connSS, connectedId, ':');
            std::getline(connSS, description);
            
            addConnection(hospitalId, connectedId, description);
        }
    }
    
    return true;
}

bool HospitalNetwork::saveConnections() {
    std::ofstream file(GRAPH_FILE);
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
    std::ofstream file(RELATIONSHIPS_FILE);
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