#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "AVLExceptions.h"

// template <typename T>
// class AVLTree;

int max(int a, int b) { return a > b ? a : b; }
template <typename T>
class AVLNode
{
    int key;
    T data;
    AVLNode *right;
    AVLNode *left;
    AVLNode *parent;
    int height;
    AVLNode();
    AVLNode(int key, const T &data) : key(key), data(data), right(NULL), left(NULL), parent(NULL), height(0) {}
    ~AVLNode() = default;
    int GetKey() const { return (this) ? key : -1; }
    T GetData() const { return data; }
    void SetLeft(AVLNode *new_left) { left = new_left; }
    AVLNode *GetLeft() const { return left; }
    void SetRight(AVLNode *new_right) { right = new_right; }
    AVLNode *GetRight() const { return right; }
    void SetParent(AVLNode *new_parent) { parent = new_parent; }
    AVLNode *GetParent() const { return parent; }
    int GetHeight() const { return (this) ? height : -1; }
    int BalanceFactor() const { return this->GetLeft()->GetHeight() - this->GetRight()->GetHeight(); }
    void updateHeight() { this->height = 1 + max(this->GetLeft()->GetHeight(), this->GetRight()->GetHeight()); }

    friend class AVLTree<T>;
};

template <typename T>
class AVLTree
{

    AVLNode<T> *root;
    AVLNode<T> *getInorderSuccessor(AVLNode<T> *current)
    {
        current = current->GetRight();
        /* loop down to find the leftmost leaf */
        while (current->left != NULL)
            current = current->left;
        return current;
    }
    AVLNode<T> *GetGreatestNode(AVLNode<T> *current)
    {
        while (current->GetRight())
            current = current->GetRight();
        return current;
    }
    void RotateLeft(AVLNode<T> *root)
    {
        //root=A
        AVLNode<T> *newroot = root->GetRight(); //newroot = B
        root->SetRight(newroot->GetLeft());     //right(A)=left(B)
        newroot->SetLeft(root);                 //left(B)=A

        if (root->GetParent() == NULL)
        {
            this->root = newroot; //B is the new root of the tree
            newroot->SetParent(NULL);
        }
        else
        {
            if (root->GetParent()->GetLeft() == root)
            {
                root->GetParent()->SetLeft(newroot);
            }
            else
            {
                root->GetParent()->SetRight(newroot);
            }
            newroot->SetParent(root->GetParent());
        }
        root->SetParent(newroot);
        root->updateHeight();
        newroot->updateHeight();
    }
    void RotateRight(AVLNode<T> *root)
    {
        AVLNode<T> *newroot = root->GetLeft();
        root->SetLeft(newroot->GetRight());
        newroot->SetRight(root);

        if (root->GetParent() == NULL)
        {
            this->root = newroot;
            newroot->SetParent(NULL);
        }
        else
        {
            if (root->GetParent()->GetLeft() == root)
            {
                root->GetParent()->SetLeft(newroot);
            }
            else
            {
                root->GetParent()->SetRight(newroot);
            }
            newroot->SetParent(root->GetParent());
        }
        root->SetParent(newroot);
        root->updateHeight();
        newroot->updateHeight();
    }
    AVLNode<T> *Find_aux(AVLNode<T> *current, int key)
    {
        if (current)
        {
            if (current->GetKey() == key)
                return current; // Found the node :)
            else if (key < current->GetKey())
                return Find_aux(current->GetLeft(), key);
            else
                return Find_aux(current->GetRight(), key);
        }
        return NULL; //didnt found the node :(
    }
    void Balance(AVLNode<T> *current)
    {
        int BF = current->BalanceFactor();
        if (BF > 1)
        {
            if (current->GetLeft()->BalanceFactor() < 0)
            {
                RotateLeft(current->GetLeft()); //do LR rotation
            }

            RotateRight(current);
        }
        else if (BF < -1)
        {
            if (current->GetRight()->BalanceFactor() > 0)
            {
                RotateRight(current->GetRight()); // do RL rotation
            }

            RotateLeft(current);
        }
    }
    bool InsertNode(AVLNode<T> *current, AVLNode<T> *to_insert)
    {
        //Regular binary-tree insertion:
        if (to_insert->GetKey() < current->GetKey()) //go left
        {
            if (current->GetLeft())
            {
                if (!(InsertNode(current->GetLeft(), to_insert)))
                    return false;
            }
            else // found place for new node
            {
                current->SetLeft(to_insert);
                to_insert->SetParent(current);
            }
        }
        else if (to_insert->GetKey() > current->GetKey())
        {
            if (current->GetRight())
            {
                if (!(InsertNode(current->GetRight(), to_insert)))
                    return false;
            }
            else //found place for new node
            {
                current->SetRight(to_insert);
                to_insert->SetParent(current);
            }
        }
        else
            return false;
        current->updateHeight();
        Balance(current);
        return true;
    }

    AVLNode<T> *RemoveNode(AVLNode<T> *current, int key_to_remove)
    {
        if (current->left == NULL && current->right == NULL) //leaf
        {
            if (current == this->root)
                this->root = NULL;
            delete current;
            return NULL;
        }

        AVLNode<T> *replacment;
        if (current->key < key_to_remove) //go right
        {
            current->right = RemoveNode(current->right, key_to_remove);
        }
        else if (current->data > key_to_remove) //go left
        {
            current->left = RemoveNode(current->left, key_to_remove);
        }
        else //found node to delete
        {
            if (current->left != NULL)
            {
                replacment = GetGreatestNode(current->left);
                current->key = replacment->key;
                current->data = replacment->data;
                current->left = RemoveNode(current->left, replacment->key);
            }
            else
            {
                replacment = getInorderSuccessor(current->right);
                current->key = replacment->key;
                current->data = replacment->data;
                current->right = RemoveNode(current->right, replacment->data);
            }
        }
        current->updateHeight();
        Balance(current);
        return current;
        /*
        // normal binary tree remove
        if (current == nullptr)
            return nullptr;
        if (key_to_remove < current->key)
            current->left = RemoveNode(current->left, key_to_remove);
        else if (key_to_remove > current->key)
            current->right = RemoveNode(current->right, key_to_remove);
        else // found the node to be deleted
        {
            // node with only one child or no child
            if ((current->left == NULL) || (current->right == NULL))
            {
                AVLNode<T> *child = current->left ? current->left : current->right;
                if (child == NULL) // No child case
                {
                    current->GetParent()->GetLeft() == current
                        ? current->GetParent()->SetLeft(NULL)
                        : current->GetParent()->SetRight(NULL);
                }
                else // One child case
                {
                    child->SetParent(current->GetParent());
                    current->GetParent()->GetLeft() == current
                        ? current->GetParent()->SetLeft(child)
                        : current->GetParent()->SetRight(child);
                }
                delete current;
                current = nullptr;
            }
            else
            {
                // AVLNode<T> *successor_right_son = successor->GetRight();
                // AVLNode<T> *successor_parent = successor->GetParent();
                // successor->SetParent(current->GetParent());
                // successor->SetLeft(current->GetLeft());
                // successor->SetRight(current->GetRight());
                // current->SetRight(successor_right_son);
                // current->SetParent(successor_parent);
                AVLNode<T> *replacement = GetGreatestNode(current->GetLeft());
                current->key = replacement->key;
                current->data = replacement->data;
            }
        }
        if (current == nullptr)
            return nullptr;
        current->updateHeight();
        Balance(current);
        return current;
        */
    }

public:
    AVLTree() : root(NULL) {}
    ~AVLTree() = default;
    const T &Find(int key) const
    {
        AVLNode<T> *node = Find_aux(root, key);
        if (node)
            return node->GetData();
        else
            return NULL;
    }
    bool Exists(int key)
    {
        return Find(key) != NULL;
    }
    bool Insert(int key, const T data = NULL)
    {
        AVLNode<T> *new_node = new AVLNode<T>(key, data);
        if (!new_node)
            return false;
        if (!root) //empty tree, special case
        {
            root = new_node;
            return true;
        }
        else
            return InsertNode(root, new_node);
        return false;
    }
    bool Remove(int key)
    {
        return RemoveNode(root, key) != NULL;
    }
};

#endif