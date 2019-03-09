#include "Trie.h"
#include <iostream>

using namespace std;


int main() {
    Trie<int> t;
    t.insert("", 2);
    t.insert("ayylmoa", 4);
    t.insert("ayylmaa", 7);
    cout << "Seems legit " << endl;
    t.find("ayylmuo", false);
    cout << "found" << endl;
    return 0;
}