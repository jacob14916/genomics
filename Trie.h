#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <list>
#include <iostream>

using namespace std;

template<typename ValueType>
class Trie
{
public:
    Trie() {
        Node* rt = new Node;
        root = rt;
    }

    ~Trie() {
        deleteNodeAndAllChildren(root);
    }

    void reset();


    void insert(const string& key, const ValueType& value);
    vector<ValueType> find(const string& key, bool exactMatchOnly) const;

      // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

private:
    struct NodePointerWithLabel;

    struct Node {
        Node() {
            children = new list<NodePointerWithLabel>;
            values = new vector<ValueType>;
        }
        ~Node() {
            delete children;
            delete values;
        }
        list<NodePointerWithLabel>* children;
        vector<ValueType>* values;
    };

    struct NodePointerWithLabel {
        NodePointerWithLabel(Node* n, char l): node(n), label(l) {}
        Node* node;
        char label;
    };

    Node* root;

    void deleteNodeAndAllChildren(Node* n);

    vector<ValueType> findStartingFrom(Node* n, const string& key, int depth, bool exactMatchOnly) const;
};


template<typename ValueType>
void Trie<ValueType>::deleteNodeAndAllChildren(Node* n) {
    for (typename list<NodePointerWithLabel>::iterator child = n->children->begin();
            child != n->children->end(); child++) {
                deleteNodeAndAllChildren((*child).node);
            }
    delete n;
}

template<typename ValueType>
void Trie<ValueType>::insert(const string& key, const ValueType& value) {
    Node* current = root;
    for (int i = 0; i < key.length(); i++) {
        char c = key[i];
        for (typename list<NodePointerWithLabel>::iterator child = current->children->begin();
            true; // end condition handled inside the loop
            child++)
        {
            if (child == current->children->end() || (*child).label > c)
            {
                Node *newnode = new Node;
                current->children->insert(child, NodePointerWithLabel(newnode, c));
                current = newnode;
                break;
            }
            else if ((*child).label == c)
            {
                current = (*child).node;
                break;
            }
        }
    }
    current->values->push_back(value);
}

template<typename ValueType>
vector<ValueType> Trie<ValueType>::find(const string& key, bool exactMatchOnly) const
{
    vector<ValueType> vals;
    if (key.length() > 0) {
        char c = key[0];
        for (typename list<NodePointerWithLabel>::iterator child = root->children->begin();
            !(child == root->children->end() || (*child).label > c);
            child++)
        {
            if ((*child).label == c)
            {
                vals = findStartingFrom((*child).node, key, 1, exactMatchOnly);
                break;
            }
        }
    }
    return vals;
}


template<typename ValueType>
vector<ValueType> Trie<ValueType>::findStartingFrom(Node* n, const string& key, int depth, bool exactMatchOnly) const
{
    cout << depth << endl;
    if (depth == key.length()) {
        return *(n->values);
    }
    char c = key[depth];
    vector<ValueType> vals;
    for(typename list<NodePointerWithLabel>::iterator child = n->children->begin();
        !(child == n->children->end() || (exactMatchOnly && (*child).label > c));
        child++)
    {
        cout << (*child).label << endl;
        bool childEMO = true;
        if ((*child).label == c)
        {
            childEMO = exactMatchOnly;
        } else if (exactMatchOnly) {
            continue;
        }
        vector<ValueType> v = findStartingFrom((*child).node, key, depth+1, childEMO);
        vals.insert(vals.end(), v.begin(), v.end());
    }
    return vals;   
}




#endif // TRIE_INCLUDED
