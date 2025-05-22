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

class Hospital {
private:
    std::string id;          // Unique identifier (e.g., H1, H2)
    std::string name;        // Hospital name
    std::string location;    // Hospital location
    int patientCount;        // Number of patients
    
    // Map to store connections with other hospitals
    // Key: Connected hospital ID, Value: Connection description
    std::map<std::string, std::string> connections;

public:
    // Constructors
    Hospital();
    Hospital(const std::string& id, const std::string& name, 
             const std::string& location, int patientCount);
    
    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getLocation() const;
    int getPatientCount() const;
    std::map<std::string, std::string> getConnections() const;
    
    // Setters
    void setName(const std::string& name);
    void setLocation(const std::string& location);
    void setPatientCount(int count);
    
    // Connection management
    void addConnection(const std::string& hospitalId, const std::string& description);
    void removeConnection(const std::string& hospitalId);
    bool hasConnection(const std::string& hospitalId) const;
    std::string getConnectionDescription(const std::string& hospitalId) const;
    
    // Data validation
    static bool isValidId(const std::string& id);
    static bool isValidPatientCount(int count);
    
    // String representation
    std::string toString() const;
    std::string toCSV() const;
};

#endif // HOSPITAL_H 