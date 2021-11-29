#include "PlayersManager.h"
#include <stdio.h>

PlayersManager::PlayersManager() : players_by_id(AVLTree<Player>()), players_by_level(AVLTree<Player>()), groups(AVLTree<Group>()) {}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0)
        return INVALID_INPUT;
    Group g = Group(GroupID);
    /*if (g)
        return ALLOCATION_ERROR;*/
    if (groups.Find(groups.GetRoot(), GroupID) == nullptr)
    {
        groups.Insert(g);
        return SUCCESS;
    }
    else
        return FAILURE;
}
StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level)
{
    if (PlayerID <= 0 || GroupID <= 0 || Level < 0)
        return INVALID_INPUT;
    if (players_by_id.Find(players_by_id.GetRoot(), PlayerID) != nullptr)
        return FAILURE;
    Group g = groups.Find(groups.GetRoot(), GroupID)->GetData();
    // if (!g)
    //     return FAILURE;

    Player p = Player(PlayerID, Level, GroupID);
    players_by_id.Insert(p);
    players_by_level.Insert(p);
    return g.AddPlayerToGroup(p);
}
StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    if (PlayerID <= 0)
        return INVALID_INPUT;
    Player p = players_by_id.Find(players_by_id.GetRoot(), PlayerID)->GetData();
    //if(!p)
    //return FAILURE;
    int g_id = p.getGroupId();
    Group g = groups.Find(groups.GetRoot(), g_id)->GetData();
    g.RemovePlayerFromGroup(p);
    // players_by_id.Remove(PlayerID);
    // players_by_level.Remove(PlayerID,level);
}
StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    Group old_group = groups.Find(groups.GetRoot(), GroupID)->GetData();
    Group new_group = groups.Find(groups.GetRoot(), ReplacementID)->GetData();
    // for (auto player : old_group.Players())
    // {
    // }
}
StatusType PlayersManager::IncreaseLevel(int PlayerID, int LevelIncrease)
{
    if (PlayerID <= 0 || LevelIncrease <= 0)
        return INVALID_INPUT;
    Player p = players_by_id.Find(players_by_id.GetRoot(), PlayerID)->GetData();
    //if(!p) return FAILURE;
    Group g = groups.Find(groups.GetRoot(), p.getGroupId())->GetData();
    g.RemovePlayerFromGroup(p);
    // players_by_id.Remove(p);
    // players_by_level.Remove(p);
    p.incrementLevel(LevelIncrease);
    players_by_id.Insert(p);
    players_by_level.Insert(p);
    g.AddPlayerToGroup(p);
}
StatusType PlayersManager::GetHighestLevel(int GroupID, int *PlayerID)
{
}
StatusType PlayersManager::GetGroupsHighestLevel(int numOfGroups, int **Players)
{
}
StatusType PlayersManager::GetAllPlayersByLevel(int GroupID, int **Players, int *numOfPlayers)
{
}
void PlayersManager::Quit()
{
    //Like this?
    players_by_id.~AVLTree();
    players_by_level.~AVLTree();
    groups.~AVLTree();
}