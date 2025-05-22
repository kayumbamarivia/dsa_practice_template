/**
 * @file main.cpp
 * @brief Main implementation file for the Hospital Network Management System
 * 
 * @details This file contains the main program logic and user interface for managing
 * a network of hospitals and their relationships.
 * 
 * Required Libraries:
 * - hospital_network.h: Custom header containing HospitalNetwork class definition
 *   and core functionality for managing hospital relationships
 * 
 * - iostream: Standard C++ library for input/output operations
 *   Provides console I/O functionality through cin and cout
 * 
 * - limits: Standard C++ library for numeric limits
 *   Used for input validation and buffer management
 */

#include "hospital_network.h"
#include <iostream>
#include <limits>

using namespace std;

// Function to clear input buffer
void clearInputBuffer() {
   cin.clear();
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to get a valid integer input
int getIntInput() {
    int value;
    while (!(cin >> value)) {
       cout << "Error: Please enter a valid number.\n";
        clearInputBuffer();
    }
    clearInputBuffer();
    return value;
}

// Function to display the main menu
void displayMenu() {
   cout << "\n=== Hospital Network Management System ===\n";
   cout << "1. Add Hospital\n";
   cout << "2. Update Hospital\n";
   cout << "3. Delete Hospital\n";
   cout << "4. Display All Hospitals\n";
   cout << "5. Add Connection\n";
   cout << "6. Remove Connection\n";
   cout << "7. Display Connections\n";
   cout << "8. Setup Predefined Scenario\n";
   cout << "9. Export Relationships\n";
   cout << "10. Display Distances\n";
   cout << "11. Find Shortest Path\n";
   cout << "12. Find Longest Path\n";
   cout << "13. Generate Network Diagram\n";
   cout << "14. Delete All Hospitals\n";
   cout << "15. Delete All Connections\n";
   cout << "0. Exit\n";
   cout << "Enter your choice: ";
}

// Function to handle hospital addition
void handleAddHospital(HospitalNetwork& network) {
   string id, name, location;
    int patientCount;
    
   cout << "\nEnter Hospital ID (e.g., H1): ";
   getline(cin, id);
    
   cout << "Enter Hospital Name: ";
   getline(cin, name);
    
   cout << "Enter Location: ";
   getline(cin, location);
    
   cout << "Enter Number of Patients: ";
    patientCount = getIntInput();
    
    network.addHospital(id, name, location, patientCount);
}

// Function to handle hospital update
void handleUpdateHospital(HospitalNetwork& network) {
   string id, name, location;
    int patientCount;
    
   cout << "\nEnter Hospital ID to update: ";
   getline(cin, id);
    
   cout << "Enter new Hospital Name: ";
   getline(cin, name);
    
   cout << "Enter new Location: ";
   getline(cin, location);
    
   cout << "Enter new Number of Patients: ";
    patientCount = getIntInput();
    
    network.updateHospital(id, name, location, patientCount);
}

// Function to handle hospital deletion
void handleDeleteHospital(HospitalNetwork& network) {
   string id;
    
   cout << "\nEnter Hospital ID to delete: ";
   getline(cin, id);
    
    network.deleteHospital(id);
}

// Function to handle connection addition
void handleAddConnection(HospitalNetwork& network) {
   string id1, id2, description;
   double distance;
    
   cout << "\nEnter first Hospital ID: ";
   getline(cin, id1);
    
   cout << "Enter second Hospital ID: ";
   getline(cin, id2);
    
   cout << "Enter connection description: ";
   getline(cin, description);
    
   cout << "Enter distance between hospitals: ";
   distance = getIntInput();
    
    network.addConnection(id1, id2, description, distance);
}

// Function to handle connection removal
void handleRemoveConnection(HospitalNetwork& network) {
   string id1, id2;
    
   cout << "\nEnter first Hospital ID: ";
   getline(cin, id1);
    
   cout << "Enter second Hospital ID: ";
   getline(cin, id2);
    
    network.removeConnection(id1, id2);
}

// Function to handle shortest path finding
void handleShortestPath(HospitalNetwork& network) {
    string start, end;
    
    cout << "\nEnter starting Hospital ID: ";
    getline(cin, start);
    
    cout << "Enter destination Hospital ID: ";
    getline(cin, end);
    
    network.displayShortestPath(start, end);
}

// Function to handle longest path finding
void handleLongestPath(HospitalNetwork& network) {
    string start, end;
    
    cout << "\nEnter starting Hospital ID: ";
    getline(cin, start);
    
    cout << "Enter destination Hospital ID: ";
    getline(cin, end);
    
    network.displayLongestPath(start, end);
}

// Function to handle delete all hospitals
void handleDeleteAllHospitals(HospitalNetwork& network) {
    char confirm;
    cout << "\nWARNING: This will delete ALL hospitals and their connections!\n";
    cout << "Are you sure you want to proceed? (y/n): ";
    cin >> confirm;
    clearInputBuffer();
    
    if (confirm == 'y' || confirm == 'Y') {
        network.deleteAllHospitals();
    } else {
        cout << "Operation cancelled.\n";
    }
}

// Function to handle delete all connections
void handleDeleteAllConnections(HospitalNetwork& network) {
    char confirm;
    cout << "\nWARNING: This will delete ALL connections between hospitals!\n";
    cout << "Are you sure you want to proceed? (y/n): ";
    cin >> confirm;
    clearInputBuffer();
    
    if (confirm == 'y' || confirm == 'Y') {
        network.deleteAllConnections();
    } else {
        cout << "Operation cancelled.\n";
    }
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
                cout << "Relationships exported to relationships.csv\n";
                break;
            case 10:
                network.displayDistances();
                break;
            case 11:
                handleShortestPath(network);
                break;
            case 12:
                handleLongestPath(network);
                break;
            case 13:
                network.generateNetworkDiagram();
                break;
            case 14:
                handleDeleteAllHospitals(network);
                break;
            case 15:
                handleDeleteAllConnections(network);
                break;
            case 0:
                cout << "Exiting program...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
    
    return 0;
} 