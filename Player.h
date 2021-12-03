#ifndef PLAYER_H
#define PLAYER_H
#include "Group.h"

class Player
{
    int id;
    int level;
    //int groupId;
    Group group;

public:
    Player(int id, int level, Group group);
    Player(const Player &) = default;
    Player() = delete;
    ~Player() = default;

    int getId();
    int getLevel();
    //int getGroupId();
    Group getGroup();
    void setGroup(Group new_group);
    void incrementLevel(int increment);

    //friend void LTRInOrderForPlayers(AVLNode<Player> *node, int **array, int *index);
    //friend void RTLInOrderForPlayers(AVLNode<AVLTree<Player>> *node, int **array, int *index);
};

#endif