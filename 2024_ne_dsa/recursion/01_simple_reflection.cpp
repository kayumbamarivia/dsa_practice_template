/**
 * Simple Reflection - Basic Recursion
 * 
 * This is like Ron's first mirror that shows one simple reflection.
 * We'll learn about the most basic form of recursion using factorial calculation.
 * 
 * A factorial is like counting how many ways you can arrange things.
 * For example, 3! = 3 × 2 × 1 = 6
 * This means there are 6 different ways to arrange 3 things!
 */

#include <iostream>
using namespace std;

// This is our magical mirror function!
// It takes a number and multiplies it by all the numbers below it
int factorial(int n) {
    // This is our stopping point - like when Ron's mirror stops reflecting
    if (n == 0 || n == 1) {
        cout << "We reached the end! Returning 1\n";
        return 1;
    }
    
    // This is where the magic happens - the function calls itself!
    // It's like one mirror showing another mirror
    cout << "Calculating " << n << "! = " << n << " × " << (n-1) << "!\n";
    return n * factorial(n - 1);
}

int main() {
    cout << "Welcome to Ron's Simple Reflection Mirror!\n";
    cout << "Let's calculate some factorials:\n\n";
    
    // Try different numbers
    int numbers[] = {0, 1, 2, 3, 4, 5};
    
    for(int num : numbers) {
        cout << "\n=== Calculating " << num << "! ===\n";
        int result = factorial(num);
        cout << num << "! = " << result << "\n";
    }
    
    return 0;
} 