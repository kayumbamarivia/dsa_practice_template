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
int readInt(const string &prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            cin.clear();
            continue;
        }
        try {
            int val = stoi(line);
            if (val < minVal || val > maxVal) {
                cout << "Please enter a number";
                if (minVal != INT_MIN && maxVal != INT_MAX)
                    cout << " between " << minVal << " and " << maxVal;
                cout << ".\n";
                continue;
            }
            return val;
        }
        catch (...) {
            cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

// ======== Data Structures ========
struct Guest {
    string id; // Passport/ID number
    string name, contact;
    Guest *next;
};

struct Room {
    int id;
    string type;
    bool isOccupied;
    double rate;
    Room *next;
};

struct Booking {
    int id;
    string guestId;
    int roomId;
    string checkInDate, checkOutDate;
    double totalCharge;
    Booking *next;
};

// ======== Hotel Class ========
class Hotel {
public:
    string hotelId, name, location;
    Guest *guests = nullptr;
    Room *rooms = nullptr;
    Booking *bookings = nullptr;

    int nextRoomId = 1;
    int nextBookingId = 1;

    Hotel() = default;
    Hotel(const string &id, const string &nm, const string &loc)
        : hotelId(id), name(nm), location(loc) {
        loadData();
        normalizeCounters();
        updateRoomStatuses();
    }

    bool registerGuest(const string &gId, const string &nm, const string &cnt) {
        if (findGuest(gId)) {
            cout << "Guest already exists!\n";
            return false;
        }
        guests = new Guest{gId, nm, cnt, guests};
        saveData();
        return true;
    }

    int addRoom(const string &t, double r) {
        int id = nextRoomId++;
        rooms = new Room{id, t, false, r, rooms};
        saveData();
        return id;
    }

    int createBooking(const string &gId, int rid, const string &checkIn, const string &checkOut) {
        if (!findGuest(gId) || !findRoom(rid))
            return -1;
        Room *room = findRoom(rid);
        if (room->isOccupied)
            return -2;

        int id = nextBookingId++;
        // Calculate total charge based on days stayed
        int days = calculateDays(checkIn, checkOut);
        double total = days * room->rate;
        
        bookings = new Booking{id, gId, rid, checkIn, checkOut, total, bookings};
        room->isOccupied = true;
        saveData();
        return id;
    }

    bool checkOut(int bookingId, const string &actualCheckOut) {
        Booking *booking = findBooking(bookingId);
        if (!booking || booking->checkOutDate != "")
            return false;

        booking->checkOutDate = actualCheckOut;
        Room *room = findRoom(booking->roomId);
        if (room)
            room->isOccupied = false;
        saveData();
        return true;
    }

    // Delete functions
    bool deleteGuest(const string &gId) {
        Guest **ptr = &guests;
        while (*ptr) {
            if ((*ptr)->id == gId) {
                // Check for active bookings
                for (auto *b = bookings; b; b = b->next) {
                    if (b->guestId == gId && b->checkOutDate == "") {
                        cout << "Cannot delete guest with active booking\n";
                        return false;
                    }
                }
                Guest *temp = *ptr;
                *ptr = temp->next;
                delete temp;
                saveData();
                return true;
            }
            ptr = &(*ptr)->next;
        }
        return false;
    }

    bool deleteRoom(int rid) {
        Room **ptr = &rooms;
        while (*ptr) {
            if ((*ptr)->id == rid) {
                if ((*ptr)->isOccupied) {
                    cout << "Cannot delete occupied room\n";
                    return false;
                }
                Room *temp = *ptr;
                *ptr = temp->next;
                delete temp;
                saveData();
                return true;
            }
            ptr = &(*ptr)->next;
        }
        return false;
    }

    bool deleteBooking(int bid) {
        Booking **ptr = &bookings;
        while (*ptr) {
            if ((*ptr)->id == bid) {
                Booking *temp = *ptr;
                // Free up the room
                Room *room = findRoom(temp->roomId);
                if (room)
                    room->isOccupied = false;
                *ptr = temp->next;
                delete temp;
                saveData();
                return true;
            }
            ptr = &(*ptr)->next;
        }
        return false;
    }

    void displayGuests() {
        cout << "-- Guests in " << name << " (" << hotelId << ") --\n";
        for (auto *g = guests; g; g = g->next)
            cout << "ID: " << g->id << " | Name: " << g->name
                 << " | Contact: " << g->contact << "\n";
    }

    void displayRooms() {
        cout << "-- Rooms in " << name << " (" << hotelId << ") --\n";
        for (auto *r = rooms; r; r = r->next)
            cout << r->id << ": " << r->type << " | Rate: $" << r->rate << "/night | "
                 << (r->isOccupied ? "Occupied" : "Available") << "\n";
    }

    void displayBookings(bool currentOnly = false) {
        cout << "-- Bookings in " << name << " (" << hotelId << ") --\n";
        for (auto *b = bookings; b; b = b->next) {
            if (!currentOnly || b->checkOutDate == "") {
                cout << b->id << ": Guest " << b->guestId << " @ Room " << b->roomId
                     << " | " << b->checkInDate << " - "
                     << (b->checkOutDate == "" ? "Current Stay" : b->checkOutDate)
                     << " | Total: $" << b->totalCharge << "\n";
            }
        }
    }

    void loadData() {
        loadGuestList(hotelId + "_guests.csv", guests);
        loadRoomList(hotelId + "_rooms.csv", rooms);
        loadBookingList(hotelId + "_bookings.csv", bookings);
    }

    void saveData() {
        saveGuests(hotelId + "_guests.csv");
        saveRooms(hotelId + "_rooms.csv");
        saveBookings(hotelId + "_bookings.csv");
    }

private:
    Guest *findGuest(const string &gId) {
        for (auto *g = guests; g; g = g->next)
            if (g->id == gId)
                return g;
        return nullptr;
    }

    Room *findRoom(int id) {
        for (auto *r = rooms; r; r = r->next)
            if (r->id == id)
                return r;
        return nullptr;
    }

    Booking *findBooking(int id) {
        for (auto *b = bookings; b; b = b->next)
            if (b->id == id)
                return b;
        return nullptr;
    }

    void updateRoomStatuses() {
        unordered_set<int> occupiedRooms;
        for (auto *b = bookings; b; b = b->next) {
            if (b->checkOutDate == "") {
                occupiedRooms.insert(b->roomId);
            }
        }
        for (auto *room = rooms; room; room = room->next) {
            room->isOccupied = occupiedRooms.count(room->id);
        }
    }

    int calculateDays(const string &checkIn, const string &checkOut) {
        // Simple date difference calculation (format: YYYY-MM-DD)
        // In a real system, you'd use proper date parsing
        return 1; // Placeholder - implement proper date calculation
    }

    void loadGuestList(const string &fn, Guest *&head) {
        ifstream f(fn);
        if (!f) return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            stringstream ss(line);
            string id, name, contact;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, contact, ',');
            head = new Guest{id, name, contact, head};
        }
    }

    void loadRoomList(const string &fn, Room *&head) {
        ifstream f(fn);
        if (!f) return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            stringstream ss(line);
            string id, type, occ, rate;
            getline(ss, id, ',');
            getline(ss, type, ',');
            getline(ss, occ, ',');
            getline(ss, rate, ',');
            head = new Room{stoi(id), type, occ == "1", stod(rate), head};
        }
    }

    void loadBookingList(const string &fn, Booking *&head) {
        ifstream f(fn);
        if (!f) return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            stringstream ss(line);
            string id, gId, rId, checkIn, checkOut, total;
            getline(ss, id, ',');
            getline(ss, gId, ',');
            getline(ss, rId, ',');
            getline(ss, checkIn, ',');
            getline(ss, checkOut, ',');
            getline(ss, total, ',');
            head = new Booking{stoi(id), gId, stoi(rId), checkIn, checkOut, stod(total), head};
        }
    }

    void saveGuests(const string &fn) {
        ofstream f(fn);
        f << "guest_id,name,contact\n";
        for (auto *g = guests; g; g = g->next)
            f << g->id << ',' << g->name << ',' << g->contact << "\n";
    }

    void saveRooms(const string &fn) {
        ofstream f(fn);
        f << "id,type,is_occupied,rate\n";
        for (auto *r = rooms; r; r = r->next)
            f << r->id << ',' << r->type << ',' << (r->isOccupied ? 1 : 0) 
              << ',' << r->rate << "\n";
    }

    void saveBookings(const string &fn) {
        ofstream f(fn);
        f << "id,guest_id,room_id,check_in,check_out,total_charge\n";
        for (auto *b = bookings; b; b = b->next)
            f << b->id << ',' << b->guestId << ',' << b->roomId << ','
              << b->checkInDate << ',' << b->checkOutDate << ',' << b->totalCharge << "\n";
    }

    void normalizeCounters() {
        for (auto *r = rooms; r; r = r->next)
            nextRoomId = max(nextRoomId, r->id + 1);
        for (auto *b = bookings; b; b = b->next)
            nextBookingId = max(nextBookingId, b->id + 1);
    }
};

// ======== HotelChain Class ========
class HotelChain {
private:
    bool areConnected(const string &a, const string &b) {
        if (adj.find(a) == adj.end())
            return false;
        for (const auto &edge : adj[a])
            if (edge.first == b)
                return true;
        return false;
    }

public:
    unordered_map<string, Hotel *> hotels;
    unordered_map<string, vector<pair<string, int>>> adj;
    int nextHotelIndex = 1;

    HotelChain() {
        loadHotels();
        loadConnections();
    }

    void addHotel() {
        cout << "Hotel Name: ";
        string nm;
        getline(cin, nm);
        cout << "Location: ";
        string loc;
        getline(cin, loc);
        string id = genId();
        hotels[id] = new Hotel(id, nm, loc);
        saveHotels();
        saveConnections();
        cout << "Added Hotel: " << id << "\n";
    }

    void updateHotel() {
        cout << "Hotel ID to update: ";
        string id;
        getline(cin, id);
        
        if (!hotels.count(id)) {
            cout << "Hotel not found.\n";
            return;
        }

        Hotel *hotel = hotels[id];
        cout << "\nCurrent Information:\n";
        cout << "ID: " << hotel->hotelId << "\n";
        cout << "Name: " << hotel->name << "\n";
        cout << "Location: " << hotel->location << "\n\n";

        cout << "What would you like to update?\n";
        cout << "1. Name\n";
        cout << "2. Location\n";
        cout << "3. Both Name and Location\n";
        
        int choice = readInt("Choose: ", 1, 3);
        
        string newName = hotel->name;
        string newLocation = hotel->location;
        
        if (choice == 1 || choice == 3) {
            cout << "Enter new name (current: " << hotel->name << "): ";
            getline(cin, newName);
            if (newName.empty()) {
                cout << "Name cannot be empty. Keeping current name.\n";
                newName = hotel->name;
            }
        }
        
        if (choice == 2 || choice == 3) {
            cout << "Enter new location (current: " << hotel->location << "): ";
            getline(cin, newLocation);
            if (newLocation.empty()) {
                cout << "Location cannot be empty. Keeping current location.\n";
                newLocation = hotel->location;
            }
        }
        
        hotel->name = newName;
        hotel->location = newLocation;
        saveHotels();
        
        cout << "\nHotel updated successfully!\n";
        cout << "Updated Information:\n";
        cout << "ID: " << hotel->hotelId << "\n";
        cout << "Name: " << hotel->name << "\n";
        cout << "Location: " << hotel->location << "\n";
    }

    void deleteHotel() {
        cout << "Hotel ID to delete: ";
        string id;
        getline(cin, id);
        if (!hotels.count(id)) {
            cout << "Not found.\n";
            return;
        }

        // Delete associated files
        remove((id + "_guests.csv").c_str());
        remove((id + "_rooms.csv").c_str());
        remove((id + "_bookings.csv").c_str());

        // Remove from network
        delete hotels[id];
        hotels.erase(id);
        adj.erase(id);

        // Remove from other hotels' connections
        for (auto &kv : adj) {
            auto &v = kv.second;
            v.erase(remove_if(v.begin(), v.end(),
                              [&](auto &p) { return p.first == id; }),
                    v.end());
        }

        saveHotels();
        saveConnections();
        cout << "Deleted Hotel " << id << "\n";
    }

    void connectHotels() {
        cout << "From Hotel ID: ";
        string a;
        getline(cin, a);
        cout << "To Hotel ID: ";
        string b;
        getline(cin, b);
        if (!hotels.count(a) || !hotels.count(b)) {
            cout << "Invalid IDs.\n";
            return;
        }
        if (areConnected(a, b)) {
            cout << "Hotels are already connected.\n";
            return;
        }
        int dist = readInt("Distance (km): ", 0);
        adj[a].push_back({b, dist});
        adj[b].push_back({a, dist});
        saveConnections();
        cout << "Connected " << a << " <-> " << b << "\n";
    }

    void listHotels() {
        cout << "-- Hotels in Chain --\n";
        for (auto &kv : hotels)
            cout << kv.first << " | " << kv.second->name
                 << " | " << kv.second->location << "\n";
    }

    void displayNetwork() {
        cout << "-- Hotel Network --\n";
        for (auto &kv : adj) {
            cout << kv.first << " -> ";
            for (auto &e : kv.second)
                cout << e.first << "(" << e.second << "km) ";
            cout << "\n";
        }
    }

    void manageHotel() {
        cout << "Hotel ID: ";
        string hid;
        getline(cin, hid);
        if (!hotels.count(hid)) {
            cout << "Not found.\n";
            return;
        }
        Hotel *hotel = hotels[hid];
        while (true) {
            cout << "\n-- Managing " << hotel->name << " (" << hid << ") --\n"
                 << "1. Register Guest\n"
                 << "2. Add Room\n"
                 << "3. Create Booking\n"
                 << "4. Check Out\n"
                 << "5. Display Guests\n"
                 << "6. Display Rooms\n"
                 << "7. Display Current Bookings\n"
                 << "8. Display All Bookings\n"
                 << "9. View Connections\n"
                 << "10. Delete Guest\n"
                 << "11. Delete Room\n"
                 << "12. Delete Booking\n"
                 << "13. Go Back\n";
            int c = readInt("Choose: ", 1, 13);
            if (c == 13)
                break;
            switch (c) {
            case 1: {
                cout << "Guest ID: ";
                string gId;
                getline(cin, gId);
                cout << "Guest Name: ";
                string name;
                getline(cin, name);
                cout << "Contact Info: ";
                string contact;
                getline(cin, contact);
                if (hotel->registerGuest(gId, name, contact))
                    cout << "Guest registered\n";
                else
                    cout << "Registration failed\n";
                break;
            }
            case 2: {
                cout << "Room Type: ";
                string t;
                getline(cin, t);
                double rate = readInt("Nightly Rate: $", 0);
                cout << "Added Room " << hotel->addRoom(t, rate) << "\n";
                break;
            }
            case 3: {
                cout << "Guest ID: ";
                string gId;
                getline(cin, gId);
                int rid = readInt("Room ID: ", 1);
                cout << "Check-in Date (YYYY-MM-DD): ";
                string checkIn;
                getline(cin, checkIn);
                cout << "Check-out Date (YYYY-MM-DD): ";
                string checkOut;
                getline(cin, checkOut);
                int id = hotel->createBooking(gId, rid, checkIn, checkOut);
                if (id == -1)
                    cout << "Invalid IDs\n";
                else if (id == -2)
                    cout << "Room occupied\n";
                else
                    cout << "Booking created: " << id << "\n";
                break;
            }
            case 4: {
                int bid = readInt("Booking ID: ", 1);
                cout << "Actual Check-out Date (YYYY-MM-DD): ";
                string checkOut;
                getline(cin, checkOut);
                if (hotel->checkOut(bid, checkOut))
                    cout << "Check-out completed\n";
                else
                    cout << "Invalid booking or already checked out\n";
                break;
            }
            case 5:
                hotel->displayGuests();
                break;
            case 6:
                hotel->displayRooms();
                break;
            case 7:
                hotel->displayBookings(true);
                break;
            case 8:
                hotel->displayBookings();
                break;
            case 9: {
                cout << "-- Connections from " << hid << " --\n";
                for (auto &e : adj[hid])
                    cout << e.first << "(" << e.second << "km)\n";
                break;
            }
            case 10: {
                cout << "Guest ID: ";
                string gId;
                getline(cin, gId);
                if (hotel->deleteGuest(gId))
                    cout << "Guest deleted\n";
                else
                    cout << "Delete failed\n";
                break;
            }
            case 11: {
                int rid = readInt("Room ID: ", 1);
                if (hotel->deleteRoom(rid))
                    cout << "Room deleted\n";
                else
                    cout << "Delete failed\n";
                break;
            }
            case 12: {
                int bid = readInt("Booking ID: ", 1);
                if (hotel->deleteBooking(bid))
                    cout << "Booking deleted\n";
                else
                    cout << "Delete failed\n";
                break;
            }
            }
        }
    }

private:
    string genId() { return "H" + to_string(nextHotelIndex++); }

    void loadHotels() {
        ifstream f("hotels.csv");
        if (!f)
            return;
        string line;
        getline(f, line);
        int maxIdx = 0;
        while (getline(f, line)) {
            stringstream ss(line);
            string id, nm, loc;
            getline(ss, id, ',');
            getline(ss, nm, ',');
            getline(ss, loc, ',');
            maxIdx = max(maxIdx, stoi(id.substr(1)));
            hotels[id] = new Hotel(id, nm, loc);
        }
        nextHotelIndex = maxIdx + 1;
    }

    void saveHotels() {
        ofstream f("hotels.csv");
        f << "id,name,location\n";
        for (auto &kv : hotels)
            f << kv.first << ',' << kv.second->name
              << ',' << kv.second->location << "\n";
    }

    void loadConnections() {
        ifstream f("connections.csv");
        if (!f)
            return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            stringstream ss(line);
            string a, b;
            int d;
            getline(ss, a, ',');
            getline(ss, b, ',');
            ss >> d;
            if (hotels.count(a) && hotels.count(b)) {
                adj[a].push_back({b, d});
                adj[b].push_back({a, d});
            }
        }
    }

    void saveConnections() {
        ofstream f("connections.csv");
        f << "from,to,distance\n";
        for (auto &kv : adj) {
            const string &a = kv.first;
            for (auto &e : kv.second) {
                const string &b = e.first;
                int d = e.second;
                if (a < b)
                    f << a << ',' << b << ',' << d << "\n";
            }
        }
    }
};

// ======== Main Function ========
int main() {
    HotelChain hc;
    while (true) {
        cout << "\n=== Hotel Management System ===\n"
             << "1. Add Hotel\n"
             << "2. Update Hotel\n"
             << "3. Manage Hotel\n"
             << "4. Connect Hotels\n"
             << "5. List Hotels\n"
             << "6. Display Network\n"
             << "7. Delete Hotel\n"
             << "8. Exit\n";
        int choice = readInt("Choose: ", 1, 8);
        if (choice == 8)
            break;
        switch (choice) {
        case 1:
            hc.addHotel();
            break;
        case 2:
            hc.updateHotel();
            break;
        case 3:
            hc.manageHotel();
            break;
        case 4:
            hc.connectHotels();
            break;
        case 5:
            hc.listHotels();
            break;
        case 6:
            hc.displayNetwork();
            break;
        case 7:
            hc.deleteHotel();
            break;
        }
    }
    cout << "Goodbye!\n";
    return 0;
}