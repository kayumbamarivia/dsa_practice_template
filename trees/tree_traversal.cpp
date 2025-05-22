/**
 * Magical Tree Traversal Adventure
 * 
 * Imagine a magical tree where each branch holds a special treasure!
 * This program shows you three ways to collect all the treasures:
 * 1. Preorder: Collect treasure as soon as you see it, then explore branches
 * 2. Inorder: Explore left branch, collect treasure, then explore right branch
 * 3. Postorder: Explore all branches first, then collect treasure
 * 
 * Think of it like collecting apples from a tree:
 * - Preorder: Grab apple as soon as you see it
 * - Inorder: Check left side, grab apple, check right side
 * - Postorder: Check all sides first, then grab apple
 */

#include <iostream>
#include <string>
using namespace std;

// A magical treasure box (node) in our tree
struct TreasureBox {
    string treasure;  // The magical treasure inside
    TreasureBox* left;   // Path to left treasure
    TreasureBox* right;  // Path to right treasure
    
    // Constructor to create a new treasure box
    TreasureBox(string t) : treasure(t), left(nullptr), right(nullptr) {}
};

// Our magical treasure tree
class MagicalTree {
private:
    TreasureBox* root;  // The top treasure box
    
    // Helper function to add a new treasure box
    TreasureBox* addTreasure(TreasureBox* current, string treasure) {
        if (current == nullptr) {
            return new TreasureBox(treasure);
        }
        
        // If treasure is smaller, go left
        if (treasure < current->treasure) {
            current->left = addTreasure(current->left, treasure);
        }
        // If treasure is bigger, go right
        else {
            current->right = addTreasure(current->right, treasure);
        }
        
        return current;
    }
    
    // Preorder: Collect treasure first, then explore
    void collectPreorder(TreasureBox* box) {
        if (box == nullptr) return;
        
        cout << "Found treasure: " << box->treasure << "\n";
        collectPreorder(box->left);
        collectPreorder(box->right);
    }
    
    // Inorder: Explore left, collect treasure, explore right
    void collectInorder(TreasureBox* box) {
        if (box == nullptr) return;
        
        collectInorder(box->left);
        cout << "Found treasure: " << box->treasure << "\n";
        collectInorder(box->right);
    }
    
    // Postorder: Explore all branches first, then collect treasure
    void collectPostorder(TreasureBox* box) {
        if (box == nullptr) return;
        
        collectPostorder(box->left);
        collectPostorder(box->right);
        cout << "Found treasure: " << box->treasure << "\n";
    }

public:
    // Constructor
    MagicalTree() : root(nullptr) {}
    
    // Add a new treasure to the tree
    void addTreasure(string treasure) {
        root = addTreasure(root, treasure);
    }
    
    // Collect treasures in preorder
    void collectTreasuresPreorder() {
        cout << "\n=== Collecting Treasures (Preorder) ===\n";
        cout << "Collecting as soon as we see each treasure!\n\n";
        collectPreorder(root);
    }
    
    // Collect treasures in inorder
    void collectTreasuresInorder() {
        cout << "\n=== Collecting Treasures (Inorder) ===\n";
        cout << "Checking left side first, then collecting, then right side!\n\n";
        collectInorder(root);
    }
    
    // Collect treasures in postorder
    void collectTreasuresPostorder() {
        cout << "\n=== Collecting Treasures (Postorder) ===\n";
        cout << "Checking all sides first, then collecting!\n\n";
        collectPostorder(root);
    }
};

int main() {
    // Create our magical treasure tree
    MagicalTree tree;
    
    // Add some magical treasures
    tree.addTreasure("Diamond");
    tree.addTreasure("Ruby");
    tree.addTreasure("Sapphire");
    tree.addTreasure("Emerald");
    tree.addTreasure("Gold");
    tree.addTreasure("Silver");
    tree.addTreasure("Pearl");
    
    cout << "Welcome to the Magical Treasure Tree!\n";
    cout << "Our tree has many magical treasures to collect.\n";
    cout << "Let's collect them in three different ways!\n";
    
    // Try all three collection methods
    tree.collectTreasuresPreorder();
    tree.collectTreasuresInorder();
    tree.collectTreasuresPostorder();
    
    return 0;
} 