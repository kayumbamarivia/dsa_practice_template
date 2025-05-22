/**
 * Magical Gem Sorting Adventure
 * 
 * Welcome to the Magical Gem Workshop where we learn different ways to sort our magical gems:
 * 1. Bubble Sort: Like bubbles rising to the top
 * 2. Selection Sort: Like picking the smallest gem each time
 * 3. Insertion Sort: Like inserting gems into their correct positions
 * 4. Merge Sort: Like dividing gems into groups and merging them back
 * 5. Quick Sort: Like picking a special gem and arranging others around it
 * 6. Heap Sort: Like building a magical gem pyramid
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// A magical gem with different properties
struct MagicalGem {
    string name;
    int power;
    string color;
    
    MagicalGem(string n, int p, string c) : name(n), power(p), color(c) {}
};

// Our magical gem workshop
class GemWorkshop {
private:
    vector<MagicalGem> gems;
    
    // Helper function to print gems
    void printGems(const string& message) {
        cout << "\n=== " << message << " ===\n";
        for (const auto& gem : gems) {
            cout << gem.name << " (Power: " << gem.power << ", Color: " << gem.color << ")\n";
        }
    }
    
    // Bubble Sort: Like bubbles rising to the top
    void bubbleSort() {
        cout << "\n=== Bubble Sort ===\n";
        cout << "Sorting gems like bubbles rising to the top...\n";
        
        for (int i = 0; i < gems.size() - 1; i++) {
            for (int j = 0; j < gems.size() - i - 1; j++) {
                if (gems[j].power > gems[j + 1].power) {
                    swap(gems[j], gems[j + 1]);
                    cout << "Swapped " << gems[j].name << " and " << gems[j + 1].name << "\n";
                }
            }
        }
    }
    
    // Selection Sort: Like picking the smallest gem each time
    void selectionSort() {
        cout << "\n=== Selection Sort ===\n";
        cout << "Picking the smallest gem each time...\n";
        
        for (int i = 0; i < gems.size() - 1; i++) {
            int minIndex = i;
            for (int j = i + 1; j < gems.size(); j++) {
                if (gems[j].power < gems[minIndex].power) {
                    minIndex = j;
                }
            }
            if (minIndex != i) {
                swap(gems[i], gems[minIndex]);
                cout << "Selected " << gems[i].name << " as smallest\n";
            }
        }
    }
    
    // Insertion Sort: Like inserting gems into their correct positions
    void insertionSort() {
        cout << "\n=== Insertion Sort ===\n";
        cout << "Inserting gems into their correct positions...\n";
        
        for (int i = 1; i < gems.size(); i++) {
            MagicalGem key = gems[i];
            int j = i - 1;
            
            while (j >= 0 && gems[j].power > key.power) {
                gems[j + 1] = gems[j];
                j--;
            }
            gems[j + 1] = key;
            cout << "Inserted " << key.name << " into position " << j + 1 << "\n";
        }
    }
    
    // Merge Sort helpers
    void merge(int left, int mid, int right) {
        vector<MagicalGem> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;
        
        while (i <= mid && j <= right) {
            if (gems[i].power <= gems[j].power) {
                temp[k++] = gems[i++];
            } else {
                temp[k++] = gems[j++];
            }
        }
        
        while (i <= mid) temp[k++] = gems[i++];
        while (j <= right) temp[k++] = gems[j++];
        
        for (i = 0; i < k; i++) {
            gems[left + i] = temp[i];
        }
    }
    
    void mergeSortHelper(int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortHelper(left, mid);
            mergeSortHelper(mid + 1, right);
            merge(left, mid, right);
        }
    }
    
    // Quick Sort helpers
    int partition(int low, int high) {
        int pivot = gems[high].power;
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            if (gems[j].power < pivot) {
                i++;
                swap(gems[i], gems[j]);
            }
        }
        swap(gems[i + 1], gems[high]);
        return i + 1;
    }
    
    void quickSortHelper(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSortHelper(low, pi - 1);
            quickSortHelper(pi + 1, high);
        }
    }
    
    // Heap Sort helpers
    void heapify(int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        if (left < n && gems[left].power > gems[largest].power)
            largest = left;
            
        if (right < n && gems[right].power > gems[largest].power)
            largest = right;
            
        if (largest != i) {
            swap(gems[i], gems[largest]);
            heapify(n, largest);
        }
    }

public:
    // Add a gem to our collection
    void addGem(const MagicalGem& gem) {
        gems.push_back(gem);
        cout << "Added " << gem.name << " to collection\n";
    }
    
    // Try all sorting methods
    void tryAllSortingMethods() {
        // Make a copy of original gems
        vector<MagicalGem> originalGems = gems;
        
        // Bubble Sort
        printGems("Before Bubble Sort");
        bubbleSort();
        printGems("After Bubble Sort");
        
        // Reset gems
        gems = originalGems;
        
        // Selection Sort
        printGems("Before Selection Sort");
        selectionSort();
        printGems("After Selection Sort");
        
        // Reset gems
        gems = originalGems;
        
        // Insertion Sort
        printGems("Before Insertion Sort");
        insertionSort();
        printGems("After Insertion Sort");
        
        // Reset gems
        gems = originalGems;
        
        // Merge Sort
        cout << "\n=== Merge Sort ===\n";
        cout << "Dividing and merging gems...\n";
        printGems("Before Merge Sort");
        mergeSortHelper(0, gems.size() - 1);
        printGems("After Merge Sort");
        
        // Reset gems
        gems = originalGems;
        
        // Quick Sort
        cout << "\n=== Quick Sort ===\n";
        cout << "Picking special gems and arranging others...\n";
        printGems("Before Quick Sort");
        quickSortHelper(0, gems.size() - 1);
        printGems("After Quick Sort");
        
        // Reset gems
        gems = originalGems;
        
        // Heap Sort
        cout << "\n=== Heap Sort ===\n";
        cout << "Building a magical gem pyramid...\n";
        printGems("Before Heap Sort");
        
        // Build heap
        for (int i = gems.size() / 2 - 1; i >= 0; i--)
            heapify(gems.size(), i);
            
        // Extract elements from heap
        for (int i = gems.size() - 1; i > 0; i--) {
            swap(gems[0], gems[i]);
            heapify(i, 0);
        }
        
        printGems("After Heap Sort");
    }
};

int main() {
    // Create our magical gem workshop
    GemWorkshop workshop;
    
    // Add some magical gems
    workshop.addGem(MagicalGem("Ruby", 100, "Red"));
    workshop.addGem(MagicalGem("Sapphire", 80, "Blue"));
    workshop.addGem(MagicalGem("Emerald", 90, "Green"));
    workshop.addGem(MagicalGem("Diamond", 120, "Clear"));
    workshop.addGem(MagicalGem("Amethyst", 70, "Purple"));
    
    cout << "Welcome to the Magical Gem Workshop!\n";
    cout << "Let's learn different ways to sort our magical gems!\n";
    
    // Try all sorting methods
    workshop.tryAllSortingMethods();
    
    return 0;
} 