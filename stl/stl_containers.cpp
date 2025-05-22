/**
 * Magical Library Adventure
 * 
 * Welcome to the Magical Library where we have different ways to store our magical books:
 * 1. Vector: Like a magical bookshelf that can grow and shrink
 * 2. List: Like a chain of magical books where each book knows its neighbors
 * 3. Map: Like a magical catalog where each book has a unique key
 * 4. Set: Like a special collection where each book is unique
 * 5. Queue: Like a line of people waiting to read books
 * 6. Stack: Like a pile of books where you can only take the top one
 */

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <string>
using namespace std;

// A magical book in our library
struct MagicalBook {
    string title;
    string author;
    int pages;
    
    MagicalBook(string t, string a, int p) : title(t), author(a), pages(p) {}
};

// Our magical library
class MagicalLibrary {
private:
    // Different ways to store our books
    vector<MagicalBook> bookshelf;      // Like a growing bookshelf
    list<MagicalBook> bookChain;        // Like a chain of books
    map<string, MagicalBook> catalog;   // Like a catalog with unique keys
    set<string> uniqueTitles;           // Like a list of unique book titles
    queue<MagicalBook> readingQueue;    // Like a line of people waiting to read
    stack<MagicalBook> bookPile;        // Like a pile of books
    
public:
    // Add a book to our bookshelf (vector)
    void addToBookshelf(const MagicalBook& book) {
        bookshelf.push_back(book);
        cout << "Added " << book.title << " to the bookshelf\n";
    }
    
    // Add a book to our chain (list)
    void addToChain(const MagicalBook& book) {
        bookChain.push_back(book);
        cout << "Added " << book.title << " to the chain\n";
    }
    
    // Add a book to our catalog (map)
    void addToCatalog(const MagicalBook& book) {
        catalog[book.title] = book;
        cout << "Added " << book.title << " to the catalog\n";
    }
    
    // Add a book title to our unique collection (set)
    void addToUniqueTitles(const string& title) {
        uniqueTitles.insert(title);
        cout << "Added " << title << " to unique titles\n";
    }
    
    // Add a book to our reading queue
    void addToReadingQueue(const MagicalBook& book) {
        readingQueue.push(book);
        cout << "Added " << book.title << " to reading queue\n";
    }
    
    // Add a book to our pile (stack)
    void addToPile(const MagicalBook& book) {
        bookPile.push(book);
        cout << "Added " << book.title << " to the pile\n";
    }
    
    // Display all books in our bookshelf
    void displayBookshelf() {
        cout << "\n=== Bookshelf Contents ===\n";
        for (const auto& book : bookshelf) {
            cout << "Title: " << book.title << ", Author: " << book.author << "\n";
        }
    }
    
    // Display all books in our chain
    void displayChain() {
        cout << "\n=== Chain Contents ===\n";
        for (const auto& book : bookChain) {
            cout << "Title: " << book.title << ", Author: " << book.author << "\n";
        }
    }
    
    // Display all books in our catalog
    void displayCatalog() {
        cout << "\n=== Catalog Contents ===\n";
        for (const auto& pair : catalog) {
            cout << "Title: " << pair.second.title << ", Author: " << pair.second.author << "\n";
        }
    }
    
    // Display all unique titles
    void displayUniqueTitles() {
        cout << "\n=== Unique Titles ===\n";
        for (const auto& title : uniqueTitles) {
            cout << title << "\n";
        }
    }
    
    // Process reading queue
    void processReadingQueue() {
        cout << "\n=== Processing Reading Queue ===\n";
        while (!readingQueue.empty()) {
            MagicalBook book = readingQueue.front();
            readingQueue.pop();
            cout << "Reading: " << book.title << "\n";
        }
    }
    
    // Process book pile
    void processBookPile() {
        cout << "\n=== Processing Book Pile ===\n";
        while (!bookPile.empty()) {
            MagicalBook book = bookPile.top();
            bookPile.pop();
            cout << "Taking book from pile: " << book.title << "\n";
        }
    }
};

int main() {
    // Create our magical library
    MagicalLibrary library;
    
    // Create some magical books
    MagicalBook book1("The Magic Tree", "Alice Wonder", 200);
    MagicalBook book2("Dragon's Tale", "Peter Pan", 150);
    MagicalBook book3("Fairy's Secret", "Tinker Bell", 180);
    
    cout << "Welcome to the Magical Library!\n";
    cout << "Let's organize our books in different ways!\n\n";
    
    // Add books to different containers
    library.addToBookshelf(book1);
    library.addToBookshelf(book2);
    library.addToBookshelf(book3);
    
    library.addToChain(book1);
    library.addToChain(book2);
    library.addToChain(book3);
    
    library.addToCatalog(book1);
    library.addToCatalog(book2);
    library.addToCatalog(book3);
    
    library.addToUniqueTitles(book1.title);
    library.addToUniqueTitles(book2.title);
    library.addToUniqueTitles(book3.title);
    
    library.addToReadingQueue(book1);
    library.addToReadingQueue(book2);
    library.addToReadingQueue(book3);
    
    library.addToPile(book1);
    library.addToPile(book2);
    library.addToPile(book3);
    
    // Display contents of different containers
    library.displayBookshelf();
    library.displayChain();
    library.displayCatalog();
    library.displayUniqueTitles();
    
    // Process queue and stack
    library.processReadingQueue();
    library.processBookPile();
    
    return 0;
} 