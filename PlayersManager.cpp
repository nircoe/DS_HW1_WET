#include "PlayersManager.h"
#include <stdio.h>

PlayersManager::PlayersManager() : players_by_id(AVLTree<Player>()), players_by_level(AVLTree<Player>()), groups(AVLTree<Group>()) {}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0)
        return INVALID_INPUT;
    if (groups.Exists(GroupID)) //group already exists
        return FAILURE;
    groups.Insert(GroupID, Group(GroupID));
    return SUCCESS;
}
StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level)
{
    if (PlayerID <= 0 || GroupID <= 0 || Level < 0)
        return INVALID_INPUT;
    if (players_by_id.Exists(PlayerID) || !groups.Exists(GroupID))
        return FAILURE;
    Group g = groups.Find(GroupID);
    Player p = Player(PlayerID, Level, GroupID);
    players_by_id.Insert(PlayerID, p);
    AVLTree<Player> p_tree;
    if (players_by_level.Exists(Level))
        p_tree = players_by_level.Find(Level);
    else
        p_tree = AVLTree<Player>();
    p_tree.Insert(PlayerID, p);
    g.AddPlayerToGroup(p);
    return SUCCESS;
}
StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    if (PlayerID <= 0)
        return INVALID_INPUT;
    if (!players_by_id.Exists(PlayerID))
        return FAILURE;
    Player p = players_by_id.Find(PlayerID);
    Group g = groups.Find(p.getGroupId());
    g.RemovePlayerFromGroup(p);
    AVLTree<Player> p_tree = players_by_level.Find(p.getLevel());
    p_tree.Remove(p.getId());
    players_by_id.Remove(p.getId());
}
StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    
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