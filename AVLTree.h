#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "AVLExceptions.h"
#include "Player.h"

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
    AVLNode(int key, const T &data) : key(key), data(data), right(nullptr), left(nullptr), parent(nullptr), height(0) {}
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
    friend void LTRInOrder(AVLNode<Player> *node, int **array, int *index);
    friend void RTLInOrder(AVLNode<AVLTree<Player>> *node, int **array, int *index);
};

template <typename T>
class AVLTree
{
    AVLNode<T> *root;
    AVLNode<T> *highest;
    AVLNode<T> *lowest;
    int size;

    AVLNode<T> *GetSmallestNode(AVLNode<T> *current)
    {
        return lowest;
        /*
        while (current->GetLeft())
            current = current->GetLeft();
        return current;
        */
    }
    T *GetHighest()
    {
        T data = highest->GetData();
        return *T;
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
            B->SetParent(nullptr);
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
            A->SetParent(nullptr);
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
        return nullptr; //didnt found the node :(
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
        if (current == nullptr)
            return to_insert;
        AVLNode<T> *sub_tree;
        if (to_insert->GetKey() < current->GetKey()) // go left
        {
            sub_tree = InsertNode(current->GetLeft(), to_insert);
            current->SetLeft(sub_tree);
            if (sub_tree)
                sub_tree->SetParent(current);
        }
        else if (to_insert->GetKey() > current->GetKey()) // go right
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
    }

    AVLNode<T> *RemoveNode(AVLNode<T> *current, int key_to_remove)
    {
        //Regular remove from BST tree:
        if (current == nullptr)
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
            if (current->GetLeft() == nullptr || current->GetRight() == nullptr) // node with one child or leaf
            {
                AVLNode<T> *child = current->GetLeft() ? current->GetLeft() : current->GetRight();
                if (child == nullptr) //node is leaf
                {
                    child = current;
                    //disconnect current from his parent
                    AVLNode<T> *parent = current->GetParent();
                    if (parent) //current is not the root of the tree
                        parent->GetLeft() == current ? parent->SetLeft(nullptr) : parent->SetRight(nullptr);
                    current = nullptr;
                }
                else //node has one child
                {
                    //copy content of child to current node
                    current->key = child->key;
                    current->data = child->data;
                    current->SetLeft(nullptr);
                    current->SetRight(nullptr);
                }
                delete child;
                child = nullptr;
            }
            else //node has two children
            {
                AVLNode<T> *replacement = GetGreatestNode(current->left);
                current->key = replacement->key;
                current->data = replacement->data;
                current->left = RemoveNode(current->left, replacement->key);

                /* previous code :
                
                 AVLNode<T> *replacement;
                if (current->left != nullptr)
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
                */
            }
        }
        //tree had one node
        if (current == nullptr)
            return current;

        current->updateHeight();
        return Balance(current);
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
    AVLTree() : root(nullptr), highest(nullptr), lowest(nullptr), size(0) {}
    ~AVLTree() = default;
    AVLNode<T> *GetRoot() const
    {
        return root;
    }
    int GetTreeSize() const
    {
        return size;
    }
    T *GetRootData() const
    {
        if (root != nullptr)
            return &(root->GetData());
        return nullptr;
    }
    T *GetHighest() const
    {
        if (highest != nullptr)
            return highest->GetData();
        return nullptr;
    }
    T *GetLowest() const
    {
        if (lowest != nullptr)
            return lowest->GetData();
        return nullptr;
    }
    bool IsEmpty()
    {
        return root != nullptr;
    }
    T &Find(int key) const
    {
        AVLNode<T> *node = Find_aux(root, key);
        if (node)
            return node->GetData();
        throw FAILURE_exception();
        // return nullptr;
    }
    bool Exists(int key)
    {
        try
        {
            T &f = Find(key);
        }
        catch (FAILURE_exception &e) // goes here only if doesn't Exist
        {
            return false;
        }
        return true; // didn't go to the catch => it Exist
    }
    bool Insert(int key, const T data = NULL)
    {
        AVLNode<T> *new_node = new AVLNode<T>(key, data);
        if (!new_node)
            return false; // ALLOCATION_ERROR
        if (!root)        //empty tree, special case
        {
            root = new_node;
            highest = new_node;
            lowest = new_node;
            size++;
            return true;
        }
        else
        {
            root = InsertNode(root, new_node);
            //how to check that the insert succeded?
            // if we insert object that his key is bigger then the highest
            if (new_node->GetKey() > highest->GetKey())
                highest = new_node;
            // if we insert object that his key is smaller then the lowest
            else if (new_node->GetKey() < lowest->GetKey())
                lowest = new_node;
        }
        size++;
        return true; // was return false, but it is success , no?
        //return false; // previous code
    }
    bool Remove(int key)
    {
        // if we remove the highest :
        if (highest->GetKey() == key) // change the highest if he will removed
        {
            if (highest->GetParent() == nullptr)
            {
                AVLNode<T> *new_highest = highest->GetLeft();
                if (new_highest)
                    while (new_highest->GetRight() != nullptr)
                        new_highest = new_highest->GetRight();
                highest = new_highest;
            }
            else
                highest = highest->GetParent();
        }
        // if we remove the lowest :
        if (lowest->GetKey() == key) // change the lowest if he will removed
        {
            if (lowest->GetParent() == nullptr)
            {
                AVLNode<T> *new_lowest = lowest->GetRight();
                while (new_lowest->GetLeft() != nullptr)
                    new_lowest = new_lowest->GetLeft();
                lowest = new_lowest;
            }
            else
                lowest = lowest->GetParent();
        }
        root = RemoveNode(root, key); // it will return nullptr only if root == nullptr
        if (!root)                    // RemoveNode failure, root was nullptr, tree is empty
            return false;
        size--;
        return true; // RemoveNode succeed
    }
    void Print()
    {
        print_tree(root);
        std::cout << "" << std::endl;
    }
    friend void LTRInOrder(AVLNode<Player> *node, int **array, int *index);
    friend void RTLInOrder(AVLNode<AVLTree<Player>> *node, int **array, int *index);
};

void LTRInOrder(AVLNode<Player> *node, int **array, int *index) // left to right
{
    if (!node)
        return;
    LTRInOrder(node->GetLeft(), array, index);
    *array[(*index)++] = node->GetData().getId();
    LTRInOrder(node->GetRight(), array, index);
}

void RTLInOrder(AVLNode<AVLTree<Player>> *node, int **array, int *index) // right to left
{
    if (!node)
        return;
    RTLInOrder(node->GetRight(), array, index);
    LTRInOrder(node->GetData().GetRoot(), array, index);
    RTLInOrder(node->GetLeft(), array, index);
}

#endif