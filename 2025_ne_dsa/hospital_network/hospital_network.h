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

using namespace std;

class HospitalNetwork {
private:
    // Map to store hospitals (key: hospital ID, value: Hospital object)
    map<string, Hospital> hospitals;
    
    // File paths
    const string HOSPITALS_FILE = "hospitals.csv";
    const string GRAPH_FILE = "graph.txt";
    const string RELATIONSHIPS_FILE = "relationships.csv";
    
    // Helper methods
    bool loadHospitals();
    bool saveHospitals();
    bool loadConnections();
    bool saveConnections();
    
public:
    // Constructor and destructor
    HospitalNetwork();
    ~HospitalNetwork();
    
    // CRUD operations
    bool addHospital(const string& id, const string& name, 
                    const string& location, int patientCount);
    bool updateHospital(const string& id, const string& name, 
                       const string& location, int patientCount);
    bool deleteHospital(const string& id);
    Hospital* getHospital(const string& id);
    
    // Connection management
    bool addConnection(const string& id1, const string& id2, 
                      const string& description);
    bool removeConnection(const string& id1, const string& id2);
    
    // Display methods
    void displayAllHospitals() const;
    void displayConnections() const;
    
    // Validation methods
    bool hospitalExists(const string& id) const;
    bool connectionExists(const string& id1, const string& id2) const;
    
    // Predefined scenario setup
    void setupPredefinedScenario();
    
    // File operations
    bool loadData();
    bool saveData();
    void exportRelationships();
};

#endif // HOSPITAL_NETWORK_H 