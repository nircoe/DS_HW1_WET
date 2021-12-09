#ifndef PLAYERS_MNGR_H
#define PLAYERS_MNGR_H

#include "library1.h"
#include "AVLTree.h"
#include "Player.h"
#include "Group.h"

class PlayersManager
{
    AVLTree<Player> players_by_id;
    AVLTree<AVLTree<Player>> players_by_level;
    AVLTree<Group> groups;
    AVLTree<Group> empty_groups;
    int* GetAllPlayersByLevel_AUX(AVLTree<AVLTree<Player>> *tree, int *numOfPlayers);

public:
    PlayersManager();
    ~PlayersManager() = default;
    StatusType AddGroup(int GroupID);
    StatusType AddPlayer(int PlayerID, int GroupID, int Level);
    StatusType RemovePlayer(int PlayerID);
    StatusType ReplaceGroup(int GroupID, int ReplacementID);
    StatusType IncreaseLevel(int PlayerID, int LevelIncrease);
    StatusType GetHighestLevel(int GroupID, int *PlayerID);
    int* GetGroupsHighestLevel(int numOfGroups, StatusType *st);
    int* GetAllPlayersByLevel(int GroupID, int *numOfPlayers, StatusType *st);
    void Quit(PlayersManager *pm);
};

#endif