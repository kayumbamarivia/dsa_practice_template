/**
 * Ruhengeri Referal Hospital Healthcare System
 * 
 * This program manages patient appointments using three linked lists:
 * - PatientsLL: Stores patient information
 * - DoctorsLL: Stores doctor information
 * - AppointmentsLL: Stores appointment information
 * 
 * The system ensures:
 * - Unique IDs for patients, doctors, and appointments
 * - Valid appointments (patient and doctor must exist)
 * - Menu-driven interface for all operations
 * 
 * Required libraries:
 * - iostream: for input/output operations
 * - string: for string operations
 * - cstdlib: for system operations
 */

#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

// Node structure for Patient Linked List
struct PatientNode {
    int patient_id;
    string name;
    string dob;
    string gender;
    PatientNode* next;
    
    // Constructor for easy node creation
    PatientNode(int id, string n, string d, string g) : 
        patient_id(id), name(n), dob(d), gender(g), next(nullptr) {}
};

// Node structure for Doctor Linked List
struct DoctorNode {
    int doctor_id;
    string name;
    string specialization;
    DoctorNode* next;
    
    // Constructor for easy node creation
    DoctorNode(int id, string n, string s) : 
        doctor_id(id), name(n), specialization(s), next(nullptr) {}
};

// Node structure for Appointment Linked List
struct AppointmentNode {
    int appointment_id;
    int patient_id;
    int doctor_id;
    string appointment_date;
    AppointmentNode* next;
    
    // Constructor for easy node creation
    AppointmentNode(int id, int pid, int did, string date) : 
        appointment_id(id), patient_id(pid), doctor_id(did), 
        appointment_date(date), next(nullptr) {}
};

// Class to manage the healthcare system
class HealthcareSystem {
private:
    PatientNode* patientsHead;
    DoctorNode* doctorsHead;
    AppointmentNode* appointmentsHead;
    
    // Helper function to check if patient exists
    bool patientExists(int id) {
        PatientNode* current = patientsHead;
        while (current != nullptr) {
            if (current->patient_id == id) return true;
            current = current->next;
        }
        return false;
    }
    
    // Helper function to check if doctor exists
    bool doctorExists(int id) {
        DoctorNode* current = doctorsHead;
        while (current != nullptr) {
            if (current->doctor_id == id) return true;
            current = current->next;
        }
        return false;
    }
    
    // Helper function to check if appointment exists
    bool appointmentExists(int id) {
        AppointmentNode* current = appointmentsHead;
        while (current != nullptr) {
            if (current->appointment_id == id) return true;
            current = current->next;
        }
        return false;
    }
    
    // Helper function to clear screen
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

public:
    // Constructor
    HealthcareSystem() : patientsHead(nullptr), doctorsHead(nullptr), appointmentsHead(nullptr) {}
    
    // Register a new patient
    void registerPatient() {
        clearScreen();
        cout << "\n=== Register New Patient ===\n";
        
        int id;
        string name, dob, gender;
        
        cout << "Enter Patient ID: ";
        cin >> id;
        
        if (patientExists(id)) {
            cout << "Error: Patient ID already exists!\n";
            return;
        }
        
        cin.ignore(); // Clear input buffer
        cout << "Enter Patient Name: ";
        getline(cin, name);
        
        cout << "Enter Date of Birth (DD/MM/YYYY): ";
        getline(cin, dob);
        
        cout << "Enter Gender (M/F): ";
        getline(cin, gender);
        
        // Create new patient node
        PatientNode* newPatient = new PatientNode(id, name, dob, gender);
        
        // Add to linked list
        if (patientsHead == nullptr) {
            patientsHead = newPatient;
        } else {
            PatientNode* current = patientsHead;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newPatient;
        }
        
        cout << "Patient registered successfully!\n";
    }
    
    // Register a new doctor
    void registerDoctor() {
        clearScreen();
        cout << "\n=== Register New Doctor ===\n";
        
        int id;
        string name, specialization;
        
        cout << "Enter Doctor ID: ";
        cin >> id;
        
        if (doctorExists(id)) {
            cout << "Error: Doctor ID already exists!\n";
            return;
        }
        
        cin.ignore(); // Clear input buffer
        cout << "Enter Doctor Name: ";
        getline(cin, name);
        
        cout << "Enter Specialization: ";
        getline(cin, specialization);
        
        // Create new doctor node
        DoctorNode* newDoctor = new DoctorNode(id, name, specialization);
        
        // Add to linked list
        if (doctorsHead == nullptr) {
            doctorsHead = newDoctor;
        } else {
            DoctorNode* current = doctorsHead;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newDoctor;
        }
        
        cout << "Doctor registered successfully!\n";
    }
    
    // Register a new appointment
    void registerAppointment() {
        clearScreen();
        cout << "\n=== Register New Appointment ===\n";
        
        int id, patient_id, doctor_id;
        string date;
        
        cout << "Enter Appointment ID: ";
        cin >> id;
        
        if (appointmentExists(id)) {
            cout << "Error: Appointment ID already exists!\n";
            return;
        }
        
        cout << "Enter Patient ID: ";
        cin >> patient_id;
        
        if (!patientExists(patient_id)) {
            cout << "Error: Patient ID does not exist!\n";
            return;
        }
        
        cout << "Enter Doctor ID: ";
        cin >> doctor_id;
        
        if (!doctorExists(doctor_id)) {
            cout << "Error: Doctor ID does not exist!\n";
            return;
        }
        
        cin.ignore(); // Clear input buffer
        cout << "Enter Appointment Date (DD/MM/YYYY): ";
        getline(cin, date);
        
        // Create new appointment node
        AppointmentNode* newAppointment = new AppointmentNode(id, patient_id, doctor_id, date);
        
        // Add to linked list
        if (appointmentsHead == nullptr) {
            appointmentsHead = newAppointment;
        } else {
            AppointmentNode* current = appointmentsHead;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newAppointment;
        }
        
        cout << "Appointment registered successfully!\n";
    }
    
    // Display all patients
    void displayPatients() {
        clearScreen();
        cout << "\n=== Patient List ===\n";
        
        if (patientsHead == nullptr) {
            cout << "No patients registered.\n";
            return;
        }
        
        cout << "ID\tName\t\tDOB\t\tGender\n";
        cout << "------------------------------------------------\n";
        
        PatientNode* current = patientsHead;
        while (current != nullptr) {
            cout << current->patient_id << "\t"
                 << current->name << "\t\t"
                 << current->dob << "\t"
                 << current->gender << "\n";
            current = current->next;
        }
    }
    
    // Display all doctors
    void displayDoctors() {
        clearScreen();
        cout << "\n=== Doctor List ===\n";
        
        if (doctorsHead == nullptr) {
            cout << "No doctors registered.\n";
            return;
        }
        
        cout << "ID\tName\t\tSpecialization\n";
        cout << "----------------------------------------\n";
        
        DoctorNode* current = doctorsHead;
        while (current != nullptr) {
            cout << current->doctor_id << "\t"
                 << current->name << "\t\t"
                 << current->specialization << "\n";
            current = current->next;
        }
    }
    
    // Display all appointments
    void displayAppointments() {
        clearScreen();
        cout << "\n=== Appointment List ===\n";
        
        if (appointmentsHead == nullptr) {
            cout << "No appointments registered.\n";
            return;
        }
        
        cout << "ID\tPatient ID\tDoctor ID\tDate\n";
        cout << "------------------------------------------------\n";
        
        AppointmentNode* current = appointmentsHead;
        while (current != nullptr) {
            cout << current->appointment_id << "\t"
                 << current->patient_id << "\t\t"
                 << current->doctor_id << "\t\t"
                 << current->appointment_date << "\n";
            current = current->next;
        }
    }
    
    // Display menu and handle user choice
    void showMenu() {
        int choice;
        
        do {
            cout << "\n=== Ruhengeri Referal Hospital ===\n";
            cout << "1. Register Patient\n";
            cout << "2. Register Doctor\n";
            cout << "3. Register Appointment\n";
            cout << "4. Display Patients\n";
            cout << "5. Display Doctors\n";
            cout << "6. Display Appointments\n";
            cout << "7. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    registerPatient();
                    break;
                case 2:
                    registerDoctor();
                    break;
                case 3:
                    registerAppointment();
                    break;
                case 4:
                    displayPatients();
                    break;
                case 5:
                    displayDoctors();
                    break;
                case 6:
                    displayAppointments();
                    break;
                case 7:
                    cout << "Thank you for using the system!\n";
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
            
            if (choice != 7) {
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
            }
            
        } while (choice != 7);
    }
    
    // Destructor to free memory
    ~HealthcareSystem() {
        // Free patient list
        while (patientsHead != nullptr) {
            PatientNode* temp = patientsHead;
            patientsHead = patientsHead->next;
            delete temp;
        }
        
        // Free doctor list
        while (doctorsHead != nullptr) {
            DoctorNode* temp = doctorsHead;
            doctorsHead = doctorsHead->next;
            delete temp;
        }
        
        // Free appointment list
        while (appointmentsHead != nullptr) {
            AppointmentNode* temp = appointmentsHead;
            appointmentsHead = appointmentsHead->next;
            delete temp;
        }
    }
};

int main() {
    HealthcareSystem system;
    system.showMenu();
    return 0;
} 