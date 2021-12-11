#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "AVLExceptions.h"
#include <memory>
using std::make_shared;
using std::shared_ptr;

template <typename T>
class AVLNode;
template <typename T>
class AVLTree;

template <typename type>
AVLTree<type> *MergeTrees(AVLTree<type> &tr1, AVLTree<type> &tr2);
template <typename type>
void LTRInOrderForGroups(AVLNode<type> *node, int **array, int *index, int size);
template <typename type>
void LTRInOrderForPlayers(AVLNode<type> *node, int **array, int *index);
template <typename type>
void RTLInOrderForPlayers(AVLNode<type> *node, int **array, int *index);

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
    AVLNode(int &new_key, const T &new_data) : 
            key(new_key), data(new_data),
                right(nullptr), left(nullptr), parent(nullptr), height(0) {}
    AVLNode(const AVLNode<T> &) = default;
    AVLNode &operator=(const AVLNode &) = default;
    ~AVLNode()
    {
        data.reset();
    }
    int GetMax(int a, int b) { return a > b ? a : b; }
    int GetKey() const { return (this != 0) ? key : -1; }
    T GetData() { return this->data; }
    void SetData(T new_data) { data = new_data; }
    void SetLeft(AVLNode *new_left) { left = new_left; }
    AVLNode *GetLeft() const { return (this != 0) ? left : nullptr; }
    void SetRight(AVLNode *new_right) { right = new_right; }
    AVLNode *GetRight() const { return (this != 0) ? right : nullptr; }
    void SetParent(AVLNode *new_parent) { parent = new_parent; }
    AVLNode *GetParent() const { return (this != 0) ? parent : nullptr; }
    int GetHeight() const { return (this != 0) ? height : -1; }
    int BalanceFactor() const { return this->GetLeft()->GetHeight() - this->GetRight()->GetHeight(); }
    void updateHeight() { this->height = 1 + GetMax(this->GetLeft()->GetHeight(), this->GetRight()->GetHeight()); }
    void ClearNode() { right = left = parent = nullptr; }

    friend class AVLTree<T>;

    template <typename type>
    friend void LTRInOrderForGroups(AVLNode<type> *node, int **array, int *index, int size);
    template <typename type>
    friend void LTRInOrderForPlayers(AVLNode<type> *node, int **array, int *index);
    template <typename type>
    friend void RTLInOrderForPlayers(AVLNode<type> *node, int **array, int *index);
};

template <typename T>
class AVLTree
{
private:
    AVLNode<T> *root;
    AVLNode<T> *highest;
    AVLNode<T> *lowest;
    int size;

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
        if (current != nullptr)
        {
            const int k = current->GetKey();
            if (k == key)
                return current; // Found the node :)
            else if (key < k)
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
                    {
                        parent->GetLeft() == current ? parent->SetLeft(nullptr) : parent->SetRight(nullptr);
                    }
                }
                else //node has one child
                {
                    //copy content of child to current node
                    if (highest == child)
                        highest = current;
                    if (lowest == child)
                        lowest = current;
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
                if (replacement == highest)
                    highest = current;
                if (replacement == lowest)
                    lowest = current;
                current->key = replacement->key;
                current->data = replacement->data;
                current->left = RemoveNode(current->left, replacement->key);
            }
        }
        //tree had one node
        if (current == nullptr)
            return current;

        current->updateHeight();
        return Balance(current);
    }
    void GetDataArray_AUX(AVLNode<T> *node, T *array, int *index)
    {
        if (!node)
            return;
        GetDataArray_AUX(node->GetLeft(), array, index);
        array[(*index)++] = node->GetData();
        //array.get()[(*index)++].reset(new T(*(node->GetData().get())));
        GetDataArray_AUX(node->GetRight(), array, index);
    }
    void GetKeysArray_AUX(AVLNode<T> *node, int *array, int *index)
    {
        if (!node)
            return;
        GetKeysArray_AUX(node->GetLeft(), array, index);
        array[(*index)++] = node->GetKey();
        GetKeysArray_AUX(node->GetRight(), array, index);
    }
    AVLNode<T> *SortedArrayToAVLTree(int *keys, T *data, int start, int end)
    {
        if (start > end)
            return nullptr;
        int mid = (start + end) / 2;
        AVLNode<T> *current = new AVLNode<T>(keys[mid], data[mid]);
        AVLNode<T> *left_child = SortedArrayToAVLTree(keys, data, start, mid - 1);
        current->SetLeft(left_child);
        if (left_child)
            left_child->SetParent(current);
        AVLNode<T> *right_child = SortedArrayToAVLTree(keys, data, mid + 1, end);
        current->SetRight(right_child);
        if (right_child)
            right_child->SetParent(current);
        current->updateHeight();
        return current;
    }

    void SetHighest(AVLNode<T> *new_highest)
    {
        highest = new_highest;
    }
    void SetLowest(AVLNode<T> *new_lowest)
    {
        lowest = new_lowest;
    }
    void PostOrderDelete(AVLNode<T> *node)
    {
        if (node != nullptr)
        {
            PostOrderDelete(node->GetLeft());
            PostOrderDelete(node->GetRight());
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr), highest(nullptr), lowest(nullptr), size(0) {}
    AVLTree(int *keys, T *data, int size_of_array) // Sorted Array to AVL Tree Constructor
    {
        if (size_of_array == 0)
            throw FAILURE_exception();
        //make sure the array is sorted
        for (int i = 1; i < size_of_array; i++)
        {
            if (keys[i - 1] >= keys[i])
                throw FAILURE_exception();
        }
        this->root = SortedArrayToAVLTree(keys, data, 0, size_of_array - 1);
        this->highest = GetGreatestNode(root);
        this->lowest = GetSmallestNode(root);
        this->size = size_of_array;
    }
    /*void CopyCons_AUX(AVLNode<T>* copy)
    {
        if(!copy)
            return;
        CopyCons_AUX(copy->GetLeft());
        T new_data = T(copy->GetData());
        //T *new_data = new T(*(copy->GetData().get()));
        this->Insert(copy->GetKey(), new_data);
        CopyCons_AUX(copy->GetRight());
    }
    AVLTree(const AVLTree<T> &tree)
    {
        CopyCons_AUX(tree.GetRoot());
    }*/
    ~AVLTree()
    {
        try
        {
            PostOrderDelete(root);
            size = -1;
        }
        catch (const std::exception &e) { }
    }
    void SwitchNodeData(int switch_key, T* new_data, AVLNode<T>* node)
    {
        if(!node)
            return;
        if(node->GetKey() == switch_key)
        {
            shared_ptr<T> d = shared_ptr<T>(new_data);
            node->SetData(d);
        }
        else if (node->GetKey() > switch_key)
        {
            SwitchNodeData(switch_key, new_data, node->GetLeft());
        }
        else if(node->GetKey() < switch_key)
            SwitchNodeData(switch_key, new_data, node->GetRight());
    }
    AVLNode<T> *GetRoot() const
    {
        return root;
    }
    int GetTreeSize() const
    {
        return size;
    }
    T GetRootData() const
    {
        if (root != nullptr)
            return root->GetData();
        return nullptr;
    }
    T GetHighest() const
    {
        if (highest != nullptr)
            return highest->GetData();
        return nullptr;
    }
    T GetLowest() const
    {
        if (lowest != nullptr)
            return lowest->GetData();
        return nullptr;
    }
    bool IsEmpty()
    {
        return root != nullptr;
    }
    T Find(int key)
    {
        AVLNode<T> *node = Find_aux(this->root, key);
        if (node)
            return node->GetData();
        return nullptr;
    }
    bool Exists(int key)
    {
        if (Find(key) == nullptr)
            return false;
        return true;
    }
    bool Insert(int new_key, T new_data)
    {
        AVLNode<T> *new_node = new AVLNode<T>(new_key, new_data);
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
        if (this->highest->GetKey() == key) // change the highest if he will removed
        {
            if (this->highest->GetParent() == nullptr)
            {
                AVLNode<T> *new_highest = highest->GetLeft();
                if (new_highest)
                    while (new_highest->GetRight() != nullptr)
                        new_highest = new_highest->GetRight();
                this->highest = new_highest;
            }
            else
                this->highest = highest->GetParent();
        }
        // if we remove the lowest :
        if (this->lowest->GetKey() == key) // change the lowest if he will removed
        {
            if (this->lowest->GetParent() == nullptr)
            {
                AVLNode<T> *new_lowest = lowest->GetRight();
                while (new_lowest->GetLeft() != nullptr)
                    new_lowest = new_lowest->GetLeft();
                this->lowest = new_lowest;
            }
            else
                this->lowest = lowest->GetParent();
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
    T *GetDataArray()
    {
        int index = 0;
        T *array = new T[size];
        GetDataArray_AUX(root, array, &index);
        return array;
    }
    int *GetKeysArray()
    {
        int index = 0;
        int *array = new int[size];
        GetKeysArray_AUX(root, array, &index);
        return array;
    }

    friend class Group;
    friend class Player;

    template <typename type>
    friend AVLTree<type> *MergeTrees(AVLTree<type> &tr1, AVLTree<type> &tr2);
    template <typename type>
    friend void LTRInOrderForGroups(AVLNode<type> *node, int **array, int *index, int size);
    template <typename type>
    friend void LTRInOrderForPlayers(AVLNode<type> *node, int **array, int *index);
    template <typename type>
    friend void RTLInOrderForPlayers(AVLNode<type> *node, int **array, int *index);
};

template <typename type>
void LTRInOrderForGroups(AVLNode<type> *node, int **array, int *index, int size)
{
    if (!node || *index >= size)
        return;
    LTRInOrderForGroups(node->GetLeft(), array, index, size);
    int id = node->GetData().get()->GetPlayerByLevel().GetHighest().get()->GetLowest().get()->GetId();
    (*array)[*index] = id;
    ++(*index);
    // not gonna get nullptr in GetHighest() and GetLowest() because there are players in this group
    LTRInOrderForGroups(node->GetRight(), array, index, size);
}

template <typename type>
void LTRInOrderForPlayers(AVLNode<type> *node, int **array, int *index) // left to right
{
    if (!node)
        return;
    LTRInOrderForPlayers(node->GetLeft(), array, index);
    (*array)[(*index)++] = node->GetData().get()->GetId();
    LTRInOrderForPlayers(node->GetRight(), array, index);
}

template <typename type>
void RTLInOrderForPlayers(AVLNode<type> *node, int **array, int *index) // right to left
{
    if (!node)
        return;
    RTLInOrderForPlayers(node->GetRight(), array, index);
    LTRInOrderForPlayers(node->GetData().get()->GetRoot(), array, index);
    RTLInOrderForPlayers(node->GetLeft(), array, index);
}

template <typename type>
AVLTree<type> *MergeTrees(AVLTree<type> &tr1, AVLTree<type> &tr2)
{
    int n1 = tr1.GetTreeSize(), n2 = tr2.GetTreeSize();
    int *keys1 = tr1.GetKeysArray(), *keys2 = tr2.GetKeysArray();
    type *data1 = tr1.GetDataArray(), *data2 = tr2.GetDataArray();
    int i1 = 0, i2 = 0, j = 0;
    type *merged_data = new type[n1 + n2];
    int *merged_keys = new int[n1 + n2];
    while (i1 < n1 && i2 < n2)
    {
        if (keys1[i1] < keys2[i2])
        {
            merged_data[j] = data1[i1];
            merged_keys[j] = keys1[i1];
            i1++;
        }
        else if (keys1[i1] > keys2[i2])
        {
            merged_data[j] = data2[i2];
            merged_keys[j] = keys2[i2];
            i2++;
        }
        else //found duplicate key!
            throw FAILURE_exception();
        j++;
    }
    while (i1 < n1)
    {
        merged_data[j] = data1[i1];
        merged_keys[j] = keys1[i1];
        i1++;
        j++;
    }
    while (i2 < n2)
    {
        merged_data[j] = data2[i2];
        merged_keys[j] = keys2[i2];
        i2++;
        j++;
    }
    AVLTree<type> *merged_tree = new AVLTree<type>(merged_keys, merged_data, n1 + n2);
    delete[] keys1;
    delete[] keys2;
    delete[] data1;
    delete[] data2;
    delete[] merged_data;
    delete[] merged_keys;

    return merged_tree;
}


#endif