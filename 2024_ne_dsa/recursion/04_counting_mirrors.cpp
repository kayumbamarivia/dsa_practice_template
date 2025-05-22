/**
 * Counting Mirrors - Recursive Counting
 * 
 * This is like Ron's counting mirrors that can count things by passing the count
 * from one mirror to another. We'll learn about recursive counting using a binary tree.
 * 
 * A binary tree is like a family tree where each person can have up to two children.
 * We'll count how many people are in the tree by counting each person and their children.
 */

#include <iostream>
using namespace std;

// This is our tree structure - like a family tree
struct TreeNode {
    int value;          // The person's name (represented as a number)
    TreeNode* left;     // Their first child
    TreeNode* right;    // Their second child
    
    // Constructor to create a new person in the tree
    TreeNode(int v) : value(v), left(nullptr), right(nullptr) {}
};

// This is our counting mirror function!
// It counts all the people in the tree
int countNodes(TreeNode* node) {
    // If there's no one here, return 0
    if (node == nullptr) {
        cout << "No one here!\n";
        return 0;
    }
    
    // Count this person
    cout << "Counting person " << node->value << "\n";
    
    // Count their children and add them up
    int leftCount = countNodes(node->left);
    int rightCount = countNodes(node->right);
    
    // Return 1 (this person) plus their children
    return 1 + leftCount + rightCount;
}

int main() {
    cout << "Welcome to Ron's Counting Mirrors!\n";
    cout << "Let's count the people in our family tree:\n\n";
    
    // Create a sample family tree
    //       1
    //      / \
    //     2   3
    //    / \
    //   4   5
    
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    
    // Count all the people in the tree
    int total = countNodes(root);
    cout << "\nThere are " << total << " people in our family tree!\n";
    
    // Clean up our tree
    delete root->left->right;
    delete root->left->left;
    delete root->right;
    delete root->left;
    delete root;
    
    return 0;
} 