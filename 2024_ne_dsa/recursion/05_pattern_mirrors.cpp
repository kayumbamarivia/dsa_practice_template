/**
 * Pattern Mirrors - Recursive Pattern Generation
 * 
 * This is like Ron's pattern mirrors that can create beautiful patterns
 * by having each mirror create part of the pattern and pass it to another mirror.
 * We'll learn about recursive pattern generation using a fractal tree.
 * 
 * A fractal tree is like a magical tree where each branch splits into two smaller branches,
 * and those branches split into even smaller branches, and so on!
 */

#include <iostream>
#include <string>
using namespace std;

// This is our pattern mirror function!
// It creates a beautiful fractal tree pattern
void drawFractalTree(int height, string prefix = "") {
    // If the tree is too small, stop drawing
    if (height <= 0) {
        return;
    }
    
    // Draw the trunk
    cout << prefix << "|" << endl;
    
    // Draw the branches
    cout << prefix << "|--" << endl;
    
    // Draw the left branch
    drawFractalTree(height - 1, prefix + "|  ");
    
    // Draw the right branch
    drawFractalTree(height - 1, prefix + "   ");
}

// This is another pattern mirror function!
// It creates a Sierpinski triangle pattern
void drawSierpinskiTriangle(int height, string prefix = "") {
    // If the triangle is too small, stop drawing
    if (height <= 0) {
        return;
    }
    
    // Draw the top of the triangle
    cout << prefix << "   *   " << endl;
    cout << prefix << "  * *  " << endl;
    cout << prefix << " *   * " << endl;
    cout << prefix << "*******" << endl;
    
    // Draw the left triangle
    drawSierpinskiTriangle(height - 1, prefix + "   ");
    
    // Draw the right triangle
    drawSierpinskiTriangle(height - 1, prefix + "    ");
}

int main() {
    cout << "Welcome to Ron's Pattern Mirrors!\n";
    cout << "Let's create some beautiful patterns:\n\n";
    
    cout << "=== Fractal Tree Pattern ===\n";
    drawFractalTree(3);
    
    cout << "\n=== Sierpinski Triangle Pattern ===\n";
    drawSierpinskiTriangle(2);
    
    return 0;
} 