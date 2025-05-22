/**
 * Graph Traversal Adventure
 * 
 * Imagine you're exploring a magical maze! This program shows you two ways to explore:
 * 1. BFS (Breadth-First Search): Like exploring level by level, like checking all rooms on one floor before going upstairs
 * 2. DFS (Depth-First Search): Like following a path until you hit a dead end, then going back to try another path
 * 
 * Think of it like exploring a castle:
 * - BFS: Check all rooms on the first floor, then all rooms on the second floor, and so on
 * - DFS: Go into a room, then into a connected room, and keep going until you can't go further, then go back
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

// A class to represent our magical maze (graph)
class MagicalMaze {
private:
    // Number of rooms (vertices) in our maze
    int numRooms;
    
    // Our maze map (adjacency list)
    vector<vector<int>> mazeMap;
    
    // Helper function to print the path we found
    void printPath(const vector<int>& path) {
        cout << "Path found: ";
        for (int room : path) {
            cout << room << " -> ";
        }
        cout << "End\n";
    }

public:
    // Constructor to create our maze
    MagicalMaze(int rooms) : numRooms(rooms) {
        // Create empty rooms
        mazeMap.resize(rooms);
    }
    
    // Add a magical door (edge) between two rooms
    void addDoor(int from, int to) {
        mazeMap[from].push_back(to);
        mazeMap[to].push_back(from);  // Doors work both ways!
    }
    
    // BFS: Explore level by level
    void exploreLevelByLevel(int startRoom) {
        cout << "\n=== Exploring Level by Level (BFS) ===\n";
        cout << "Starting from room " << startRoom << "\n\n";
        
        // Keep track of rooms we've visited
        vector<bool> visited(numRooms, false);
        
        // Our magic queue to remember which rooms to check next
        queue<int> roomQueue;
        
        // Start with our first room
        visited[startRoom] = true;
        roomQueue.push(startRoom);
        
        cout << "Exploring rooms in this order:\n";
        
        // Keep exploring until we've checked all rooms
        while (!roomQueue.empty()) {
            // Get the next room to explore
            int currentRoom = roomQueue.front();
            roomQueue.pop();
            
            cout << "Checking room " << currentRoom << "\n";
            
            // Look at all connected rooms
            for (int nextRoom : mazeMap[currentRoom]) {
                if (!visited[nextRoom]) {
                    visited[nextRoom] = true;
                    roomQueue.push(nextRoom);
                    cout << "  Found new room: " << nextRoom << "\n";
                }
            }
        }
    }
    
    // DFS: Follow paths until we hit dead ends
    void explorePaths(int startRoom) {
        cout << "\n=== Following Paths (DFS) ===\n";
        cout << "Starting from room " << startRoom << "\n\n";
        
        // Keep track of rooms we've visited
        vector<bool> visited(numRooms, false);
        
        // Our magic stack to remember which paths to try
        stack<int> pathStack;
        
        // Start with our first room
        visited[startRoom] = true;
        pathStack.push(startRoom);
        
        cout << "Following paths in this order:\n";
        
        // Keep exploring until we've tried all paths
        while (!pathStack.empty()) {
            // Get the next room to explore
            int currentRoom = pathStack.top();
            pathStack.pop();
            
            cout << "Checking room " << currentRoom << "\n";
            
            // Look at all connected rooms
            for (int nextRoom : mazeMap[currentRoom]) {
                if (!visited[nextRoom]) {
                    visited[nextRoom] = true;
                    pathStack.push(nextRoom);
                    cout << "  Found new path to room: " << nextRoom << "\n";
                }
            }
        }
    }
};

int main() {
    // Create a magical maze with 6 rooms
    MagicalMaze maze(6);
    
    // Add magical doors between rooms
    maze.addDoor(0, 1);  // Room 0 connects to Room 1
    maze.addDoor(0, 2);  // Room 0 connects to Room 2
    maze.addDoor(1, 3);  // Room 1 connects to Room 3
    maze.addDoor(2, 3);  // Room 2 connects to Room 3
    maze.addDoor(2, 4);  // Room 2 connects to Room 4
    maze.addDoor(3, 5);  // Room 3 connects to Room 5
    maze.addDoor(4, 5);  // Room 4 connects to Room 5
    
    cout << "Welcome to the Magical Maze Explorer!\n";
    cout << "Our maze has 6 rooms connected by magical doors.\n";
    cout << "Let's explore it in two different ways!\n";
    
    // Try both exploration methods
    maze.exploreLevelByLevel(0);  // Start from Room 0
    maze.explorePaths(0);         // Start from Room 0
    
    return 0;
} 