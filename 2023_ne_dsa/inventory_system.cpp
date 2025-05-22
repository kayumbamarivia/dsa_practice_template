/**Inventory System
 This program allows you to manage your inventory by adding new items, listing all items, and displaying help information.
 It uses a CSV file to store the inventory data.
 Required libraries:
 - iostream: for input/output stream operations
 - fstream: for file stream operations
 - string: for string operations
 - vector: for dynamic array operations
 - algorithm: for sorting operations
 - sstream: for string stream operations
 - iomanip: for input/output manipulation operations
 - ctime: for date and time operations
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

// Structure to represent an inventory item
struct Item {
    string id;
    string name;
    int quantity;
    string registration_date;
};

// Function to convert string to lowercase
string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// Function to add a new item to the inventory
void addItem(const string& id, const string& name, int quantity, const string& reg_date) {
    ofstream file("inventory.csv", ios::app);
    if (file.is_open()) {
        file << id << "," << name << "," << quantity << "," << reg_date << "\n";
        file.close();
        cout << "Item added successfully!\n";
    } else {
        cout << "Error opening file!\n";
    }
}

// Function to read and display all items in alphabetical order
void listItems() {
    vector<Item> items;
    ifstream file("inventory.csv");
    string line;

    // Read all items from CSV
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, quantity_str, date;
        
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, quantity_str, ',');
        getline(ss, date, ',');

        Item item = {id, name, stoi(quantity_str), date};
        items.push_back(item);
    }
    file.close();

    // Sort items by name
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return toLower(a.name) < toLower(b.name);
    });

    // Display items in formatted table
    cout << "\n| Item ID\t| Item Name\t\t| Quantity\t| Reg Date\t|\n";
    cout << "|-----------|-----------------------|-----------|------------------|\n";
    
    for (const auto& item : items) {
        cout << "| " << setw(10) << item.id << "\t| " 
             << setw(20) << item.name << "\t| " 
             << setw(10) << item.quantity << "\t| " 
             << setw(10) << item.registration_date << "\t|\n";
    }
    cout << "\n";
}

// Function to display help information
void showHelp() {
    cout << "\nCommands syntaxes:\n";
    cout << "itemadd <item_id> <item_name> <quantity> <registration_date>\n";
    cout << "itemslist\n";
    cout << "help\n";
    cout << "exit\n\n";
}

// Function to process user commands
void processCommand(const string& command) {
    string cmd = toLower(command);
    
    if (cmd == "help") {
        showHelp();
    }
    else if (cmd == "itemslist") {
        listItems();
    }
    else if (cmd == "exit") {
        cout << "Exiting program...\n";
        exit(0);
    }
    else if (cmd.substr(0, 8) == "itemadd ") {
        stringstream ss(command.substr(8));
        string id, name, quantity_str, date;
        
        if (ss >> id >> name >> quantity_str >> date) {
            try {
                int quantity = stoi(quantity_str);
                addItem(id, name, quantity, date);
            } catch (...) {
                cout << "Invalid quantity format!\n";
            }
        } else {
            cout << "Invalid itemadd command format!\n";
        }
    }
    else {
        cout << "Unknown command! Type 'help' for available commands.\n";
    }
}

int main() {
    cout << "Welcome to RCA Inventory System\n";
    cout << "Type 'help' for available commands\n\n";

    string command;
    while (true) {
        cout << "> ";
        getline(cin, command);
        processCommand(command);
    }

    return 0;
} 