#include <iostream>
using namespace std;

class Resource {
private:
    string name;
    
public:
    // Constructor
    Resource(string n) : name(n) {
        cout << "Resource " << name << " created\n";
    }
    
    // Destructor
    ~Resource() {
        cout << "Resource " << name << " destroyed\n";
    }
    
    void use() {
        cout << "Using resource " << name << "\n";
    }
};

// Function scope example
void functionScope() {
    cout << "\n=== Function Scope Example ===\n";
    Resource r("FunctionScope");
    r.use();
    cout << "End of function - destructor will be called here\n";
} // r's destructor is called here when function ends

int main() {
    cout << "=== Block Scope Example ===\n";
    {
        cout << "Entering block scope\n";
        Resource r1("BlockScope");
        r1.use();
        cout << "End of block - destructor will be called here\n";
    } // r1's destructor is called here when block ends
    
    cout << "\n=== Function Scope Example ===\n";
    functionScope(); // Function scope example
    
    cout << "\n=== Loop Scope Example ===\n";
    for(int i = 0; i < 2; i++) {
        cout << "Loop iteration " << i << "\n";
        Resource r2("LoopScope");
        r2.use();
        cout << "End of loop iteration - destructor will be called here\n";
    } // r2's destructor is called here at the end of each loop iteration
    
    cout << "\n=== If Statement Scope Example ===\n";
    if(true) {
        cout << "Entering if block\n";
        Resource r3("IfScope");
        r3.use();
        cout << "End of if block - destructor will be called here\n";
    } // r3's destructor is called here when if block ends
    
    return 0;
} // All objects created in main will be destroyed here 