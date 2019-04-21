#include <iostream>
#include "BinaryTree.h"
#include "DoublyLinkedList.h"
using namespace std;

BinaryTree::BinaryTree()
{
    Node *node = new Node();
    node->x1 = 0;
    node->x2 = 1e9;
    node->y = 0;
    doubly_linked_list.insert(node);
}

BinaryTree::~BinaryTree()
{
    free(root);
}

void BinaryTree::print(tree_node *node)
{
    if(node == NULL) return;
    // if(node->get_parent()!=NULL)
        // cout << node->get_parent()->get_key() << " -> ";
    cout << node->get_key() << " ";
    print(node->get_left());
    print(node->get_right());
}

void BinaryTree::free(tree_node *node)
{
    if(node == NULL) return;
    free(node->get_left());
    free(node->get_right());
    delete node;
}

void BinaryTree::clear(tree_node *node)
{
    if(node == NULL) return;
    clear(node->get_left());
    clear(node->get_right());
    delete node;
}

tree_node *BinaryTree::find_last_node(tree_node *node)
{
    if(node == NULL) return NULL;
    if(node->get_left() == NULL && node->get_right() == NULL) return node;
    else
    {
        tree_node *right_return = find_last_node(node->get_right());
        if(right_return == NULL)
            return find_last_node(node->get_left());
        else
            return right_return;
    }
}
