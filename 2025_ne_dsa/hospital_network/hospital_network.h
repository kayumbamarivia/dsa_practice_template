/**
 * Hospital Network Class Header
 * 
 * This class manages the entire hospital network, including:
 * - CRUD operations for hospitals
 * - Graph connectivity between hospitals
 * - File I/O operations
 * - Data validation and error handling
 */

#ifndef HOSPITAL_NETWORK_H
#define HOSPITAL_NETWORK_H

#include "hospital.h"
#include <map>
#include <string>
#include <fstream>

class HospitalNetwork {
private:
    // Map to store hospitals (key: hospital ID, value: Hospital object)
    std::map<std::string, Hospital> hospitals;
    
    // File paths
    const std::string HOSPITALS_FILE = "hospitals.csv";
    const std::string GRAPH_FILE = "graph.txt";
    const std::string RELATIONSHIPS_FILE = "relationships.csv";
    
    // Helper methods
    bool loadHospitals();
    bool saveHospitals();
    bool loadConnections();
    bool saveConnections();
    void exportRelationships();
    
public:
    // Constructor and destructor
    HospitalNetwork();
    ~HospitalNetwork();
    
    // CRUD operations
    bool addHospital(const std::string& id, const std::string& name, 
                    const std::string& location, int patientCount);
    bool updateHospital(const std::string& id, const std::string& name, 
                       const std::string& location, int patientCount);
    bool deleteHospital(const std::string& id);
    Hospital* getHospital(const std::string& id);
    
    // Connection management
    bool addConnection(const std::string& id1, const std::string& id2, 
                      const std::string& description);
    bool removeConnection(const std::string& id1, const std::string& id2);
    
    // Display methods
    void displayAllHospitals() const;
    void displayConnections() const;
    
    // Validation methods
    bool hospitalExists(const std::string& id) const;
    bool connectionExists(const std::string& id1, const std::string& id2) const;
    
    // Predefined scenario setup
    void setupPredefinedScenario();
    
    // File operations
    bool loadData();
    bool saveData();
};

#endif // HOSPITAL_NETWORK_H 