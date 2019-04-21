#ifndef Doublylinkedlist_HPP
#define Doublylinkedlist_HPP
#include <string>
using namespace std;

class Node{
public:
    Node() {};
    ~Node() {};
    int x1 = 0;
    int x2 = 0;
    int y = 0;
    void set_prev(Node *x) { prev = x; }
    void set_next(Node *x) { next = x; }
    Node *get_prev() { return this->prev; }
    Node *get_next() { return this->next; }
private:
    Node *prev = NULL;
    Node *next = NULL;
};

class DoublyLinkedList{
public:
    Node *head = new Node();
    DoublyLinkedList();
    ~DoublyLinkedList();
    void insert(Node *&node);
    void print_nodes();
    void clean();
private:
};

#endif
