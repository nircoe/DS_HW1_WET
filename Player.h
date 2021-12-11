#ifndef PLAYER_H
#define PLAYER_H
#include "Group.h"

class Group;

class Player
{
    int player_id;
    int player_level;
    shared_ptr<Group> player_group;

public:
    Player();
    Player(int id, int level, shared_ptr<Group> group);
    Player(const Player &) = default;
    ~Player() = default;
    Player &operator=(const Player &) = default;

    int GetId();
    int GetLevel();
    shared_ptr<Group> GetGroup();
    void SetGroup(shared_ptr<Group> new_group);
    void IncrementLevel(int increment);

    //friend void LTRInOrderForPlayers(AVLNode<Player> *node, int **array, int *index);
    //friend void RTLInOrderForPlayers(AVLNode<AVLTree<Player>> *node, int **array, int *index);
};

#endif