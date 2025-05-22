#include "hospital.h"
#include <sstream>
#include <iomanip>

// Default constructor
Hospital::Hospital() : patientCount(0) {}

// Parameterized constructor
Hospital::Hospital(const std::string& id, const std::string& name, 
                  const std::string& location, int patientCount)
    : id(id), name(name), location(location), patientCount(patientCount) {}

// Getters
std::string Hospital::getId() const { return id; }
std::string Hospital::getName() const { return name; }
std::string Hospital::getLocation() const { return location; }
int Hospital::getPatientCount() const { return patientCount; }
std::map<std::string, std::string> Hospital::getConnections() const { return connections; }

// Setters
void Hospital::setName(const std::string& name) { this->name = name; }
void Hospital::setLocation(const std::string& location) { this->location = location; }
void Hospital::setPatientCount(int count) { this->patientCount = count; }

// Connection management
void Hospital::addConnection(const std::string& hospitalId, const std::string& description) {
    connections[hospitalId] = description;
}

void Hospital::removeConnection(const std::string& hospitalId) {
    connections.erase(hospitalId);
}

bool Hospital::hasConnection(const std::string& hospitalId) const {
    return connections.find(hospitalId) != connections.end();
}

std::string Hospital::getConnectionDescription(const std::string& hospitalId) const {
    auto it = connections.find(hospitalId);
    return (it != connections.end()) ? it->second : "";
}

// Data validation
bool Hospital::isValidId(const std::string& id) {
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
std::string Hospital::toString() const {
    std::stringstream ss;
    ss << std::left << std::setw(5) << id << " | "
       << std::setw(20) << name << " | "
       << std::setw(20) << location << " | "
       << std::setw(10) << patientCount;
    return ss.str();
}

std::string Hospital::toCSV() const {
    std::stringstream ss;
    ss << id << "," << name << "," << location << "," << patientCount;
    return ss.str();
} 