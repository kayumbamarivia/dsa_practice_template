/**
 * Tree of Mirrors - Recursive Tree Structures
 * 
 * This is like Ron's tree of mirrors, where each mirror creates two smaller mirrors,
 * which create two even smaller mirrors, and so on. We'll learn about recursive tree
 * structures using a binary search tree.
 * 
 * A binary search tree is like a magical tree where:
 * 1. Each node can have up to two children
 * 2. All numbers smaller than the current node go to the left
 * 3. All numbers larger than the current node go to the right
 */

#include <iostream>
using namespace std;

// This is our tree structure
struct TreeNode {
    int value;          // The number stored in this node
    TreeNode* left;     // The left child (smaller numbers)
    TreeNode* right;    // The right child (larger numbers)
    
    // Constructor to create a new node
    TreeNode(int v) : value(v), left(nullptr), right(nullptr) {}
};

// This is our tree of mirrors class!
class BinarySearchTree {
private:
    TreeNode* root;  // The root of our tree
    
    // Helper function to insert a number
    TreeNode* insert(TreeNode* node, int value) {
        // If we found an empty spot, create a new node
        if (node == nullptr) {
            cout << "Creating new node with value " << value << endl;
            return new TreeNode(value);
        }
        
        // If the number is smaller, go left
        if (value < node->value) {
            cout << value << " is smaller than " << node->value << ", going left\n";
            node->left = insert(node->left, value);
        }
        // If the number is larger, go right
        else if (value > node->value) {
            cout << value << " is larger than " << node->value << ", going right\n";
            node->right = insert(node->right, value);
        }
        
        return node;
    }
    
    // Helper function to print the tree
    void printTree(TreeNode* node, string prefix = "") {
        if (node == nullptr) {
            return;
        }
        
        // Print the right subtree
        printTree(node->right, prefix + "    ");
        
        // Print the current node
        cout << prefix << node->value << endl;
        
        // Print the left subtree
        printTree(node->left, prefix + "    ");
    }
    
    // Helper function to clean up the tree
    void cleanup(TreeNode* node) {
        if (node == nullptr) {
            return;
        }
        
        // Clean up the left subtree
        cleanup(node->left);
        
        // Clean up the right subtree
        cleanup(node->right);
        
        // Delete this node
        delete node;
    }
    
public:
    // Constructor
    BinarySearchTree() : root(nullptr) {}
    
    // Destructor
    ~BinarySearchTree() {
        cleanup(root);
    }
    
    // Insert a number into the tree
    void insert(int value) {
        root = insert(root, value);
    }
    
    // Print the tree
    void print() {
        cout << "\nOur tree looks like this (rotated 90 degrees):\n";
        printTree(root);
    }
};

int main() {
    cout << "Welcome to Ron's Tree of Mirrors!\n";
    cout << "Let's create a magical binary search tree:\n\n";
    
    // Create our tree
    BinarySearchTree tree;
    
    // Insert some numbers
    int numbers[] = {5, 3, 7, 1, 4, 6, 8};
    
    for(int num : numbers) {
        cout << "\n=== Inserting " << num << " ===\n";
        tree.insert(num);
    }
    
    // Print the tree
    tree.print();
    
    return 0;
} 