#include "hospital.h"
#include <sstream>
#include <iomanip>

using namespace std;

// Default constructor
Hospital::Hospital() : patientCount(0) {}

// Parameterized constructor
Hospital::Hospital(const string& id, const string& name, 
                  const string& location, int patientCount)
    : id(id), name(name), location(location), patientCount(patientCount) {}

// Getters
string Hospital::getId() const { return id; }
string Hospital::getName() const { return name; }
string Hospital::getLocation() const { return location; }
int Hospital::getPatientCount() const { return patientCount; }
map<string, string> Hospital::getConnections() const { return connections; }

// Setters
void Hospital::setName(const string& name) { this->name = name; }
void Hospital::setLocation(const string& location) { this->location = location; }
void Hospital::setPatientCount(int count) { this->patientCount = count; }

// Connection management
void Hospital::addConnection(const string& hospitalId, const string& description) {
    connections[hospitalId] = description;
}

void Hospital::removeConnection(const string& hospitalId) {
    connections.erase(hospitalId);
}

bool Hospital::hasConnection(const string& hospitalId) const {
    return connections.find(hospitalId) != connections.end();
}

string Hospital::getConnectionDescription(const string& hospitalId) const {
    auto it = connections.find(hospitalId);
    return (it != connections.end()) ? it->second : "";
}

// Data validation
bool Hospital::isValidId(const string& id) {
    if (id.length() < 2) return false;
    if (id[0] != 'H') return false;
    for (size_t i = 1; i < id.length(); i++) {
        if (!isdigit(id[i])) return false;
    }
    return true;
}

bool Hospital::isValidPatientCount(int count) {
    return count >= 0;
}

// String representation
string Hospital::toString() const {
    stringstream ss;
    ss << left << setw(5) << id << " | "
       << setw(20) << name << " | "
       << setw(20) << location << " | "
       << setw(10) << patientCount;
    return ss.str();
}

string Hospital::toCSV() const {
    stringstream ss;
    ss << id << "," << name << "," << location << "," << patientCount;
    return ss.str();
} 
