#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    int id;
    int level;
    int groupId;

public:
    Player(int id, int level, int groupId);
    Player(const Player &) = default;
    Player() = delete;
    ~Player() = default;

    int getId();
    int getLevel();
    int getGroupId();
    int setGroupId(int new_id);
    void incrementLevel(int increment);
    friend void LTRInOrderForPlayers(AVLNode<Player> *node, int **array, int *index);
    friend void RTLInOrderForPlayers(AVLNode<AVLTree<Player>> *node, int **array, int *index);
};

void LTRInOrderForPlayers(AVLNode<Player> *node, int **array, int *index) // left to right
{
    if (!node)
        return;
    LTRInOrderForPlayers(node->GetLeft(), array, index);
    *array[(*index)++] = node->GetData().getId();
    LTRInOrderForPlayers(node->GetRight(), array, index);
}

void RTLInOrderForPlayers(AVLNode<AVLTree<Player>> *node, int **array, int *index) // right to left
{
    if (!node)
        return;
    RTLInOrderForPlayers(node->GetRight(), array, index);
    LTRInOrderForPlayers(node->GetData().GetRoot(), array, index);
    RTLInOrderForPlayers(node->GetLeft(), array, index);
}

#endif