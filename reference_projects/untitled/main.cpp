#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <queue>
#include <set>

using namespace std;

struct Hospital {
    string name;
    string type;
    string location;
    string director;
    int doctors, nurses, cleaners, management, patients;
};

class HospitalGraph {
private:
    unordered_map<string, Hospital> hospitals;
    unordered_map<string, unordered_map<string, int>> edges;
    const string defaultFile = "database.txt";
    const string connectionsFile = "connections.csv";

    bool isValidString(const string& input, bool allowNumbers = false) {
        if (input.empty()) {
            return false;
        }

        bool hasLetter = false;
        for (char c : input) {
            if (isalpha(c)) {
                hasLetter = true;
                break;
            }
        }
        if (!hasLetter) {
            return false;
        }

        for (char c : input) {
            if (!isalpha(c) && c != ' ' && c != '-' && c != '.' && c != '\'' && c != ',') {
                if (allowNumbers && isdigit(c)) {
                    continue;
                }
                return false;
            }
        }
        return true;
    }

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }


    pair<vector<string>, int> findShortestPath(const string& start, const string& end) {
        unordered_map<string, int> distances;
        unordered_map<string, string> previous;
        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
        set<string> visited;

        // Initialize distances
        for (const auto& [name, _] : hospitals) {
            distances[name] = numeric_limits<int>::max();
        }
        distances[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            string current = pq.top().second;
            pq.pop();

            if (current == end) {
                break;
            }

            if (visited.count(current)) {
                continue;
            }
            visited.insert(current);

            if (!edges.count(current)) {
                continue;
            }

            for (const auto& [neighbor, distance] : edges[current]) {
                if (!hospitals.count(neighbor)) {
                    continue;
                }

                int newDistance = distances[current] + distance;
                if (newDistance < distances[neighbor]) {
                    distances[neighbor] = newDistance;
                    previous[neighbor] = current;
                    pq.push({newDistance, neighbor});
                }
            }
        }

        // Reconstruct path
        vector<string> path;
        if (distances[end] == numeric_limits<int>::max()) {
            return {path, -1}; // No path found
        }

        string current = end;
        while (current != start) {
            path.push_back(current);
            current = previous[current];
        }
        path.push_back(start);
        reverse(path.begin(), path.end());

        return {path, distances[end]};
    }

public:
    bool getValidInteger(int& value, const string& prompt) {
        cout << prompt;
        string input;
        getline(cin, input);
        try {
            size_t pos;
            value = stoi(input, &pos);
            if (pos != input.length()) {
                cout << "Error: Invalid input. Please enter a number.\n";
                return false;
            }
            if (value < 0) {
                cout << "Error: Number cannot be negative.\n";
                return false;
            }
            return true;
        } catch (...) {
            cout << "Error: Please enter a valid number.\n";
            return false;
        }
    }

    bool loadFromFile() {
        ifstream file(defaultFile);
        if (!file.is_open()) {
            cout << "Error: Could not open file " << defaultFile << " for reading.\n";
            return false;
        }
        string line;
        if (!getline(file, line)) {
            cout << "Error: File is empty or corrupted.\n";
            file.close();
            return false;
        }
        while (getline(file, line)) {
            stringstream ss(line);
            Hospital h;
            string val;
            try {
                getline(ss, h.name, ',');
                if (!isValidString(h.name)) throw runtime_error("Invalid hospital name.");
                getline(ss, h.type, ',');
                getline(ss, h.location, ',');
                getline(ss, h.director, ',');
                getline(ss, val, ',');
                h.doctors = stoi(val);
                getline(ss, val, ',');
                h.nurses = stoi(val);
                getline(ss, val, ',');
                h.cleaners = stoi(val);
                getline(ss, val, ',');
                h.management = stoi(val);
                getline(ss, val);
                h.patients = stoi(val);
                if (h.doctors < 0 || h.nurses < 0 || h.cleaners < 0 || h.management < 0 || h.patients < 0) {
                    throw runtime_error("Negative values not allowed.");
                }
                hospitals[h.name] = h;
            } catch (const exception& e) {
                cout << "Error parsing line: " << line << " (" << e.what() << ")\n";
                continue;
            }
        }
        file.close();

        // Load connections from connections file
        ifstream connFile(connectionsFile);
        if (connFile.is_open()) {
            while (getline(connFile, line)) {
                stringstream ss(line);
                string h1, h2, distStr;
                try {
                    getline(ss, h1, ',');
                    getline(ss, h2, ',');
                    getline(ss, distStr);
                    int dist = stoi(distStr);
                    if (hospitals.count(h1) && hospitals.count(h2)) {
                        edges[h1][h2] = dist;
                        edges[h2][h1] = dist;
                    }
                } catch (...) {
                    cout << "Error parsing connection: " << line << "\n";
                    continue;
                }
            }
            connFile.close();
        }
        return true;
    }

    bool saveToDefaultFile() {
        ofstream file(defaultFile);
        if (!file.is_open()) {
            cout << "Error: Could not open file " << defaultFile << " for writing.\n";
            return false;
        }
        file << "Name,Type,Location,Director,Doctors,Nurses,Cleaners,Management,Patients\n";
        for (const auto& [_, h] : hospitals) {
            file << h.name << "," << h.type << "," << h.location << "," << h.director << ","
                 << h.doctors << "," << h.nurses << "," << h.cleaners << ","
                 << h.management << "," << h.patients << "\n";
        }
        file.close();

        // Save connections to connections file
        ofstream connFile(connectionsFile);
        if (!connFile.is_open()) {
            cout << "Error: Could not open connections file " << connectionsFile << " for writing.\n";
            return false;
        }
        for (const auto& [h1, connections] : edges) {
            for (const auto& [h2, dist] : connections) {
                // To avoid duplicate entries (A,B and B,A), we only save when h1 < h2
                if (h1 < h2) {
                    connFile << h1 << "," << h2 << "," << dist << "\n";
                }
            }
        }
        connFile.close();
        return true;
    }

    void addHospital() {
        Hospital h;
        clearInputBuffer();

        while (true) {
            cout << "Name: ";
            getline(cin, h.name);
            if (!isValidString(h.name)) {
                cout << "Error: Hospital name must contain letters and can include spaces, hyphens, periods, and apostrophes.\n";
                continue;
            }
            if (hospitals.count(h.name)) {
                cout << "Error: Hospital with name '" << h.name << "' already exists.\n";
                continue;
            }
            break;
        }

        while (true) {
            cout << "Type (e.g., General, Children's, Specialty): ";
            getline(cin, h.type);
            if (!isValidString(h.type)) {
                cout << "Error: Type must contain letters and can include spaces, hyphens, periods, and apostrophes.\n";
                continue;
            }
            break;
        }

        while (true) {
            cout << "Location (e.g., City, Street Address): ";
            getline(cin, h.location);
            if (!isValidString(h.location, true)) { // Allow numbers in addresses
                cout << "Error: Location must contain letters and can include spaces, numbers, hyphens, periods, and commas.\n";
                continue;
            }
            break;
        }

        while (true) {
            cout << "Director (Full Name): ";
            getline(cin, h.director);
            if (!isValidString(h.director)) {
                cout << "Error: Director name must contain letters and can include spaces, hyphens, periods, and apostrophes.\n";
                continue;
            }
            break;
        }

        if (!getValidInteger(h.doctors, "Doctors: ")) return;
        if (!getValidInteger(h.nurses, "Nurses: ")) return;
        if (!getValidInteger(h.cleaners, "Cleaners: ")) return;
        if (!getValidInteger(h.management, "Management: ")) return;
        if (!getValidInteger(h.patients, "Patients: ")) return;

        hospitals[h.name] = h;
        if (saveToDefaultFile()) {
            cout << "Hospital added successfully.\n";
        }
    }
    void connectHospitals() {
        string h1, h2;
        int dist;
        clearInputBuffer();
        cout << "Base hospital name: "; getline(cin, h1);
        if (!hospitals.count(h1)) {
            cout << "Error: Hospital '" << h1 << "' not found.\n";
            return;
        }
        cout << "Hospital to connect to: "; getline(cin, h2);
        if (!hospitals.count(h2)) {
            cout << "Error: Hospital '" << h2 << "' not found.\n";
            return;
        }
        if (h1 == h2) {
            cout << "Error: Cannot connect a hospital to itself.\n";
            return;
        }
        if (!getValidInteger(dist, "Distance (km): ")) return;
        edges[h1][h2] = dist;
        edges[h2][h1] = dist;
        if (saveToDefaultFile()) {
            cout << "Hospitals connected successfully.\n";
        }
    }

    void viewAll() {
        if (hospitals.empty()) {
            cout << "No hospitals in the system.\n";
            return;
        }
        for (const auto& [_, h] : hospitals)
            displayHospitalWithConnections(h.name);
    }

    void viewHospital() {
        clearInputBuffer();
        string name;
        cout << "Enter hospital name: "; getline(cin, name);
        if (hospitals.count(name)) {
            displayHospitalWithConnections(name);
        } else {
            cout << "Error: Hospital '" << name << "' not found.\n";
        }
    }

    void viewConnections() {
        clearInputBuffer();
        string name;
        cout << "Enter hospital name: "; getline(cin, name);
        if (!hospitals.count(name)) {
            cout << "Error: Hospital '" << name << "' not found.\n";
            return;
        }
        if (!edges.count(name)) {
            cout << "No connections found for hospital: " << name << "\n";
            return;
        }
        cout << "\nConnections from " << name << ":\n";
        for (const auto& [other, dist] : edges[name]) {
            cout << " -> " << other << " : " << dist << " km\n";
        }
    }

    void viewShortestPath() {
        clearInputBuffer();
        string start, end;
        cout << "Enter starting hospital name: "; getline(cin, start);
        if (!hospitals.count(start)) {
            cout << "Error: Hospital '" << start << "' not found.\n";
            return;
        }
        cout << "Enter destination hospital name: "; getline(cin, end);
        if (!hospitals.count(end)) {
            cout << "Error: Hospital '" << end << "' not found.\n";
            return;
        }

        auto [path, distance] = findShortestPath(start, end);
        if (path.empty()) {
            cout << "No path exists between " << start << " and " << end << ".\n";
            return;
        }

        cout << "\nShortest path from " << start << " to " << end << ":\n";
        cout << "Total distance: " << distance << " km\n";
        cout << "Path: ";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i != 0) cout << " -> ";
            cout << path[i];
        }
        cout << "\n";
    }

    void deleteHospital() {
        clearInputBuffer();
        string name;
        cout << "Enter hospital name: "; getline(cin, name);
        if (!hospitals.count(name)) {
            cout << "Error: Hospital '" << name << "' not found.\n";
            return;
        }
        hospitals.erase(name);
        edges.erase(name);
        for (auto& [_, e] : edges) e.erase(name);
        if (saveToDefaultFile()) {
            cout << "Hospital deleted successfully.\n";
        }
    }

    void deleteAll() {
        if (hospitals.empty()) {
            cout << "No hospitals to delete.\n";
            return;
        }
        hospitals.clear();
        edges.clear();
        if (saveToDefaultFile()) {
            cout << "All hospitals deleted successfully.\n";
        }
    }

    void updateHospital() {
        clearInputBuffer();
        string name;
        cout << "Enter hospital name: "; getline(cin, name);
        if (!hospitals.count(name)) {
            cout << "Error: Hospital '" << name << "' not found.\n";
            return;
        }

        Hospital newData;
        cout << "Enter new data:\n";
        cout << "Name: "; getline(cin, newData.name);
        if (!isValidString(newData.name)) {
            cout << "Error: Hospital name cannot be empty.\n";
            return;
        }
        if (hospitals.count(newData.name) && newData.name != name) {
            cout << "Error: Hospital with name '" << newData.name << "' already exists.\n";
            return;
        }

        cout << "Type: "; getline(cin, newData.type);
        if (!isValidString(newData.type)) {
            cout << "Error: Type cannot be empty.\n";
            return;
        }
        cout << "Location: "; getline(cin, newData.location);
        if (!isValidString(newData.location)) {
            cout << "Error: Location cannot be empty.\n";
            return;
        }
        cout << "Director: "; getline(cin, newData.director);
        if (!isValidString(newData.director)) {
            cout << "Error: Director cannot be empty.\n";
            return;
        }
        if (!getValidInteger(newData.doctors, "Doctors: ")) return;
        if (!getValidInteger(newData.nurses, "Nurses: ")) return;
        if (!getValidInteger(newData.cleaners, "Cleaners: ")) return;
        if (!getValidInteger(newData.management, "Management: ")) return;
        if (!getValidInteger(newData.patients, "Patients: ")) return;

        if (name != newData.name) {
            if (edges.count(name)) {
                edges[newData.name] = edges[name];
                edges.erase(name);
                for (auto& [_, e] : edges) {
                    if (e.count(name)) {
                        e[newData.name] = e[name];
                        e.erase(name);
                    }
                }
            }
        }

        hospitals.erase(name);
        hospitals[newData.name] = newData;

        if (saveToDefaultFile()) {
            cout << "Hospital updated successfully.\n";
        }
    }

    void searchHospital() {
        clearInputBuffer();
        string key;
        cout << "Enter search key: "; getline(cin, key);
        if (key.empty()) {
            cout << "Error: Search key cannot be empty.\n";
            return;
        }
        bool found = false;
        for (const auto& [_, h] : hospitals) {
            if (h.name.find(key) != string::npos || h.location.find(key) != string::npos || h.director.find(key) != string::npos) {
                displayHospitalWithConnections(h.name);
                found = true;
            }
        }
        if (!found) {
            cout << "No hospitals found matching '" << key << "'.\n";
        }
    }

    void displayHospital(const Hospital& h) {
        cout << "\nName: " << h.name
             << "\nType: " << h.type
             << "\nLocation: " << h.location
             << "\nDirector: " << h.director
             << "\nDoctors: " << h.doctors
             << "\nNurses: " << h.nurses
             << "\nCleaners: " << h.cleaners
             << "\nManagement: " << h.management
             << "\nPatients: " << h.patients << "\n";
    }

    void displayHospitalWithConnections(const string& name) {
        if (!hospitals.count(name)) return;
        displayHospital(hospitals[name]);
        if (edges.count(name)) {
            cout << "Connections:\n";
            for (const auto& [other, dist] : edges[name]) {
                cout << " - " << other << " (" << dist << " km)\n";
            }
        } else {
            cout << "No connected hospitals.\n";
        }
    }

    bool saveToFile() {
        clearInputBuffer();
        string filename;
        cout << "Enter filename (e.g., data.csv): "; getline(cin, filename);
        if (filename.empty()) {
            cout << "Error: Filename cannot be empty.\n";
            return false;
        }

        // Save hospital data
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file " << filename << " for writing.\n";
            return false;
        }
        file << "Name,Type,Location,Director,Doctors,Nurses,Cleaners,Management,Patients\n";
        for (const auto& [_, h] : hospitals) {
            file << h.name << "," << h.type << "," << h.location << "," << h.director << ","
                 << h.doctors << "," << h.nurses << "," << h.cleaners << ","
                 << h.management << "," << h.patients << "\n";
        }
        file.close();

        // Save connections to a separate file
        size_t lastdot = filename.find_last_of(".");
        string connFilename = filename.substr(0, lastdot) + "_connections" + filename.substr(lastdot);

        ofstream connFile(connFilename);
        if (!connFile.is_open()) {
            cout << "Error: Could not open connections file " << connFilename << " for writing.\n";
            return false;
        }
        connFile << "BaseHospital, ConnectedHospital, Distance\n";
        for (const auto& [h1, connections] : edges) {
            for (const auto& [h2, dist] : connections) {
                if (h1 < h2) {
                    connFile << h1 << "," << h2 << "," << dist << "\n";
                }
            }
        }
        connFile.close();

        cout << "Data saved to " << filename << " and connections to " << connFilename << " successfully.\n";
        return true;
    }

    void help() {
        cout << "\nAvailable commands:\n"
             << "1. Add a hospital\n"
             << "2. Connect two hospitals\n"
             << "3. View all hospitals\n"
             << "4. View a hospital\n"
             << "5. View connections\n"
             << "6. View shortest path between hospitals\n"
             << "7. Delete a hospital\n"
             << "8. Delete all hospitals\n"
             << "9. Update a hospital\n"
             << "10. Search hospitals\n"
             << "11. Save to file\n"
             << "12. Show help\n"
             << "13. Exit\n"
             << "Enter a number (1-13): ";
    }
};

int main() {
    HospitalGraph hg;
    if (!hg.loadFromFile()) {
        cout << "Starting with an empty database.\n";
    }

    cout << "=============================================================\n";
    cout << "         WELCOME TO THE HOSPITAL MANAGEMENT SYSTEM          \n";
    cout << "=============================================================\n";
    hg.help();

    while (true) {
        int choice;
        if (!hg.getValidInteger(choice, "")) {
            continue;
        }
        switch (choice) {
            case 1: hg.addHospital(); break;
            case 2: hg.connectHospitals(); break;
            case 3: hg.viewAll(); break;
            case 4: hg.viewHospital(); break;
            case 5: hg.viewConnections(); break;
            case 6: hg.viewShortestPath(); break;
            case 7: hg.deleteHospital(); break;
            case 8: hg.deleteAll(); break;
            case 9: hg.updateHospital(); break;
            case 10: hg.searchHospital(); break;
            case 11: hg.saveToFile(); break;
            case 12: hg.help(); break;
            case 13: cout << "We will be happy to see you back. Byeee\n"; return 0;
            default: cout << "Error: Invalid choice. Enter a number between 1 and 13.\n"; break;
        }
    }

    return 0;
}
}