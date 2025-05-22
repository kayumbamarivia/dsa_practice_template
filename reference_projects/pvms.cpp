#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <climits>
#include <ctime>

using namespace std;

// ======== Utility: Safe Integer Input ========
int readInt(const string &prompt, int minVal = INT_MIN, int maxVal = INT_MAX)
{
    while (true)
    {
        cout << prompt;
        string line;
        if (!getline(cin, line))
        {
            cin.clear();
            continue;
        }
        try
        {
            int val = stoi(line);
            if (val < minVal || val > maxVal)
            {
                cout << "Please enter a number";
                if (minVal != INT_MIN && maxVal != INT_MAX)
                    cout << " between " << minVal << " and " << maxVal;
                cout << ".\n";
                continue;
            }
            return val;
        }
        catch (...)
        {
            cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

// ======== Data Structures ========
struct Vehicle
{
    string id; // License plate (e.g., "RAD 220 C")
    string type, owner;
    Vehicle *next;
};

struct ParkingSpot
{
    int id;
    string type;
    bool isOccupied;
    ParkingSpot *next;
};

struct ParkingSession
{
    int id;
    string vehicleId; // License plate
    int spotId;
    string entryTime, exitTime;
    ParkingSession *next;
};

// ======== ParkingLot Class ========
class ParkingLot
{
public:
    string lotId, name, location;
    Vehicle *vehicles = nullptr;
    ParkingSpot *spots = nullptr;
    ParkingSession *sessions = nullptr;

    int nextSpotId = 1;
    int nextSessionId = 1;

    ParkingLot() = default;
    ParkingLot(const string &id, const string &nm, const string &loc)
        : lotId(id), name(nm), location(loc)
    {
        loadData();
        normalizeCounters();
        updateSpotStatuses();
    }

    bool registerVehicle(const string &lp, const string &t, const string &own)
    {
        if (findVehicle(lp))
        {
            cout << "Vehicle already exists!\n";
            return false;
        }
        vehicles = new Vehicle{lp, t, own, vehicles};
        saveData();
        return true;
    }

    int addParkingSpot(const string &t)
    {
        int id = nextSpotId++;
        spots = new ParkingSpot{id, t, false, spots};
        saveData();
        return id;
    }

    int startParkingSession(const string &vId, int sid, const string &entry)
    {
        if (!findVehicle(vId) || !findSpot(sid))
            return -1;
        ParkingSpot *spot = findSpot(sid);
        if (spot->isOccupied)
            return -2;

        int id = nextSessionId++;
        sessions = new ParkingSession{id, vId, sid, entry, "", sessions};
        spot->isOccupied = true;
        saveData();
        return id;
    }

    bool endParkingSession(int sessionId, const string &exit)
    {
        ParkingSession *session = findSession(sessionId);
        if (!session || session->exitTime != "")
            return false;

        session->exitTime = exit;
        ParkingSpot *spot = findSpot(session->spotId);
        if (spot)
            spot->isOccupied = false;
        saveData();
        return true;
    }

    // Delete functions
    bool deleteVehicle(const string &vId)
    {
        Vehicle **ptr = &vehicles;
        while (*ptr)
        {
            if ((*ptr)->id == vId)
            {
                // Check for active sessions
                for (auto *s = sessions; s; s = s->next)
                {
                    if (s->vehicleId == vId && s->exitTime == "")
                    {
                        cout << "Cannot delete vehicle with active session\n";
                        return false;
                    }
                }
                Vehicle *temp = *ptr;
                *ptr = temp->next;
                delete temp;
                saveData();
                return true;
            }
            ptr = &(*ptr)->next;
        }
        return false;
    }

    bool deleteSpot(int sid)
    {
        ParkingSpot **ptr = &spots;
        while (*ptr)
        {
            if ((*ptr)->id == sid)
            {
                if ((*ptr)->isOccupied)
                {
                    cout << "Cannot delete occupied spot\n";
                    return false;
                }
                ParkingSpot *temp = *ptr;
                *ptr = temp->next;
                delete temp;
                saveData();
                return true;
            }
            ptr = &(*ptr)->next;
        }
        return false;
    }

    bool deleteSession(int sid)
    {
        ParkingSession **ptr = &sessions;
        while (*ptr)
        {
            if ((*ptr)->id == sid)
            {
                ParkingSession *temp = *ptr;
                // Free up the parking spot
                ParkingSpot *spot = findSpot(temp->spotId);
                if (spot)
                    spot->isOccupied = false;
                *ptr = temp->next;
                delete temp;
                saveData();
                return true;
            }
            ptr = &(*ptr)->next;
        }
        return false;
    }

    void displayVehicles()
    {
        cout << "-- Vehicles in " << name << " (" << lotId << ") --\n";
        for (auto *v = vehicles; v; v = v->next)
            cout << "License: " << v->id << " | Type: " << v->type
                 << " | Owner: " << v->owner << "\n";
    }

    void displaySpots()
    {
        cout << "-- Parking Spots in " << name << " (" << lotId << ") --\n";
        for (auto *s = spots; s; s = s->next)
            cout << s->id << ": " << s->type << " | "
                 << (s->isOccupied ? "Occupied" : "Available") << "\n";
    }

    void displaySessions(bool currentOnly = false)
    {
        cout << "-- Parking Sessions in " << name << " (" << lotId << ") --\n";
        for (auto *s = sessions; s; s = s->next)
        {
            if (!currentOnly || s->exitTime == "")
                cout << s->id << ": " << s->vehicleId << " @ S" << s->spotId
                     << " | " << s->entryTime << " - "
                     << (s->exitTime == "" ? "Ongoing" : s->exitTime) << "\n";
        }
    }

    void loadData()
    {
        loadList<Vehicle>(lotId + "_vehicles.csv", vehicles);
        loadList<ParkingSpot>(lotId + "_spots.csv", spots);
        loadList<ParkingSession>(lotId + "_sessions.csv", sessions);
    }

    void saveData()
    {
        saveVehicles(lotId + "_vehicles.csv");
        saveSpots(lotId + "_spots.csv");
        saveSessions(lotId + "_sessions.csv");
    }

private:
    Vehicle *findVehicle(const string &vId)
    {
        for (auto *v = vehicles; v; v = v->next)
            if (v->id == vId)
                return v;
        return nullptr;
    }

    ParkingSpot *findSpot(int id)
    {
        for (auto *s = spots; s; s = s->next)
            if (s->id == id)
                return s;
        return nullptr;
    }

    ParkingSession *findSession(int id)
    {
        for (auto *s = sessions; s; s = s->next)
            if (s->id == id)
                return s;
        return nullptr;
    }

    void updateSpotStatuses()
    {
        unordered_set<int> occupiedSpots;
        for (auto *s = sessions; s; s = s->next)
        {
            if (s->exitTime == "")
            {
                occupiedSpots.insert(s->spotId);
            }
        }
        for (auto *spot = spots; spot; spot = spot->next)
        {
            spot->isOccupied = occupiedSpots.count(spot->id);
        }
    }

    template <typename T>
    void loadList(const string &fn, T *&head)
    {
        ifstream f(fn);
        if (!f)
            return;
        string line;
        getline(f, line);
        while (getline(f, line))
        {
            stringstream ss(line);
            vector<string> cols;
            string tok;
            while (getline(ss, tok, ','))
                cols.push_back(tok);

            if constexpr (is_same<T, Vehicle>::value)
            {
                head = new Vehicle{cols[0], cols[1], cols[2], head};
            }
            else if constexpr (is_same<T, ParkingSpot>::value)
            {
                head = new ParkingSpot{stoi(cols[0]), cols[1], cols[2] == "1", head};
            }
            else
            {
                head = new ParkingSession{stoi(cols[0]), cols[1],
                                          stoi(cols[2]), cols[3], cols[4], head};
            }
        }
    }

    void saveVehicles(const string &fn)
    {
        ofstream f(fn);
        f << "license_plate,type,owner\n";
        for (auto *v = vehicles; v; v = v->next)
            f << v->id << ',' << v->type << ',' << v->owner << "\n";
    }

    void saveSpots(const string &fn)
    {
        ofstream f(fn);
        f << "id,type,is_occupied\n";
        for (auto *s = spots; s; s = s->next)
            f << s->id << ',' << s->type << ',' << (s->isOccupied ? 1 : 0) << "\n";
    }

    void saveSessions(const string &fn)
    {
        ofstream f(fn);
        f << "id,vehicle_id,spot_id,entry_time,exit_time\n";
        for (auto *s = sessions; s; s = s->next)
            f << s->id << ',' << s->vehicleId << ',' << s->spotId << ','
              << s->entryTime << ',' << s->exitTime << "\n";
    }

    void normalizeCounters()
    {
        for (auto *s = spots; s; s = s->next)
            nextSpotId = max(nextSpotId, s->id + 1);
        for (auto *s = sessions; s; s = s->next)
            nextSessionId = max(nextSessionId, s->id + 1);
    }
};

// ======== ParkingNetwork Class ========
class ParkingNetwork
{
private:
    bool areConnected(const string &a, const string &b)
    {
        if (adj.find(a) == adj.end())
            return false;
        for (const auto &edge : adj[a])
            if (edge.first == b)
                return true;
        return false;
    }

public:
    unordered_map<string, ParkingLot *> nodes;
    unordered_map<string, vector<pair<string, int>>> adj;
    int nextLotIndex = 1;

    ParkingNetwork()
    {
        loadLots();
        loadConnections();
    }

    void addParkingLot()
    {
        cout << "Name: ";
        string nm;
        getline(cin, nm);
        cout << "Location: ";
        string loc;
        getline(cin, loc);
        string id = genId();
        nodes[id] = new ParkingLot(id, nm, loc);
        saveLots();
        saveConnections();
        cout << "Added: " << id << "\n";
    }

    void updateParkingLot()
    {
        cout << "Parking Lot ID to update: ";
        string id;
        getline(cin, id);
        
        if (!nodes.count(id))
        {
            cout << "Parking lot not found.\n";
            return;
        }

        ParkingLot *lot = nodes[id];
        cout << "\nCurrent Information:\n";
        cout << "ID: " << lot->lotId << "\n";
        cout << "Name: " << lot->name << "\n";
        cout << "Location: " << lot->location << "\n\n";

        cout << "What would you like to update?\n";
        cout << "1. Name\n";
        cout << "2. Location\n";
        cout << "3. Both Name and Location\n";
        
        int choice = readInt("Choose: ", 1, 3);
        
        string newName = lot->name;
        string newLocation = lot->location;
        
        if (choice == 1 || choice == 3)
        {
            cout << "Enter new name (current: " << lot->name << "): ";
            getline(cin, newName);
            if (newName.empty())
            {
                cout << "Name cannot be empty. Keeping current name.\n";
                newName = lot->name;
            }
        }
        
        if (choice == 2 || choice == 3)
        {
            cout << "Enter new location (current: " << lot->location << "): ";
            getline(cin, newLocation);
            if (newLocation.empty())
            {
                cout << "Location cannot be empty. Keeping current location.\n";
                newLocation = lot->location;
            }
        }
        
        // Update the lot information
        lot->name = newName;
        lot->location = newLocation;
        
        // Save changes to file
        saveLots();
        
        cout << "\nParking lot updated successfully!\n";
        cout << "Updated Information:\n";
        cout << "ID: " << lot->lotId << "\n";
        cout << "Name: " << lot->name << "\n";
        cout << "Location: " << lot->location << "\n";
    }

    void deleteParkingLot()
    {
        cout << "Lot ID to delete: ";
        string id;
        getline(cin, id);
        if (!nodes.count(id))
        {
            cout << "Not found.\n";
            return;
        }

        // Delete associated files
        remove((id + "_vehicles.csv").c_str());
        remove((id + "_spots.csv").c_str());
        remove((id + "_sessions.csv").c_str());

        // Remove from network
        delete nodes[id];
        nodes.erase(id);
        adj.erase(id);

        // Remove from other lots' connections
        for (auto &kv : adj)
        {
            auto &v = kv.second;
            v.erase(remove_if(v.begin(), v.end(),
                              [&](auto &p)
                              { return p.first == id; }),
                    v.end());
        }

        saveLots();
        saveConnections();
        cout << "Deleted " << id << "\n";
    }

    void connectParkingLots()
    {
        cout << "From Lot ID: ";
        string a;
        getline(cin, a);
        cout << "To Lot ID:   ";
        string b;
        getline(cin, b);
        if (!nodes.count(a) || !nodes.count(b))
        {
            cout << "Invalid IDs.\n";
            return;
        }
        if (areConnected(a, b))
        {
            cout << "Lots are already connected.\n";
            return;
        }
        int dist = readInt("Distance (meters): ", 0);
        adj[a].push_back({b, dist});
        adj[b].push_back({a, dist});
        saveConnections();
        cout << "Connected " << a << " <-> " << b << "\n";
    }

    void listParkingLots()
    {
        cout << "-- Parking Lots --\n";
        for (auto &kv : nodes)
            cout << kv.first << " | " << kv.second->name
                 << " | " << kv.second->location << "\n";
    }

    void displayNetwork()
    {
        cout << "-- Parking Network --\n";
        for (auto &kv : adj)
        {
            cout << kv.first << " -> ";
            for (auto &e : kv.second)
                cout << e.first << "(" << e.second << "m) ";
            cout << "\n";
        }
    }

    void manageParkingLot()
    {
        cout << "Parking Lot ID: ";
        string lid;
        getline(cin, lid);
        if (!nodes.count(lid))
        {
            cout << "Not found.\n";
            return;
        }
        ParkingLot *lot = nodes[lid];
        while (true)
        {
            cout << "\n-- Managing " << lot->name << " (" << lid << ") --\n"
                 << "1. Register Vehicle\n"
                 << "2. Add Parking Spot\n"
                 << "3. Start Parking Session\n"
                 << "4. End Parking Session\n"
                 << "5. Display Vehicles\n"
                 << "6. Display Spots\n"
                 << "7. Display Current Sessions\n"
                 << "8. Display All Sessions\n"
                 << "9. View Connections\n"
                 << "10. Delete Vehicle\n"
                 << "11. Delete Spot\n"
                 << "12. Delete Session\n"
                 << "13. Go Back\n";
            int c = readInt("Choose: ", 1, 13);
            if (c == 13)
                break;
            switch (c)
            {
            case 1:
            {
                cout << "License Plate: ";
                string lp;
                getline(cin, lp);
                cout << "Vehicle Type: ";
                string t;
                getline(cin, t);
                cout << "Owner Name: ";
                string own;
                getline(cin, own);
                if (lot->registerVehicle(lp, t, own))
                    cout << "Vehicle registered\n";
                else
                    cout << "Registration failed\n";
                break;
            }
            case 2:
            {
                cout << "Spot Type: ";
                string t;
                getline(cin, t);
                cout << "Added Spot " << lot->addParkingSpot(t) << "\n";
                break;
            }
            case 3:
            {
                cout << "Vehicle License: ";
                string vId;
                getline(cin, vId);
                int sid = readInt("Spot ID: ", 1);
                time_t now = time(0);
                string entry = ctime(&now);
                entry.erase(entry.find('\n'));
                int id = lot->startParkingSession(vId, sid, entry);
                if (id == -1)
                    cout << "Invalid IDs\n";
                else if (id == -2)
                    cout << "Spot occupied\n";
                else
                    cout << "Session started: " << id << "\n";
                break;
            }
            case 4:
            {
                int sid = readInt("Session ID: ", 1);
                time_t now = time(0);
                string exit = ctime(&now);
                exit.erase(exit.find('\n'));
                if (lot->endParkingSession(sid, exit))
                    cout << "Session ended\n";
                else
                    cout << "Invalid session or already ended\n";
                break;
            }
            case 5:
                lot->displayVehicles();
                break;
            case 6:
                lot->displaySpots();
                break;
            case 7:
                lot->displaySessions(true);
                break;
            case 8:
                lot->displaySessions();
                break;
            case 9:
            {
                cout << "-- Connections from " << lid << " --\n";
                for (auto &e : adj[lid])
                    cout << e.first << "(" << e.second << "m)\n";
                break;
            }
            case 10:
            {
                cout << "Vehicle License: ";
                string vId;
                getline(cin, vId);
                if (lot->deleteVehicle(vId))
                    cout << "Vehicle deleted\n";
                else
                    cout << "Delete failed\n";
                break;
            }
            case 11:
            {
                int sid = readInt("Spot ID: ", 1);
                if (lot->deleteSpot(sid))
                    cout << "Spot deleted\n";
                else
                    cout << "Delete failed\n";
                break;
            }
            case 12:
            {
                int sid = readInt("Session ID: ", 1);
                if (lot->deleteSession(sid))
                    cout << "Session deleted\n";
                else
                    cout << "Delete failed\n";
                break;
            }
            }
        }
    }

private:
    string genId() { return "L" + to_string(nextLotIndex++); }

    void loadLots()
    {
        ifstream f("parking_lots.csv");
        if (!f)
            return;
        string line;
        getline(f, line);
        int maxIdx = 0;
        while (getline(f, line))
        {
            stringstream ss(line);
            string id, nm, loc;
            getline(ss, id, ',');
            getline(ss, nm, ',');
            getline(ss, loc, ',');
            maxIdx = max(maxIdx, stoi(id.substr(1)));
            nodes[id] = new ParkingLot(id, nm, loc);
        }
        nextLotIndex = maxIdx + 1;
    }

    void saveLots()
    {
        ofstream f("parking_lots.csv");
        f << "id,name,location\n";
        for (auto &kv : nodes)
            f << kv.first << ',' << kv.second->name
              << ',' << kv.second->location << "\n";
    }

    void loadConnections()
    {
        ifstream f("connections.csv");
        if (!f)
            return;
        string line;
        getline(f, line);
        while (getline(f, line))
        {
            stringstream ss(line);
            string a, b;
            int d;
            getline(ss, a, ',');
            getline(ss, b, ',');
            ss >> d;
            if (nodes.count(a) && nodes.count(b))
            {
                adj[a].push_back({b, d});
                adj[b].push_back({a, d});
            }
        }
    }

    void saveConnections()
    {
        ofstream f("connections.csv");
        f << "from,to,distance\n";
        for (auto &kv : adj)
        {
            const string &a = kv.first;
            for (auto &e : kv.second)
            {
                const string &b = e.first;
                int d = e.second;
                if (a < b)
                    f << a << ',' << b << ',' << d << "\n";
            }
        }
    }
};

// ======== Main Function ========
int main()
{
    ParkingNetwork pn;
    while (true)
    {
        cout << "\n=== Parking Management System ===\n"
             << "1. Add Parking Lot\n"
             << "2. Update Parking Lot\n"
             << "3. Manage Parking Lot\n"
             << "4. Connect Parking Lots\n"
             << "5. List Parking Lots\n"
             << "6. Display Network\n"
             << "7. Delete Parking Lot\n"
             << "8. Exit\n";
        int choice = readInt("Choose: ", 1, 8);
        if (choice == 8)
            break;
        switch (choice)
        {
        case 1:
            pn.addParkingLot();
            break;
        case 2:
            pn.updateParkingLot();
            break;
        case 3:
            pn.manageParkingLot();
            break;
        case 4:
            pn.connectParkingLots();
            break;
        case 5:
            pn.listParkingLots();
            break;
        case 6:
            pn.displayNetwork();
            break;
        case 7:
            pn.deleteParkingLot();
            break;
        }
    }
    cout << "Goodbye!\n";
    return 0;
}