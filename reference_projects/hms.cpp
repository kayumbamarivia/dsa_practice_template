#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm> // std::remove_if
#include <climits>   // INT_MIN, INT_MAX

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
struct Patient
{
    int id;
    string name, dob, gender;
    Patient *next;
};
struct Doctor
{
    int id;
    string name, specialization;
    Doctor *next;
};
struct Appointment
{
    int id, patientId, doctorId;
    string date;
    Appointment *next;
};

// ======== Hospital Class ========
class Hospital
{
public:
    string hospitalId, name, location;
    Patient *patients = nullptr;
    Doctor *doctors = nullptr;
    Appointment *appointments = nullptr;

    int nextPatientId = 1;
    int nextDoctorId = 1;
    int nextAppointmentId = 1;

    Hospital() = default;
    Hospital(const string &id, const string &nm, const string &loc)
        : hospitalId(id), name(nm), location(loc)
    {
        loadData();
        normalizeCounters();
    }

    int registerPatient(const string &n, const string &d, const string &g)
    {
        int id = nextPatientId++;
        patients = new Patient{id, n, d, g, patients};
        saveData();
        return id;
    }
    int registerDoctor(const string &n, const string &spec)
    {
        int id = nextDoctorId++;
        doctors = new Doctor{id, n, spec, doctors};
        saveData();
        return id;
    }
    int registerAppointment(int pid, int did, const string &dt)
    {
        if (!findPatient(pid) || !findDoctor(did))
            return -1;
        int id = nextAppointmentId++;
        appointments = new Appointment{id, pid, did, dt, appointments};
        saveData();
        return id;
    }

    void displayPatients()
    {
        cout << "-- Patients in " << name << " (" << hospitalId << ") --\n";
        for (auto *p = patients; p; p = p->next)
            cout << p->id << ": " << p->name << " | " << p->dob << " | " << p->gender << "\n";
    }
    void displayDoctors()
    {
        cout << "-- Doctors in " << name << " (" << hospitalId << ") --\n";
        for (auto *d = doctors; d; d = d->next)
            cout << d->id << ": " << d->name << " | " << d->specialization << "\n";
    }
    void displayAppointments()
    {
        cout << "-- Appointments in " << name << " (" << hospitalId << ") --\n";
        for (auto *a = appointments; a; a = a->next)
            cout << a->id << ": P" << a->patientId << " → D" << a->doctorId << " on " << a->date << "\n";
    }

    void loadData()
    {
        loadList<Patient>(hospitalId + "_patients.csv", patients);
        loadList<Doctor>(hospitalId + "_doctors.csv", doctors);
        loadList<Appointment>(hospitalId + "_appointments.csv", appointments);
    }
    void saveData()
    {
        savePatients(hospitalId + "_patients.csv");
        saveDoctors(hospitalId + "_doctors.csv");
        saveAppointments(hospitalId + "_appointments.csv");
    }

private:
    Patient *findPatient(int id)
    {
        for (auto *p = patients; p; p = p->next)
            if (p->id == id)
                return p;
        return nullptr;
    }
    Doctor *findDoctor(int id)
    {
        for (auto *d = doctors; d; d = d->next)
            if (d->id == id)
                return d;
        return nullptr;
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
            if constexpr (is_same<T, Patient>::value)
            {
                head = new Patient{stoi(cols[0]), cols[1], cols[2], cols[3], head};
            }
            else if constexpr (is_same<T, Doctor>::value)
            {
                head = new Doctor{stoi(cols[0]), cols[1], cols[2], head};
            }
            else
            {
                head = new Appointment{stoi(cols[0]), stoi(cols[1]), stoi(cols[2]), cols[3], head};
            }
        }
    }

    void savePatients(const string &fn)
    {
        ofstream f(fn);
        f << "id,name,dob,gender\n";
        for (auto *p = patients; p; p = p->next)
            f << p->id << ',' << p->name << ',' << p->dob << ',' << p->gender << "\n";
    }
    void saveDoctors(const string &fn)
    {
        ofstream f(fn);
        f << "id,name,specialization\n";
        for (auto *d = doctors; d; d = d->next)
            f << d->id << ',' << d->name << ',' << d->specialization << "\n";
    }
    void saveAppointments(const string &fn)
    {
        ofstream f(fn);
        f << "id,patientId,doctorId,date\n";
        for (auto *a = appointments; a; a = a->next)
            f << a->id << ',' << a->patientId << ',' << a->doctorId << ',' << a->date << "\n";
    }

    void normalizeCounters()
    {
        for (auto *p = patients; p; p = p->next)
            nextPatientId = max(nextPatientId, p->id + 1);
        for (auto *d = doctors; d; d = d->next)
            nextDoctorId = max(nextDoctorId, d->id + 1);
        for (auto *a = appointments; a; a = a->next)
            nextAppointmentId = max(nextAppointmentId, a->id + 1);
    }
};

// ======== Graph Class with Full CRUD on Connections ========
class Graph
{

private:
    bool areConnected(const string &a, const string &b)
    {
        if (adj.find(a) == adj.end())
            return false;
        for (const auto &edge : adj[a])
        {
            if (edge.first == b)
            {
                return true;
            }
        }
        return false;
    }

public:
    unordered_map<string, Hospital *> nodes;
    unordered_map<string, vector<pair<string, int>>> adj;
    int nextHospitalIndex = 1;

    Graph()
    {
        loadHospitals();
        loadConnections();
    }

    // --- Node operations ---
    void addHospital()
    {
        cout << "Name: ";
        string nm;
        getline(cin, nm);
        cout << "Location: ";
        string loc;
        getline(cin, loc);
        string id = genId();
        nodes[id] = new Hospital(id, nm, loc);
        saveHospitals();
        saveConnections();
        cout << "Added: " << id << "\n";
    }
    void deleteHospital()
    {
        cout << "Hospital ID to delete: ";
        string id;
        getline(cin, id);
        if (!nodes.count(id))
        {
            cout << "Not found.\n";
            return;
        }
        delete nodes[id];
        nodes.erase(id);
        adj.erase(id);
        for (auto &kv : adj)
        {
            auto &v = kv.second;
            v.erase(remove_if(v.begin(), v.end(),
                              [&](auto &p)
                              { return p.first == id; }),
                    v.end());
        }
        saveHospitals();
        saveConnections();
        cout << "Deleted " << id << "\n";
    }
    void updateHospitalInfo()
    {
        cout << "Hospital ID: ";
        string id;
        getline(cin, id);
        if (!nodes.count(id))
        {
            cout << "Not found.\n";
            return;
        }
        cout << "New Name (blank=keep): ";
        string nm;
        getline(cin, nm);
        cout << "New Location (blank=keep): ";
        string loc;
        getline(cin, loc);
        if (!nm.empty())
            nodes[id]->name = nm;
        if (!loc.empty())
            nodes[id]->location = loc;
        saveHospitals();
        cout << "Updated " << id << "\n";
    }

    // --- Connection operations ---
    void connectHospitals()
    {
        cout << "From ID: ";
        string a;
        getline(cin, a);
        cout << "To ID:   ";
        string b;
        getline(cin, b);
        if (!nodes.count(a) || !nodes.count(b))
        {
            cout << "Invalid IDs.\n";
            return;
        }
        if (areConnected(a, b))
        {
            cout << "Hospitals are already connected.\n";
            return;
        }
        int dist = readInt("Distance (km): ", 0);
        adj[a].push_back({b, dist});
        adj[b].push_back({a, dist});
        saveConnections();
        cout << "Connected " << a << " <-> " << b << "\n";
    }
    void updateConnection(const string &a, const string &b)
    {
        if (!nodes.count(a) || !nodes.count(b))
        {
            cout << "Invalid IDs.\n";
            return;
        }
        auto &va = adj[a], &vb = adj[b];
        int idxA = -1, idxB = -1;
        for (int i = 0; i < va.size(); ++i)
            if (va[i].first == b)
                idxA = i;
        for (int i = 0; i < vb.size(); ++i)
            if (vb[i].first == a)
                idxB = i;
        if (idxA < 0 || idxB < 0)
        {
            cout << "No connection found.\n";
            return;
        }
        int nd = readInt("New distance (km): ", 0);
        va[idxA].second = vb[idxB].second = nd;
        saveConnections();
        cout << "Updated " << a << "<->" << b << " to " << nd << "km\n";
    }
    void deleteConnection(const string &a, const string &b)
    {
        if (!nodes.count(a) || !nodes.count(b))
        {
            cout << "Invalid IDs.\n";
            return;
        }
        auto &va = adj[a], &vb = adj[b];
        va.erase(remove_if(va.begin(), va.end(),
                           [&](auto &p)
                           { return p.first == b; }),
                 va.end());
        vb.erase(remove_if(vb.begin(), vb.end(),
                           [&](auto &p)
                           { return p.first == a; }),
                 vb.end());
        saveConnections();
        cout << "Removed connection " << a << " <-> " << b << "\n";
    }

    // --- Display ---
    void listHospitals()
    {
        cout << "-- Hospitals --\n";
        for (auto &kv : nodes)
            cout << kv.first << " | " << kv.second->name << " | " << kv.second->location << "\n";
    }
    void displayNetwork()
    {
        cout << "-- Network --\n";
        for (auto &kv : adj)
        {
            cout << kv.first << " -> ";
            for (auto &e : kv.second)
                cout << e.first << "(" << e.second << "km) ";
            cout << "\n";
        }
    }

    // --- Per-hospital management ---
    void manageHospital()
    {
        cout << "Hospital ID: ";
        string hid;
        getline(cin, hid);
        if (!nodes.count(hid))
        {
            cout << "Not found.\n";
            return;
        }
        Hospital *h = nodes[hid];
        while (true)
        {
            cout << "\n-- Managing " << h->name << " (" << hid << ") --\n"
                 << "1. Register Patient\n"
                 << "2. Register Doctor\n"
                 << "3. Register Appointment\n"
                 << "4. Display Patients\n"
                 << "5. Display Doctors\n"
                 << "6. Display Appointments\n"
                 << "7. View Connections\n"
                 << "8. Add Connection\n"
                 << "9. Update Connection\n"
                 << "10.Delete Connection\n"
                 << "11.Go Back\n";
            int c = readInt("Choose: ", 1, 11);
            if (c == 11)
                break;
            switch (c)
            {
            case 1:
            {
                cout << "Name: ";
                string n;
                getline(cin, n);
                cout << "DOB: ";
                string d;
                getline(cin, d);
                cout << "Gender: ";
                string g;
                getline(cin, g);
                cout << "Added Patient " << h->registerPatient(n, d, g) << "\n";
                break;
            }
            case 2:
            {
                cout << "Name: ";
                string n;
                getline(cin, n);
                cout << "Spec: ";
                string s;
                getline(cin, s);
                cout << "Added Doctor " << h->registerDoctor(n, s) << "\n";
                break;
            }
            case 3:
            {
                int pid = readInt("Patient ID: ", 1), did = readInt("Doctor ID: ", 1);
                cout << "Date: ";
                string dt;
                getline(cin, dt);
                int id = h->registerAppointment(pid, did, dt);
                cout << (id < 0 ? "Invalid IDs" : "Added Appointment " + to_string(id)) << "\n";
                break;
            }
            case 4:
                h->displayPatients();
                break;
            case 5:
                h->displayDoctors();
                break;
            case 6:
                h->displayAppointments();
                break;
            case 7:
            {
                cout << "-- Connections from " << hid << " --\n";
                for (auto &e : adj[hid])
                    cout << e.first << "(" << e.second << "km)\n";
                break;
            }
            case 8:
            {
                cout << "Connect to ID: ";
                string other;
                getline(cin, other);
                if (!nodes.count(other))
                {
                    cout << "Invalid ID.\n";
                    break;
                }
                if (areConnected(hid, other))
                {
                    cout << "Hospitals are already connected.\n";
                    break;
                }
                int dist = readInt("Distance (km): ", 0);
                adj[hid].push_back({other, dist});
                adj[other].push_back({hid, dist});
                saveConnections();
                cout << "Connected.\n";
                break;
            }
            case 9:
            {
                cout << "Update connection to: ";
                string other;
                getline(cin, other);
                updateConnection(hid, other);
                break;
            }
            case 10:
            {
                cout << "Delete connection to: ";
                string other;
                getline(cin, other);
                deleteConnection(hid, other);
                break;
            }
            }
        }
    }

private:
    string genId()
    {
        return "H" + to_string(nextHospitalIndex++);
    }

    // -- hospitals.csv --
    void loadHospitals()
    {
        ifstream f("hospitals.csv");
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
            nodes[id] = new Hospital(id, nm, loc);
        }
        nextHospitalIndex = maxIdx + 1;
    }
    void saveHospitals()
    {
        ofstream f("hospitals.csv");
        f << "id,name,location\n";
        for (auto &kv : nodes)
            f << kv.first << ',' << kv.second->name << ',' << kv.second->location << "\n";
    }

    // -- connections.csv --
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

// ======== Main ========
int main()
{
    Graph graph;
    while (true)
    {
        cout << "\n=== Multi-Hospital Management ===\n"
             << "1. Add Hospital\n"
             << "2. Delete Hospital\n"
             << "3. Connect Hospitals\n"
             << "4. Update Hospital Info\n"
             << "5. List Hospitals\n"
             << "6. View Network\n"
             << "7. Manage Hospital\n"
             << "8. Exit\n";
        int choice = readInt("Choose: ", 1, 8);
        if (choice == 8)
            break;
        switch (choice)
        {
        case 1:
            graph.addHospital();
            break;
        case 2:
            graph.deleteHospital();
            break;
        case 3:
            graph.connectHospitals();
            break;
        case 4:
            graph.updateHospitalInfo();
            break;
        case 5:
            graph.listHospitals();
            break;
        case 6:
            graph.displayNetwork();
            break;
        case 7:
            graph.manageHospital();
            break;
        }
    }
    cout << "Goodbye!\n";
    return 0;
}
