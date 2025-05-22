#include "hospital_network.h"
#include <iostream>
#include <limits>

// Function to clear input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to get a valid integer input
int getIntInput() {
    int value;
    while (!(std::cin >> value)) {
        std::cout << "Error: Please enter a valid number.\n";
        clearInputBuffer();
    }
    clearInputBuffer();
    return value;
}

// Function to display the main menu
void displayMenu() {
    std::cout << "\n=== Hospital Network Management System ===\n";
    std::cout << "1. Add Hospital\n";
    std::cout << "2. Update Hospital\n";
    std::cout << "3. Delete Hospital\n";
    std::cout << "4. Display All Hospitals\n";
    std::cout << "5. Add Connection\n";
    std::cout << "6. Remove Connection\n";
    std::cout << "7. Display Connections\n";
    std::cout << "8. Setup Predefined Scenario\n";
    std::cout << "9. Export Relationships\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

// Function to handle hospital addition
void handleAddHospital(HospitalNetwork& network) {
    std::string id, name, location;
    int patientCount;
    
    std::cout << "\nEnter Hospital ID (e.g., H1): ";
    std::getline(std::cin, id);
    
    std::cout << "Enter Hospital Name: ";
    std::getline(std::cin, name);
    
    std::cout << "Enter Location: ";
    std::getline(std::cin, location);
    
    std::cout << "Enter Number of Patients: ";
    patientCount = getIntInput();
    
    network.addHospital(id, name, location, patientCount);
}

// Function to handle hospital update
void handleUpdateHospital(HospitalNetwork& network) {
    std::string id, name, location;
    int patientCount;
    
    std::cout << "\nEnter Hospital ID to update: ";
    std::getline(std::cin, id);
    
    std::cout << "Enter new Hospital Name: ";
    std::getline(std::cin, name);
    
    std::cout << "Enter new Location: ";
    std::getline(std::cin, location);
    
    std::cout << "Enter new Number of Patients: ";
    patientCount = getIntInput();
    
    network.updateHospital(id, name, location, patientCount);
}

// Function to handle hospital deletion
void handleDeleteHospital(HospitalNetwork& network) {
    std::string id;
    
    std::cout << "\nEnter Hospital ID to delete: ";
    std::getline(std::cin, id);
    
    network.deleteHospital(id);
}

// Function to handle connection addition
void handleAddConnection(HospitalNetwork& network) {
    std::string id1, id2, description;
    
    std::cout << "\nEnter first Hospital ID: ";
    std::getline(std::cin, id1);
    
    std::cout << "Enter second Hospital ID: ";
    std::getline(std::cin, id2);
    
    std::cout << "Enter connection description: ";
    std::getline(std::cin, description);
    
    network.addConnection(id1, id2, description);
}

// Function to handle connection removal
void handleRemoveConnection(HospitalNetwork& network) {
    std::string id1, id2;
    
    std::cout << "\nEnter first Hospital ID: ";
    std::getline(std::cin, id1);
    
    std::cout << "Enter second Hospital ID: ";
    std::getline(std::cin, id2);
    
    network.removeConnection(id1, id2);
}

int main() {
    HospitalNetwork network;
    int choice;
    
    do {
        displayMenu();
        choice = getIntInput();
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                handleAddHospital(network);
                break;
            case 2:
                handleUpdateHospital(network);
                break;
            case 3:
                handleDeleteHospital(network);
                break;
            case 4:
                network.displayAllHospitals();
                break;
            case 5:
                handleAddConnection(network);
                break;
            case 6:
                handleRemoveConnection(network);
                break;
            case 7:
                network.displayConnections();
                break;
            case 8:
                network.setupPredefinedScenario();
                break;
            case 9:
                network.exportRelationships();
                std::cout << "Relationships exported to relationships.csv\n";
                break;
            case 0:
                std::cout << "Exiting program...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
    
    return 0;
} 