#include <iostream>
#include "DoublyLinkedList.h"
using namespace std;

DoublyLinkedList::DoublyLinkedList()
{}

DoublyLinkedList::~DoublyLinkedList()
{
    Node *tmp;
    while(head != NULL)
    {
        tmp = head;
        head = head->get_next();
        delete tmp;
    }
}

void DoublyLinkedList::insert(Node *&node)
{
    node->set_next(head->get_next());
    if(head->get_next() != NULL)
        (head->get_next())->set_prev(node);
    head->set_next(node);
    node->set_prev(head);
}

void DoublyLinkedList::print_nodes()
{
    Node *x = head->get_next();
    while(x != NULL)
    {
        cout << x->x1 << " , " << x->x2 << " ";
        cout << x->y << endl;
        x = x->get_next();
    }
}

void DoublyLinkedList::clean()
{
    Node *tmp_ptr;
    Node *delete_ptr;
    while(delete_ptr != NULL)
    {
        tmp_ptr = head->get_next();
        delete_ptr = tmp_ptr->get_next();
        delete tmp_ptr;
    }
    Node *node = new Node();
    node->x1 = 0;
    node->x2 = 1e9;
    node->y = 0;
    insert(node);
}
