#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "AVLExceptions.h"

template <typename T>
class AVLTree;

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

    AVLNode<T> *GetSmallestNode(AVLNode<T> *current)
    {
        while (current->GetLeft())
            current = current->GetLeft();
        return current;
    }
    AVLNode<T> *GetGreatestNode(AVLNode<T> *current)
    {
        while (current->GetRight())
            current = current->GetRight();
        return current;
    }
    AVLNode<T> *RotateLeft(AVLNode<T> *A)
    {
        AVLNode<T> *B = A->GetRight();
        AVLNode<T> *Bl = B->GetLeft();
        A->SetRight(Bl);
        if (Bl)
            Bl->SetParent(A);
        B->SetLeft(A);
        if (!A->GetParent()) //A is the root if the tree
        {
            this->root = B; //B is the new root
            B->SetParent(NULL);
        }
        else
        {
            if (A->GetParent()->GetLeft() == A) //A is the left child of his parent
            {
                A->GetParent()->SetLeft(B);
            }
            else //A is the right child of his parent
            {
                A->GetParent()->SetRight(B);
            }
            B->SetParent(A->GetParent());
        }
        A->SetParent(B);
        A->updateHeight();
        B->updateHeight();
        return B;
    }
    AVLNode<T> *RotateRight(AVLNode<T> *B)
    {
        AVLNode<T> *A = B->GetLeft();
        AVLNode<T> *Ar = A->GetRight();
        B->SetLeft(Ar);
        if (Ar)
            Ar->SetParent(B);
        A->SetRight(B);

        if (!B->GetParent()) //B is the root of the tree
        {
            this->root = A; //A is the new root
            A->SetParent(NULL);
        }
        else //we are in a sub-tree
        {
            if (B->GetParent()->GetLeft() == B) //B is the left child of his parent
                B->GetParent()->SetLeft(A);
            else //root is right child of his parent
                B->GetParent()->SetRight(A);
            A->SetParent(B->GetParent());
        }
        B->SetParent(A);
        B->updateHeight();
        A->updateHeight();
        return A;
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
    AVLNode<T> *Balance(AVLNode<T> *current)
    {
        int BF = current->BalanceFactor();
        if (BF > 1)
        {
            if (current->GetLeft()->BalanceFactor() < 0)
            {
                current->SetLeft(RotateLeft(current->GetLeft())); //do LR rotation
            }
            return RotateRight(current);
        }
        else if (BF < -1)
        {
            if (current->GetRight()->BalanceFactor() > 0)
            {
                current->SetRight(RotateRight(current->GetRight())); // do RL rotation
            }
            return RotateLeft(current);
        }
        return current;
    }
    AVLNode<T> *InsertNode(AVLNode<T> *current, AVLNode<T> *to_insert)
    {
        if (current == NULL)
            return to_insert;
        AVLNode<T> *sub_tree;
        if (to_insert->GetKey() < current->GetKey()) //go left
        {
            sub_tree = InsertNode(current->GetLeft(), to_insert);
            current->SetLeft(sub_tree);
            if (sub_tree)
                sub_tree->SetParent(current);
        }
        else if (to_insert->GetKey() > current->GetKey()) //go right
        {
            sub_tree = InsertNode(current->GetRight(), to_insert);
            current->SetRight(sub_tree);
            if (sub_tree)
                sub_tree->SetParent(current);
        }
        else //equal keys not allowed
            return current;
        current->updateHeight();
        return Balance(current);

        /*
        bool res = true;
        //Regular binary-tree insertion:
        if (to_insert->GetKey() < current->GetKey()) //go left
        {
            if (current->GetLeft())
            {
                res = InsertNode(current->GetLeft(), to_insert);
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
                res = InsertNode(current->GetRight(), to_insert);
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
        return res;
        */
    }

    AVLNode<T> *RemoveNode(AVLNode<T> *current, int key_to_remove)
    {
        //Regular remove from BST tree:
        if (current == NULL)
            return current;
        if (current->GetKey() > key_to_remove) //remove from left sub-tree
        {
            current->SetLeft(RemoveNode(current->GetLeft(), key_to_remove));
        }
        else if (current->GetKey() < key_to_remove) //remove from right sub-tree
        {
            current->SetRight(RemoveNode(current->GetRight(), key_to_remove));
        }
        else //found node to delete
        {
            if (current->GetLeft() == NULL || current->GetRight() == NULL) // node with one child or leaf
            {
                AVLNode<T> *child = current->GetLeft() ? current->GetLeft() : current->GetRight();
                if (child == NULL) //node is leaf
                {
                    child = current;
                    //disconnect current from his parent
                    AVLNode<T> *parent = current->GetParent();
                    if (parent) //current is not the root of the tree
                        parent->GetLeft() == current ? parent->SetLeft(NULL) : parent->SetRight(NULL);
                    current = NULL;
                }
                else //node has one child
                {
                    //copy content of child to current node
                    current->key = child->key;
                    current->data = child->data;
                    current->SetLeft(NULL);
                    current->SetRight(NULL);
                }
                delete child;
                child = NULL;
            }
            else //node has two children
            {
                AVLNode<T> *replacement;
                if (current->left != NULL)
                {
                    replacement = GetGreatestNode(current->left);
                    current->key = replacement->key;
                    current->data = replacement->data;
                    current->left = RemoveNode(current->left, replacement->key);
                }
                else
                {
                    replacement = GetSmallestNode(current->right);
                    current->key = replacement->key;
                    current->data = replacement->data;
                    current->right = RemoveNode(current->right, replacement->data);
                }
            }
        }

        //tree had one node
        if (current == NULL)
            return current;

        current->updateHeight();
        return Balance(current);

        /*
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
        else if (current->key > key_to_remove) //go left
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
        if (current == NULL)
            return NULL;
        current->updateHeight();
        Balance(current);
        return current;
        */
    }
    void print_tree(AVLNode<T> *node)
    {
        if (!node)
        {
            return;
        }
        print_tree(node->left);
        std::cout << node->key << " ";
        print_tree(node->right);
    }

public:
    AVLTree() : root(NULL) {}
    ~AVLTree() = default;
    bool IsEmpty()
    {
        return root != NULL;
    }
    T &Find(int key) const
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
        {
            root = InsertNode(root, new_node);
            //how to check that the insert succeded?
        }
        return false;
    }
    bool Remove(int key)
    {
        root = RemoveNode(root, key);
        //what to return?
    }
    void Print()
    {
        print_tree(root);
        std::cout << "" << std::endl;
    }
};

#endif