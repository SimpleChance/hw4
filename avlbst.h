#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
        Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);
    // Add helper functions here

    void insert_fix(AVLNode<Key,Value>* p, AVLNode<Key, Value>* n);
    void rotate_left(AVLNode<Key,Value>* p);
    void rotate_right(AVLNode<Key,Value>* p);
    void removeFix(AVLNode<Key, Value>* n, int diff);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    if(this->BinarySearchTree<Key,Value>::root_ == NULL)
    {
        AVLNode<Key, Value>* holder = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        holder->setBalance(0);
        AVLTree<Key, Value>::root_ = holder;
        return;
    }

    AVLNode<Key, Value>* temp = (AVLNode<Key, Value>*)AVLTree<Key, Value>::root_;
    while(true)
    {
        if(new_item.first == temp->getKey())
        {
            temp->setValue(new_item.second);
            return;
        }
        else if(new_item.first < temp->getKey())
        {
            if(temp->getLeft() == NULL)
            {
                AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, temp);
                temp->setLeft(newNode);
                temp = temp->getLeft();
                break;
            }
            else
            {
                temp = temp->getLeft();
            }
        }
        else
        {
            if(temp->getRight() == NULL)
            {
                AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, temp);
                temp->setRight(newNode);
                temp = temp->getRight();
                break;
            }
            else
            {
                temp = temp->getRight();
            }
        }
    }

    temp->setBalance(0);
    if(temp->getParent()->getBalance() == -1 || temp->getParent()->getBalance() == 1)
    {
        temp->getParent()->setBalance(0);
    }
    else
    {
        if(temp->getParent()->getLeft() == temp)
        {
            temp->getParent()->setBalance(-1);
        }
        else
        {
            temp->getParent()->setBalance(1);
        }
        insert_fix(temp->getParent(),temp);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* g = p->getParent();

    if(p == NULL || g == NULL)
    {
        return;
    }
    if(g->getLeft() == p)
    {
        g->updateBalance(-1);
        if(g->getBalance() == 0)
            return;
        else if (g->getBalance() == -1)
            insert_fix(g,p);
        else    //b(g) == -2
        {
            if(p->getLeft() == n)
            {
                rotate_right(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else
            {
                rotate_left(p);
                rotate_right(g);
                if(n->getBalance() == -1)
                {
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                }
                else if(n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else
                {
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
    else
    {
        g->updateBalance(1);
        if(g->getBalance() == 0)
            return;
        else if (g->getBalance() == 1)
            insert_fix(g,p);
        else
        {
            if(p->getRight() == n)
            {
                rotate_left(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else
            {
                rotate_right(p);
                rotate_left(g);
                if(n->getBalance() == 1)
                {
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                }
                else if(n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else
                {
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
    return;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotate_right(AVLNode<Key, Value>* p)
{
    if(p->getParent() == NULL)
    {
        this->root_ = p->getLeft();
    }
    else
    {
        if(p->getParent()->getLeft() == p)
        {
            p->getParent()->setLeft(p->getLeft());
        }
        else
        {
            p->getParent()->setRight(p->getLeft());
        }
        p->getLeft()->setParent(p->getParent());
    }

    AVLNode<Key, Value>* temp = p->getLeft();

    p->setLeft(p->getLeft()->getRight());
    if(p->getLeft()!= NULL)
    {
        p->getLeft()->setParent(p);
    }

    temp->setRight(p);
    temp->setParent(p->getParent());
    p->setParent(temp);
    return;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotate_left(AVLNode<Key, Value>* p)
{
    if(p->getParent() == NULL)
    {
        BinarySearchTree<Key, Value>::root_ = p->getRight();
    }
    else
    {
        if(p->getParent()->getLeft() == p)
        {
            p->getParent()->setLeft(p->getRight());
        }
        else
        {
            p->getParent()->setRight(p->getRight());
        }
        p->getRight()->setParent(p->getParent());
    }

    AVLNode<Key, Value>* temp = p->getRight();

    p->setRight(p->getRight()->getLeft());
    if(p->getRight()!= NULL)
    {
        p->getRight()->setParent(p);
    }

    temp->setLeft(p);
    temp->setParent(p->getParent());
    p->setParent(temp);
    return;
}
/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* n = (AVLNode<Key,Value>*)AVLTree<Key, Value>::internalFind(key);
    if(n == nullptr)
    {
        return;
    }
    if(n->getLeft() != NULL && n->getRight() != NULL)
    {
        AVLNode<Key, Value>* pred = (AVLNode<Key, Value>*)BinarySearchTree<Key, Value>::predecessor(n);
        nodeSwap(n,pred);
    }

    AVLNode<Key, Value>* p = n->getParent();
    int diff;
    if(p != NULL)
    {
        if(p->getLeft() == n)
        {
            diff = 1;
        }
        else
        {
            diff = -1;
        }
    }

    if(n->getLeft() != NULL)
    {
        BinarySearchTree<Key, Value>::removeWithLeftChild(n);
    }
    else if(n->getRight() != NULL)
    {
        BinarySearchTree<Key, Value>::removeWithRightChild(n);
    }
    else
    {
        BinarySearchTree<Key, Value>::removeLeafNode(n);
    }

    removeFix(p, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff)
{
    if(n == NULL)
    {
        return;
    }
    int ndiff = 0;
    AVLNode<Key, Value>* p = n->getParent();
    if(p != NULL)
    {
        if(p->getLeft() == n)
        {
            ndiff = 1;
        }
        else
        {
            ndiff = -1;
        }
    }
    if(diff == -1) {
        if (n->getBalance() + diff == -2)
        {
            AVLNode<Key, Value>* c = n->getLeft();
            if(c->getBalance() == -1)
            {
                rotate_right(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p,ndiff);
            }
            else if (c->getBalance() == 0)
            {
                rotate_right(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else
            {
                AVLNode<Key, Value>* g = c->getRight();
                rotate_left(c);
                rotate_right(n);
                if(g->getBalance() == 1)
                {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else
                {
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        if(n->getBalance() + diff == -1)
        {
            n->setBalance(-1);
        }
        else
        {
            n->setBalance(0);
            removeFix(p,ndiff);
        }
    }
    else
    {
        if(n->getBalance() + diff == 2)
        {
            AVLNode<Key, Value>* c = n->getRight();
            if(c->getBalance() == 1)
            {
                rotate_left(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0)
            {
                rotate_left(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else
            {
                AVLNode<Key, Value>* g = c->getLeft();
                rotate_right(c);
                rotate_left(n);
                if(g->getBalance() == -1)
                {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else
                {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == 1)
        {
            n->setBalance(1);
        }
        else
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    return;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif