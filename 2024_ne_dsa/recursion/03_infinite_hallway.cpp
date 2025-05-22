/**
 * Infinite Hallway - Recursion with Backtracking
 * 
 * This is like Ron's infinite hallway of mirrors, where each mirror leads to another.
 * We'll learn about recursion that keeps going until it finds what it's looking for,
 * using a maze solver as our example.
 * 
 * Imagine you're in a maze:
 * 1. You look in one direction
 * 2. If you can go that way, you take a step
 * 3. You keep going until you find the exit or hit a dead end
 * 4. If you hit a dead end, you go back and try another way
 */

#include <iostream>
#include <vector>
using namespace std;

// Our maze is represented by a 2D vector
// 0 means path, 1 means wall, 2 means exit
vector<vector<int>> maze = {
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 2, 1}
};

// This is our infinite hallway function!
// It keeps trying different paths until it finds the exit
bool solveMaze(int row, int col) {
    // First, let's check if we're still in the maze
    if (row < 0 || row >= maze.size() || col < 0 || col >= maze[0].size()) {
        cout << "Oops! We hit a wall!\n";
        return false;
    }
    
    // If we found the exit, we're done!
    if (maze[row][col] == 2) {
        cout << "Yay! We found the exit!\n";
        return true;
    }
    
    // If we hit a wall or a path we've already tried, go back
    if (maze[row][col] == 1 || maze[row][col] == 3) {
        cout << "This path is blocked!\n";
        return false;
    }
    
    // Mark this path as tried
    maze[row][col] = 3;
    
    // Try going in all four directions
    cout << "Trying path at position (" << row << "," << col << ")\n";
    
    // Try going right
    if (solveMaze(row, col + 1)) return true;
    
    // Try going down
    if (solveMaze(row + 1, col)) return true;
    
    // Try going left
    if (solveMaze(row, col - 1)) return true;
    
    // Try going up
    if (solveMaze(row - 1, col)) return true;
    
    // If we get here, this path didn't lead to the exit
    cout << "This path didn't work, let's try another way!\n";
    return false;
}

int main() {
    cout << "Welcome to Ron's Infinite Hallway Maze!\n";
    cout << "Let's try to find our way to the exit:\n\n";
    
    // Start at position (1,0)
    if (solveMaze(1, 0)) {
        cout << "\nWe found our way out of the maze!\n";
    } else {
        cout << "\nWe couldn't find a way out!\n";
    }
    
    return 0;
} 