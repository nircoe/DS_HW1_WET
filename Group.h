#ifndef GROUP_H
#define GROUP_H

#include "AVLTree.h"
#include "Player.h"
#include "library1.h"

class Player;

class Group
{
    int id;
    int size;
    AVLTree<Player> *group_players_by_id;
    AVLTree<AVLTree<Player>> *group_players_by_level;

public:
    Group(int id);
    Group() = delete;
    Group(const Group &) = default;
    ~Group();

    int GetId();
    StatusType AddPlayerToGroup(Player *p);
    StatusType RemovePlayerFromGroup(Player *p);
    StatusType RemovePlayerFromGroupWithoutDelete(Player *p);
    AVLTree<Player>* GetPlayerById();
    AVLTree<AVLTree<Player>>* GetPlayerByLevel();
    void SetTrees(AVLTree<Player> *group_players_by_id, AVLTree<AVLTree<Player>> *group_players_by_level);
    void ClearGroup();
    //friend void LTRInOrderForGroups(AVLNode<Group> *node, int **array, int *index, int size);
    //gethighest
};

#endif