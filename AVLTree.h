#ifndef AVLTREE_H_
#define AVLTREE_H_

template <class T>
class Node
{
    T data;
    int id;
    int level; // -1 if <Group>
    Node<T> *parent;
    Node<T> *left;
    Node<T> *right;
    int height;
    Node();

public:
    Node(const T &newdata) : 
    data(newdata), parent(nullptr), left(nullptr), right(nullptr), height(0), id(-1), level(-1) {}
    ~Node();
    const T &GetData() const
    {
        return data;
    }
    void SetLeft(Node<T> *newleft)
    {
        left = newleft;
        newleft->parent = *this;
    }
    Node<T> *GetLeft() const
    {
        return left;
    }
    void SetRight(Node<T> *newright)
    {
        right = newright;
        newright->parent = *this;
    }
    Node<T> *GetRight() const
    {
        return right;
    }
    Node<T> *GetParent() const
    {
        return parent;
    }
    void SetID(int newid)
    {
        if (newid > 0)
        {
            id = newid;
        }
    }
    int GetID() const
    {
        return id;
    }
    void SetLevel(int newlevel) // used for players only
    {
        if(newlevel>0)
        {
            level = newlevel;
        }
    }
    int GetLevel() // used for players only
    {
        return level;
    }

    int GetHeight()
    {
        return height;
    }

    /////// SetHeight

};

template <class T>
class AVLTree
{
    Node<T> *root;
    int sortType; // 1 = ID, 2 = Level
    void InsertNode(Node<T> *root, Node<T> *newnode);
    void DeleteNode(Node<T> *node);
    template<typename Compare>
    void SortByID(Node<T> *root, Node<T> *newnode, Compare cmp);

public:
    AVLTree() : root(nullptr) {}
    AVLTree(const AVLTree<T> &tree);
    ~AVLTree();
    bool Insert(const T &newdata);
    Node<T> *GetRoot() const { return root; }
    Node<T> *Find(Node<T> *root, const int id) const; // sort by id
    Node<T> *Find(Node<T> *root, const int id, const int level) const; // sort by level
    int TreeHeight() const;
    int BalanceFactor(Node<T> *root) const;
    void RotateLeft(Node<T> *root);
    void RotateRight(Node<T> *root);
};

template<class T>
AVLTree<T>::~AVLTree()
{
    if(root)
    {
        DeleteNode(root);
    }
}

template<class T>
void AVLTree<T>::DeleteNode(Node<T>* node)
{
    if(node)
    {
        DeleteNode(node->GetLeft());
        DeleteNode(node->GetRight());
        delete node;
    }
}

template<class T>
bool AVLTree<T>::Insert(const T& newdata)
{
    Node<T> *newnode = new Node<T>(newdata);
    if(!newdata)
    {
        return true;
    }
    InsertNode(root, newnode);
    return false;
}

static bool BiggerThen(int n1, int n2)
{
    return n1 > n2;
}

static bool SmallerThen(int n1, int n2)
{
    return n1 < n2;
}

template<class T>
template<typename Compare>
void AVLTree<T>::SortByID(Node<T>* root, Node<T>* newnode, Compare cmp)
{
    if (cmp(newnode->GetID(),root->GetID()))
    {
        if (root->GetLeft())
        {
            InsertNode(root->GetLeft(), newnode);
        }
        else
        {
            root->SetLeft(newnode);
        }
    }
    else
    {
        if (root->GetRight())
        {
            InsertNode(root->GetRight(), newnode);
        }
        else
        {
            root->SetRight(newnode);
        }
    }
}

template<class T>
void AVLTree<T>::InsertNode(Node<T>* root, Node<T>* newnode)
{
    if(!root || !newnode)
    {
        throw null_args; // exeption
    }
    if(sortType == 1) // sort by ID <
    {
        SortByID(root, newnode, SmallerThen);
    }
    else // typeSort == 2, sort by Level, then ID >
    {
        if (newnode->GetLevel() < root->GetLevel())
        {
            if (root->GetLeft())
            {
                InsertNode(root->GetLeft(), newnode);
            }
            else
            {
                root->SetLeft(newnode);
            }
        }
        else if(newnode->GetLevel() > root->GetLevel())
        {
            if (root->GetRight())
            {
                InsertNode(root->GetRight(), newnode);
            }
            else
            {
                root->SetRight(newnode);
            }
        }
        else
        {
            SortByID(root, newnode, BiggerThen);
        }
    }
    int balance = BalanceFactor(root);
    if(balance>1)
    {
        if(BalanceFactor(root->GetLeft()) < 0)
        {
            RotateLeft(root->GetLeft());
        }
        RotateRight(root);
    }
    else if(balance < -1)
    {
        if(BalanceFactor(root->GetRight()) > 0)
        {
            RotateRight(root->GetRight());
        }
        RotateLeft(root);
    }
}

template<class T>
Node<T>* AVLTree<T>::Find(Node<T>* root, const int id) const 
{
    if(!root)
    {
        return nullptr;
    }

    if(sortType==1) // sort by ID
    {
        if(root->GetID() == id)
        {
            return root;
        }
        else if(root->GetID() > id)
        {
            return Find(root->GetLeft(), id);
        }
        else
        {
            return Find(root->GetRight(), id);
        }
    }
    return nullptr;
}

template <class T>
Node<T> *AVLTree<T>::Find(Node<T> *root, const int id, const int level) const
{
    if (!root)
    {
        return nullptr;
    }

    if(sortType == 2) // sortType == 2, sort by Level
    {
        if(root->GetLevel() == level && root->GetID() == id)
        {
            return root;
        }
        else if(root->GetLevel() > level || (root->GetLevel() == level && root->GetID() < id))
        {
            return Find(root->GetLeft(), id, level);
        }
        else
        {
            return Find(root->GetRight(), id, level);
        }
    }
}

template<class T>
int AVLTree<T>::TreeHeight() const
{
    return this->root->GetHeight();
}

#endif