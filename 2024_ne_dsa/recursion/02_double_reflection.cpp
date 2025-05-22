/**
 * Double Reflection - Multiple Recursive Calls
 * 
 * This is like Ron's second mirror that shows two reflections at once.
 * We'll learn about functions that call themselves twice using the Fibonacci sequence.
 * 
 * The Fibonacci sequence is like counting rabbits:
 * Each month, each pair of rabbits has a new pair of baby rabbits.
 * The sequence goes: 1, 1, 2, 3, 5, 8, 13, 21, ...
 * Each number is the sum of the two numbers before it!
 */

#include <iostream>
using namespace std;

// This is our double-reflection mirror function!
// It calls itself twice, just like Ron's special mirror
int fibonacci(int n) {
    // These are our stopping points - like when the mirrors stop reflecting
    if (n <= 1) {
        cout << "We reached the end! Returning " << n << "\n";
        return n;
    }
    
    // This is where the double magic happens - the function calls itself twice!
    // It's like one mirror showing two other mirrors
    cout << "Calculating fibonacci(" << n << ") = fibonacci(" << n-1 
         << ") + fibonacci(" << n-2 << ")\n";
    return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    cout << "Welcome to Ron's Double Reflection Mirror!\n";
    cout << "Let's calculate some Fibonacci numbers:\n\n";
    
    // Try different numbers
    int numbers[] = {0, 1, 2, 3, 4, 5, 6};
    
    for(int num : numbers) {
        cout << "\n=== Calculating fibonacci(" << num << ") ===\n";
        int result = fibonacci(num);
        cout << "fibonacci(" << num << ") = " << result << "\n";
    }
    
    return 0;
} 