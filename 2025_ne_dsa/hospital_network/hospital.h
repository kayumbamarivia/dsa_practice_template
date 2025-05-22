/**
 * Hospital Class Header
 * 
 * This class represents a hospital in our network management system.
 * It stores all the essential information about a hospital and provides
 * methods to manage its data.
 */

#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Hospital {
private:
    string id;          // Unique identifier (e.g., H1, H2)
    string name;        // Hospital name
    string location;    // Hospital location
    int patientCount;        // Number of patients
    
    // Map to store connections with other hospitals
    // Key: Connected hospital ID, Value: Connection description
    map<string, string> connections;

public:
    // Constructors
    Hospital();
    Hospital(const string& id, const string& name, 
             const string& location, int patientCount);
    
    // Getters
    string getId() const;
    string getName() const;
    string getLocation() const;
    int getPatientCount() const;
    map<string, string> getConnections() const;
    
    // Setters
    void setName(const string& name);
    void setLocation(const string& location);
    void setPatientCount(int count);
    
    // Connection management
    void addConnection(const string& hospitalId, const string& description);
    void removeConnection(const string& hospitalId);
    void clearConnections();
    bool hasConnection(const string& hospitalId) const;
    string getConnectionDescription(const string& hospitalId) const;
    
    // Data validation
    static bool isValidId(const string& id);
    static bool isValidPatientCount(int count);
    
    // String representation
    string toString() const;
    string toCSV() const;
};

#endif // HOSPITAL_H 