#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <set>
#include <algorithm>
#include <cctype>
#include <climits>

using namespace std;

const int MAX = 1000; // Maximum number of health centers

// Struct for Date
struct Date {
    int day, month, year;
};

// Struct for Health Center
struct HealthCenter {
    int id;
    string name;
    string district;
    float lat, lon;
    int capacity;
    Date established_date;
};

// Struct for Connection
struct Connection {
    int to;
    float distance;
    int time;
    string description;
};

// Global variables
vector<HealthCenter> centers;
vector<Connection> adjList[MAX];

// Utility functions for input validation
bool isValidID(int id) {
    return id >= 0 && id < MAX;
}

bool isUniqueID(int id) {
    for (const auto& center : centers) {
        if (center.id == id) return false;
    }
    return true;
}

bool isValidName(const string& name) {
    return !name.empty() && all_of(name.begin(), name.end(), [](char c) {
        return isalnum(c) || isspace(c) || c == '_';
    });
}

bool isValidDistrict(const string& district) {
    return !district.empty() && all_of(district.begin(), district.end(), [](char c) {
        return isalnum(c) || isspace(c);
    });
}

bool isValidLatLon(float value) {
    return value >= -90.0 && value <= 90.0;
}

bool isValidCapacity(int capacity) {
    return capacity >= 0;
}

bool isValidDistance(float distance) {
    return distance > 0;
}

bool isValidTime(int time) {
    return time > 0;
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isValidDate(int day, int month, int year) {
    if (year < 1900 || year > 2025) return false; // No future dates beyond 2025
    if (month < 1 || month > 12) return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isLeapYear(year)) daysInMonth[1] = 29;

    return day >= 1 && day <= daysInMonth[month - 1];
}

// Trim whitespace from string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n");
    size_t last = str.find_last_not_of(" \t\n");
    if (first == string::npos) return "";
    return str.substr(first, last - first + 1);
}

// Convert string to lowercase for case-insensitive comparison
string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Read health centers from CSV
void readHealthCenters() {
    ifstream file("health_centers.csv");
    if (!file.is_open()) {
        ofstream outfile("health_centers.csv");
        outfile << "ID,Name,District,Latitude,Longitude,Capacity,Day,Month,Year\n";
        outfile.close();
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        try {
            stringstream ss(line);
            string idStr, name, district, latStr, lonStr, capStr, dayStr, monthStr, yearStr;
            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, district, ',');
            getline(ss, latStr, ',');
            getline(ss, lonStr, ',');
            getline(ss, capStr, ',');
            getline(ss, dayStr, ',');
            getline(ss, monthStr, ',');
            getline(ss, yearStr);

            HealthCenter hc;
            hc.id = stoi(idStr);
            hc.name = trim(name);
            hc.district = trim(district);
            hc.lat = stof(latStr);
            hc.lon = stof(lonStr);
            hc.capacity = stoi(capStr);
            hc.established_date.day = stoi(dayStr);
            hc.established_date.month = stoi(monthStr);
            hc.established_date.year = stoi(yearStr);

            if (isValidID(hc.id) && isUniqueID(hc.id) && isValidLatLon(hc.lat) && 
                isValidLatLon(hc.lon) && isValidCapacity(hc.capacity) && 
                isValidDate(hc.established_date.day, hc.established_date.month, hc.established_date.year)) {
                centers.push_back(hc);
            }
        } catch (const exception& e) {
            cout << "Error parsing health center: " << e.what() << endl;
        }
    }
    file.close();
}

// Read connections from CSV
void readConnections() {
    ifstream file("connections.csv");
    if (!file.is_open()) {
        ofstream outfile("connections.csv");
        outfile << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
        outfile.close();
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        try {
            stringstream ss(line);
            string fromStr, toStr, distStr, timeStr, desc;
            getline(ss, fromStr, ',');
            getline(ss, toStr, ',');
            getline(ss, distStr, ',');
            getline(ss, timeStr, ',');
            getline(ss, desc);

            Connection conn;
            int fromID = stoi(fromStr);
            conn.to = stoi(toStr);
            conn.distance = stof(distStr);
            conn.time = stoi(timeStr);
            conn.description = trim(desc);

            if (isValidID(fromID) && isValidID(conn.to) && isValidDistance(conn.distance) && 
                isValidTime(conn.time)) {
                adjList[fromID].push_back(conn);
            }
        } catch (const exception& e) {
            cout << "Error parsing connection: " << e.what() << endl;
        }
    }
    file.close();
}

// Save health centers to CSV
void saveHealthCenters() {
    ofstream file("health_centers.csv");
    file << "ID,Name,District,Latitude,Longitude,Capacity,Day,Month,Year\n";
    for (const auto& center : centers) {
        file << center.id << "," << center.name << "," << center.district << ","
             << center.lat << "," << center.lon << "," << center.capacity << ","
             << center.established_date.day << "," << center.established_date.month << ","
             << center.established_date.year << "\n";
    }
    file.close();
}

// Save connections to CSV
void saveConnections() {
    ofstream file("connections.csv");
    file << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
    for (int i = 0; i < MAX; i++) {
        for (const auto& conn : adjList[i]) {
            file << i << "," << conn.to << "," << conn.distance << ","
                 << conn.time << "," << conn.description << "\n";
        }
    }
    file.close();
}

// CRUD Operations for Health Centers
void addHealthCenter() {
    HealthCenter hc;
    string input;

    cout << "Enter ID (0-999): ";
    while (getline(cin, input)) {
        try {
            hc.id = stoi(input);
            if (!isValidID(hc.id)) {
                cout << "Invalid ID. Must be between 0 and 999. Try again: ";
                continue;
            }
            if (!isUniqueID(hc.id)) {
                cout << "ID already exists. Try another: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Name (alphanumeric, spaces, or underscores): ";
    while (getline(cin, input)) {
        input = trim(input);
        if (!isValidName(input)) {
            cout << "Invalid name. Use alphanumeric, spaces, or underscores: ";
            continue;
        }
        hc.name = input;
        break;
    }

    cout << "Enter District (alphanumeric or spaces): ";
    while (getline(cin, input)) {
        input = trim(input);
        if (!isValidDistrict(input)) {
            cout << "Invalid district. Use alphanumeric or spaces: ";
            continue;
        }
        hc.district = input;
        break;
    }

    cout << "Enter Latitude (-90 to 90): ";
    while (getline(cin, input)) {
        try {
            hc.lat = stof(input);
            if (!isValidLatLon(hc.lat)) {
                cout << "Invalid latitude. Must be between -90 and 90: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Longitude (-90 to 90): ";
    while (getline(cin, input)) {
        try {
            hc.lon = stof(input);
            if (!isValidLatLon(hc.lon)) {
                cout << "Invalid longitude. Must be between -90 and 90: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Capacity (non-negative): ";
    while (getline(cin, input)) {
        try {
            hc.capacity = stoi(input);
            if (!isValidCapacity(hc.capacity)) {
                cout << "Invalid capacity. Must be non-negative: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Established Date (DD MM YYYY, e.g., 15 04 2020):\n";
    cout << "Day (1-31): ";
    while (getline(cin, input)) {
        try {
            hc.established_date.day = stoi(input);
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Month (1-12): ";
    while (getline(cin, input)) {
        try {
            hc.established_date.month = stoi(input);
            if (hc.established_date.month < 1 || hc.established_date.month > 12) {
                cout << "Invalid month. Must be between 1 and 12: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Year (1900-2025): ";
    while (getline(cin, input)) {
        try {
            hc.established_date.year = stoi(input);
            if (!isValidDate(hc.established_date.day, hc.established_date.month, hc.established_date.year)) {
                cout << "Invalid date. Check day, month, and year (1900-2025): ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    centers.push_back(hc);
    saveHealthCenters();
    cout << "Health Center added successfully.\n";
}

void editHealthCenter() {
    int id;
    string input;
    cout << "Enter Health Center ID to edit: ";
    while (getline(cin, input)) {
        try {
            id = stoi(input);
            if (!isValidID(id)) {
                cout << "Invalid ID. Must be between 0 and 999: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    auto it = find_if(centers.begin(), centers.end(), [id](const HealthCenter& hc) {
        return hc.id == id;
    });

    if (it == centers.end()) {
        cout << "Health Center not found.\n";
        return;
    }

    HealthCenter& hc = *it;
    cout << "Editing Health Center ID " << id << ". Leave blank to keep current value.\n";

    cout << "Current Name (" << hc.name << "): ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) {
        if (isValidName(input)) hc.name = input;
        else cout << "Invalid name. Keeping current value.\n";
    }

    cout << "Current District (" << hc.district << "): ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) {
        if (isValidDistrict(input)) hc.district = input;
        else cout << "Invalid district. Keeping current value.\n";
    }

    cout << "Current Latitude (" << hc.lat << "): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            float lat = stof(input);
            if (isValidLatLon(lat)) hc.lat = lat;
            else cout << "Invalid latitude. Keeping current value.\n";
        } catch (...) {
            cout << "Invalid input. Keeping current value.\n";
        }
    }

    cout << "Current Longitude (" << hc.lon << "): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            float lon = stof(input);
            if (isValidLatLon(lon)) hc.lon = lon;
            else cout << "Invalid longitude. Keeping current value.\n";
        } catch (...) {
            cout << "Invalid input. Keeping current value.\n";
        }
    }

    cout << "Current Capacity (" << hc.capacity << "): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            int cap = stoi(input);
            if (isValidCapacity(cap)) hc.capacity = cap;
            else cout << "Invalid capacity. Keeping current value.\n";
        } catch (...) {
            cout << "Invalid input. Keeping current value.\n";
        }
    }

    cout << "Current Established Date (" << hc.established_date.day << "/" 
         << hc.established_date.month << "/" << hc.established_date.year << "): \n";
    cout << "New Day (1-31, blank to keep): ";
    int newDay = hc.established_date.day;
    getline(cin, input);
    if (!input.empty()) {
        try {
            newDay = stoi(input);
        } catch (...) {
            cout << "Invalid input. Keeping current day.\n";
        }
    }

    cout << "New Month (1-12, blank to keep): ";
    int newMonth = hc.established_date.month;
    getline(cin, input);
    if (!input.empty()) {
        try {
            newMonth = stoi(input);
            if (newMonth < 1 || newMonth > 12) {
                cout << "Invalid month. Keeping current month.\n";
                newMonth = hc.established_date.month;
            }
        } catch (...) {
            cout << "Invalid input. Keeping current month.\n";
        }
    }

    cout << "New Year (1900-2025, blank to keep): ";
    int newYear = hc.established_date.year;
    getline(cin, input);
    if (!input.empty()) {
        try {
            newYear = stoi(input);
        } catch (...) {
            cout << "Invalid input. Keeping current year.\n";
        }
    }

    if (isValidDate(newDay, newMonth, newYear)) {
        hc.established_date.day = newDay;
        hc.established_date.month = newMonth;
        hc.established_date.year = newYear;
    } else {
        cout << "Invalid date. Keeping current date.\n";
    }

    saveHealthCenters();
    cout << "Health Center updated successfully.\n";
}

void viewHealthCenters() {
    if (centers.empty()) {
        cout << "No health centers available.\n";
        return;
    }

    cout << left << setw(5) << "ID" << setw(30) << "Name" << setw(20) << "District"
         << setw(15) << "Latitude" << setw(15) << "Longitude" << setw(10) << "Capacity"
         << setw(12) << "Est. Date" << "\n";
    cout << string(107, '-') << "\n";
    for (const auto& hc : centers) {
        cout << left << setw(5) << hc.id << setw(30) << hc.name << setw(20) << hc.district
             << setw(15) << fixed << setprecision(4) << hc.lat
             << setw(15) << hc.lon << setw(10) << hc.capacity
             << setw(12) << (to_string(hc.established_date.day) + "/" + 
                             to_string(hc.established_date.month) + "/" + 
                             to_string(hc.established_date.year)) << "\n";
    }
}

void removeHealthCenter() {
    int id;
    string input;
    cout << "Enter Health Center ID to remove: ";
    while (getline(cin, input)) {
        try {
            id = stoi(input);
            if (!isValidID(id)) {
                cout << "Invalid ID. Must be between 0 and 999: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    auto it = find_if(centers.begin(), centers.end(), [id](const HealthCenter& hc) {
        return hc.id == id;
    });

    if (it == centers.end()) {
        cout << "Health Center not found.\n";
        return;
    }

    centers.erase(it);
    adjList[id].clear();
    for (int i = 0; i < MAX; i++) {
        adjList[i].erase(remove_if(adjList[i].begin(), adjList[i].end(),
            [id](const Connection& conn) { return conn.to == id; }), adjList[i].end());
    }

    saveHealthCenters();
    saveConnections();
    cout << "Health Center removed successfully.\n";
}

// Search Health Center
void searchHealthCenter() {
    string criterion, input;
    cout << "Search by (1) Name or (2) District: ";
    getline(cin, input);
    try {
        int choice = stoi(input);
        if (choice != 1 && choice != 2) {
            cout << "Invalid choice. Please enter 1 or 2.\n";
            return;
        }
        criterion = (choice == 1) ? "Name" : "District";
    } catch (...) {
        cout << "Invalid input. Please enter a number (1 or 2).\n";
        return;
    }

    cout << "Enter " << criterion << " to search: ";
    getline(cin, input);
    input = trim(input);
    if (input.empty()) {
        cout << "Search term cannot be empty.\n";
        return;
    }

    vector<HealthCenter> results;
    if (criterion == "Name") {
        string searchTerm = toLowerCase(input);
        for (const auto& hc : centers) {
            if (toLowerCase(hc.name).find(searchTerm) != string::npos) {
                results.push_back(hc);
            }
        }
    } else {
        for (const auto& hc : centers) {
            if (hc.district == input) {
                results.push_back(hc);
            }
        }
    }

    if (results.empty()) {
        cout << "No health centers found matching the criterion.\n";
        return;
    }

    cout << left << setw(5) << "ID" << setw(30) << "Name" << setw(20) << "District"
         << setw(15) << "Latitude" << setw(15) << "Longitude" << setw(10) << "Capacity"
         << setw(12) << "Est. Date" << "\n";
    cout << string(107, '-') << "\n";
    for (const auto& hc : results) {
        cout << left << setw(5) << hc.id << setw(30) << hc.name << setw(20) << hc.district
             << setw(15) << fixed << setprecision(4) << hc.lat
             << setw(15) << hc.lon << setw(10) << hc.capacity
             << setw(12) << (to_string(hc.established_date.day) + "/" + 
                             to_string(hc.established_date.month) + "/" + 
                             to_string(hc.established_date.year)) << "\n";
    }
}

// Search Connection
void searchConnection() {
    string criterion, input;
    cout << "Search by (1) From ID, (2) To ID, or (3) Description: ";
    getline(cin, input);
    int choice;
    try {
        choice = stoi(input);
        if (choice < 1 || choice > 3) {
            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
            return;
        }
    } catch (...) {
        cout << "Invalid input. Please enter a number (1, 2, or 3).\n";
        return;
    }

    vector<pair<int, Connection>> results;
    if (choice == 1 || choice == 2) {
        cout << "Enter ID: ";
        getline(cin, input);
        int id;
        try {
            id = stoi(input);
            if (!isValidID(id)) {
                cout << "Invalid ID. Must be between 0 and 999.\n";
                return;
            }
        } catch (...) {
            cout << "Invalid input. Enter a number.\n";
            return;
        }

        for (int i = 0; i < MAX; i++) {
            for (const auto& conn : adjList[i]) {
                if ((choice == 1 && i == id) || (choice == 2 && conn.to == id)) {
                    results.emplace_back(i, conn);
                }
            }
        }
    } else {
        cout << "Enter Description to search: ";
        getline(cin, input);
        input = trim(input);
        if (input.empty()) {
            cout << "Search term cannot be empty.\n";
            return;
        }
        string searchTerm = toLowerCase(input);
        for (int i = 0; i < MAX; i++) {
            for (const auto& conn : adjList[i]) {
                if (toLowerCase(conn.description).find(searchTerm) != string::npos) {
                    results.emplace_back(i, conn);
                }
            }
        }
    }

    if (results.empty()) {
        cout << "No connections found matching the criterion.\n";
        return;
    }

    cout << left << setw(10) << "From ID" << setw(10) << "To ID" << setw(15) << "Distance (km)"
         << setw(15) << "Time (min)" << "Description" << "\n";
    cout << string(65, '-') << "\n";
    for (const auto& result : results) {
        const auto& conn = result.second;
        cout << left << setw(10) << result.first << setw(10) << conn.to << setw(15) << fixed << setprecision(2)
             << conn.distance << setw(15) << conn.time << conn.description << "\n";
    }
}

// CRUD Operations for Connections
void addConnection() {
    Connection conn;
    int fromID;
    string input;

    cout << "Enter From ID: ";
    while (getline(cin, input)) {
        try {
            fromID = stoi(input);
            if (!isValidID(fromID) || find_if(centers.begin(), centers.end(),
                [fromID](const HealthCenter& hc) { return hc.id == fromID; }) == centers.end()) {
                cout << "Invalid or non-existent From ID. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter To ID: ";
    while (getline(cin, input)) {
        try {
            conn.to = stoi(input);
            if (!isValidID(conn.to) || find_if(centers.begin(), centers.end(),
                [conn](const HealthCenter& hc) { return hc.id == conn.to; }) == centers.end()) {
                cout << "Invalid or non-existent To ID. Try again: ";
                continue;
            }
            if (fromID == conn.to) {
                cout << "From and To IDs cannot be the same. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Distance (km, positive): ";
    while (getline(cin, input)) {
        try {
            conn.distance = stof(input);
            if (!isValidDistance(conn.distance)) {
                cout << "Invalid distance. Must be positive: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Time (minutes, positive): ";
    while (getline(cin, input)) {
        try {
            conn.time = stoi(input);
            if (!isValidTime(conn.time)) {
                cout << "Invalid time. Must be positive: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Description: ";
    getline(cin, conn.description);
    conn.description = trim(conn.description);
    if (conn.description.empty()) conn.description = "No description";

    adjList[fromID].push_back(conn);
    saveConnections();
    cout << "Connection added successfully.\n";
}

void editConnection() {
    int fromID, toID;
    string input;

    cout << "Enter From ID: ";
    while (getline(cin, input)) {
        try {
            fromID = stoi(input);
            if (!isValidID(fromID)) {
                cout << "Invalid From ID. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter To ID: ";
    while (getline(cin, input)) {
        try {
            toID = stoi(input);
            if (!isValidID(toID)) {
                cout << "Invalid To ID. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    auto it = find_if(adjList[fromID].begin(), adjList[fromID].end(),
        [toID](const Connection& conn) { return conn.to == toID; });

    if (it == adjList[fromID].end()) {
        cout << "Connection not found.\n";
        return;
    }

    Connection& conn = *it;
    cout << "Editing connection from ID " << fromID << " to ID " << toID << ".\n";

    cout << "Current Distance (" << conn.distance << " km): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            float dist = stof(input);
            if (isValidDistance(dist)) conn.distance = dist;
            else cout << "Invalid distance. Keeping current value.\n";
        } catch (...) {
            cout << "Invalid input. Keeping current value.\n";
        }
    }

    cout << "Current Time (" << conn.time << " minutes): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            int time = stoi(input);
            if (isValidTime(time)) conn.time = time;
            else cout << "Invalid time. Keeping current value.\n";
        } catch (...) {
            cout << "Invalid input. Keeping current value.\n";
        }
    }

    cout << "Current Description (" << conn.description << "): ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) conn.description = input;

    saveConnections();
    cout << "Connection updated successfully.\n";
}

void viewConnections() {
    bool hasConnections = false;
    for (int i = 0; i < MAX; i++) {
        if (!adjList[i].empty()) {
            hasConnections = true;
            break;
        }
    }

    if (!hasConnections) {
        cout << "No connections available.\n";
        return;
    }

    cout << left << setw(10) << "From ID" << setw(10) << "To ID" << setw(15) << "Distance (km)"
         << setw(15) << "Time (min)" << "Description" << "\n";
    cout << string(65, '-') << "\n";
    for (int i = 0; i < MAX; i++) {
        for (const auto& conn : adjList[i]) {
            cout << left << setw(10) << i << setw(10) << conn.to << setw(15) << fixed << setprecision(2)
                 << conn.distance << setw(15) << conn.time << conn.description << "\n";
        }
    }
}

void removeConnection() {
    int fromID, toID;
    string input;

    cout << "Enter From ID: ";
    while (getline(cin, input)) {
        try {
            fromID = stoi(input);
            if (!isValidID(fromID)) {
                cout << "Invalid From ID. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter To ID: ";
    while (getline(cin, input)) {
        try {
            toID = stoi(input);
            if (!isValidID(toID)) {
                cout << "Invalid To ID. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    auto it = find_if(adjList[fromID].begin(), adjList[fromID].end(),
        [toID](const Connection& conn) { return conn.to == toID; });

    if (it == adjList[fromID].end()) {
        cout << "Connection not found.\n";
        return;
    }

    adjList[fromID].erase(it);
    saveConnections();
    cout << "Connection removed successfully.\n";
}

// Graph Algorithms
void dijkstra(int start, int end) {
    vector<float> dist(MAX, numeric_limits<float>::infinity());
    vector<int> prev(MAX, -1);
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        float d = pq.top().first;
        pq.pop();

        if (d > dist[u]) continue;

        for (const auto& conn : adjList[u]) {
            int v = conn.to;
            float weight = conn.distance;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[end] == numeric_limits<float>::infinity()) {
        cout << "No path exists from ID " << start << " to ID " << end << ".\n";
        return;
    }

    cout << "Shortest path from ID " << start << " to ID " << end << ": ";
    vector<int> path;
    for (int v = end; v != -1; v = prev[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i < path.size() - 1) cout << " -> ";
    }
    cout << "\nTotal Distance: " << fixed << setprecision(2) << dist[end] << " km\n";
}

void bfs(int start) {
    vector<bool> visited(MAX, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;

    cout << "BFS Traversal starting from ID " << start << ": ";
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        cout << u << " ";

        for (const auto& conn : adjList[u]) {
            if (!visited[conn.to]) {
                visited[conn.to] = true;
                q.push(conn.to);
            }
        }
    }
    cout << "\n";
}

bool detectCycleUtil(int v, vector<bool>& visited, vector<bool>& recStack) {
    visited[v] = true;
    recStack[v] = true;

    for (const auto& conn : adjList[v]) {
        int u = conn.to;
        if (!visited[u] && detectCycleUtil(u, visited, recStack)) {
            return true;
        } else if (recStack[u]) {
            return true;
        }
    }

    recStack[v] = false;
    return false;
}

void detectCycle() {
    vector<bool> visited(MAX, false);
    vector<bool> recStack(MAX, false);
    bool hasCycle = false;

    for (int i = 0; i < MAX; i++) {
        if (!visited[i] && detectCycleUtil(i, visited, recStack)) {
            hasCycle = true;
            break;
        }
    }

    cout << (hasCycle ? "Cycle detected in the network.\n" : "No cycle detected in the network.\n");
}

void floydWarshall() {
    vector<vector<float>> dist(MAX, vector<float>(MAX, numeric_limits<float>::infinity()));
    for (int i = 0; i < MAX; i++) {
        dist[i][i] = 0;
        for (const auto& conn : adjList[i]) {
            dist[i][conn.to] = conn.distance;
        }
    }

    for (int k = 0; k < MAX; k++) {
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                if (dist[i][k] != numeric_limits<float>::infinity() &&
                    dist[k][j] != numeric_limits<float>::infinity() &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    cout << "All-Pairs Shortest Paths (distances in km):\n";
    for (const auto& hc1 : centers) {
        for (const auto& hc2 : centers) {
            if (hc1.id != hc2.id && dist[hc1.id][hc2.id] != numeric_limits<float>::infinity()) {
                cout << "From ID " << hc1.id << " to ID " << hc2.id << ": "
                     << fixed << setprecision(2) << dist[hc1.id][hc2.id] << " km\n";
            }
        }
    }
}

void primMST() {
    vector<float> key(MAX, numeric_limits<float>::infinity());
    vector<int> parent(MAX, -1);
    vector<bool> inMST(MAX, false);
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;

    int start = centers.empty() ? 0 : centers[0].id;
    key[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true;

        for (const auto& conn : adjList[u]) {
            int v = conn.to;
            float weight = conn.distance;
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }

    cout << "Minimum Spanning Tree Edges:\n";
    float totalWeight = 0;
    for (int i = 0; i < MAX; i++) {
        if (parent[i] != -1) {
            cout << "Edge: " << parent[i] << " - " << i << " Distance: " << fixed << setprecision(2)
                 << key[i] << " km\n";
            totalWeight += key[i];
        }
    }
    cout << "Total MST Weight: " << fixed << setprecision(2) << totalWeight << " km\n";
}

void emergencyRouting() {
    int startID, minCapacity;
    string input;

    cout << "Enter Starting Health Center ID: ";
    while (getline(cin, input)) {
        try {
            startID = stoi(input);
            if (!isValidID(startID) || find_if(centers.begin(), centers.end(),
                [startID](const HealthCenter& hc) { return hc.id == startID; }) == centers.end()) {
                cout << "Invalid or non-existent ID. Try again: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    cout << "Enter Minimum Capacity Required: ";
    while (getline(cin, input)) {
        try {
            minCapacity = stoi(input);
            if (!isValidCapacity(minCapacity)) {
                cout << "Invalid capacity. Must be non-negative: ";
                continue;
            }
            break;
        } catch (...) {
            cout << "Invalid input. Enter a number: ";
        }
    }

    vector<float> dist(MAX, numeric_limits<float>::infinity());
    vector<int> prev(MAX, -1);
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;
    dist[startID] = 0;
    pq.push({0, startID});

    int nearestID = -1;
    float minDist = numeric_limits<float>::infinity();

    while (!pq.empty()) {
        int u = pq.top().second;
        float d = pq.top().first;
        pq.pop();

        if (d > dist[u]) continue;

        auto it = find_if(centers.begin(), centers.end(),
            [u, minCapacity](const HealthCenter& hc) { return hc.id == u && hc.capacity >= minCapacity; });
        if (it != centers.end() && u != startID && d < minDist) {
            nearestID = u;
            minDist = d;
        }

        for (const auto& conn : adjList[u]) {
            int v = conn.to;
            float weight = conn.distance;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (nearestID == -1) {
        cout << "No health center found with capacity >= " << minCapacity << ".\n";
        return;
    }

    cout << "Nearest Health Center with capacity >= " << minCapacity << " is ID " << nearestID << "\n";
    cout << "Path: ";
    vector<int> path;
    for (int v = nearestID; v != -1; v = prev[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i < path.size() - 1) cout << " -> ";
    }
    cout << "\nTotal Distance: " << fixed << setprecision(2) << minDist << " km\n";
}

// Additional Features
void viewRelationships() {
    ofstream file("relationship_table.csv");
    file << "HealthCenterID,ConnectedToID,DistanceKM,TimeMinutes,Description\n";

    cout << left << setw(15) << "Health Center" << setw(15) << "Connected To"
         << setw(15) << "Distance (km)" << setw(15) << "Time (min)" << "Description" << "\n";
    cout << string(75, '-') << "\n";

    for (const auto& hc : centers) {
        for (const auto& conn : adjList[hc.id]) {
            cout << left << setw(15) << hc.id << setw(15) << conn.to << setw(15) << fixed << setprecision(2)
                 << conn.distance << setw(15) << conn.time << conn.description << "\n";
            file << hc.id << "," << conn.to << "," << conn.distance << "," << conn.time << ","
                 << conn.description << "\n";
        }
    }

    file.close();
    cout << "Relationships saved to relationship_table.csv\n";
}

// Menu System
void displayWelcomeBanner() {
    cout << "\n";
    cout << "_____________________________________________________\n";
    cout << "|                                                   |\n";
    cout << "|   Welcome to the Health Center Network System!     |\n";
    cout << "|                                                   |\n";
    cout << "|___________________________________________________|\n";
    cout << "\n";
}

void displayMenu() {
    cout << "\n==== Health Center Network System ====\n";
    cout << "1. Add Health Center\n";
    cout << "2. Edit Health Center\n";
    cout << "3. View Health Centers\n";
    cout << "4. Remove Health Center\n";
    cout << "5. Add Connection\n";
    cout << "6. Edit Connection\n";
    cout << "7. View Connections\n";
    cout << "8. Remove Connection\n";
    cout << "9. View Relationships\n";
    cout << "10. Dijkstra's Shortest Path\n";
    cout << "11. BFS Traversal\n";
    cout << "12. Detect Cycle\n";
    cout << "13. Floyd-Warshall All-Pairs\n";
    cout << "14. Prim's MST\n";
    cout << "15. Emergency Routing\n";
    cout << "16. Search Health Center\n";
    cout << "17. Search Connection\n";
    cout << "0. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    displayWelcomeBanner();
    readHealthCenters();
    readConnections();

    string input;
    int choice;
    while (true) {
        displayMenu();
        getline(cin, input);
        try {
            choice = stoi(input);
        } catch (...) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 0) break;

        switch (choice) {
            case 1: addHealthCenter(); break;
            case 2: editHealthCenter(); break;
            case 3: viewHealthCenters(); break;
            case 4: removeHealthCenter(); break;
            case 5: addConnection(); break;
            case 6: editConnection(); break;
            case 7: viewConnections(); break;
            case 8: removeConnection(); break;
            case 9: viewRelationships(); break;
            case 10: {
                int start, end;
                cout << "Enter Start ID: ";
                getline(cin, input);
                try {
                    start = stoi(input);
                    if (!isValidID(start)) throw out_of_range("Invalid ID");
                } catch (...) {
                    cout << "Invalid input. Operation cancelled.\n";
                    break;
                }
                cout << "Enter End ID: ";
                getline(cin, input);
                try {
                    end = stoi(input);
                    if (!isValidID(end)) throw out_of_range("Invalid ID");
                } catch (...) {
                    cout << "Invalid input. Operation cancelled.\n";
                    break;
                }
                dijkstra(start, end);
                break;
            }
            case 11: {
                int start;
                cout << "Enter Start ID: ";
                getline(cin, input);
                try {
                    start = stoi(input);
                    if (!isValidID(start)) throw out_of_range("Invalid ID");
                } catch (...) {
                    cout << "Invalid input. Operation cancelled.\n";
                    break;
                }
                bfs(start);
                break;
            }
            case 12: detectCycle(); break;
            case 13: floydWarshall(); break;
            case 14: primMST(); break;
            case 15: emergencyRouting(); break;
            case 16: searchHealthCenter(); break;
            case 17: searchConnection(); break;
            default: cout << "Invalid choice. Try again.\n";
        }
    }

    cout << "Exiting program.\n";
    return 0;
}