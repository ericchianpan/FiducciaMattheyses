#ifndef BinaryTree_H 
#define BinaryTree_H
#include "DoublyLinkedList.h"
using namespace std;

class tree_node{
public:
    tree_node(string nodeKey)
    { set_key(nodeKey); }
    int x_coordinate = 0; 
    int y_coordinate = 0;
    bool child = false; /* bool is true when node is left child, otherwise false*/
    void set_key(string x) { key = x; }
    void set_parent(tree_node *x) { parent = x; }
    void set_left(tree_node *x) { left = x; }
    void set_right(tree_node *x) { right = x; }
    void set_next(tree_node *x) { next = x; }
    string get_key() { return this->key; }
    tree_node *get_parent() { return this->parent; }
    tree_node *get_left() { return this->left; }
    tree_node *get_right() { return this->right; }
    tree_node *get_next() { return this->next; }
private:
    string key;
    tree_node *parent = NULL;
    tree_node *left = NULL;
    tree_node *right = NULL;
    tree_node *next = NULL;
};

class BinaryTree{
public:
    BinaryTree();
    ~BinaryTree();
    tree_node *root = new tree_node("root"); 
    DoublyLinkedList doubly_linked_list;
    int max_x = 0;
    int max_y = 0;
    void print(tree_node *node);
    void clear(tree_node *node);
    tree_node *find_last_node(tree_node *node);
private:
    void free(tree_node *node);
};

#endif
