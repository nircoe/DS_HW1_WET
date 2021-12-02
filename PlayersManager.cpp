#include "PlayersManager.h"
#include <iostream>

PlayersManager::PlayersManager() : 
    players_by_id(AVLTree<Player>()), players_by_level(AVLTree<AVLTree<Player>>()), groups(AVLTree<Group>()) {}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0)
        return INVALID_INPUT;
    if (groups.Exists(GroupID)) // group already exists
        return FAILURE;
    if(!groups.Insert(GroupID, Group(GroupID))) // check if Insert return false => only on allocation error
        return ALLOCATION_ERROR;
    return SUCCESS;
}
StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level)
{
    if (PlayerID <= 0 || GroupID <= 0 || Level < 0)
        return INVALID_INPUT;
    if (players_by_id.Exists(PlayerID) || !groups.Exists(GroupID))
        return FAILURE;
    Group &g = groups.Find(GroupID); // doesn't gonna throw because its Exists
    Player p = Player(PlayerID, Level, GroupID);
    if (!players_by_id.Insert(PlayerID, p)) // check if Insert return false => only on allocation error
        return ALLOCATION_ERROR;
    AVLTree<Player> &p_tree = AVLTree<Player>();
    if (players_by_level.Exists(Level))
        p_tree = players_by_level.Find(Level); // doesn't gonna throw because its Exists
    else
    {
        if(!players_by_level.Insert(Level, p_tree)) // if Insert return false => allocation error
            return ALLOCATION_ERROR;
    }
    if (!p_tree.Insert(PlayerID, p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    StatusType st = g.AddPlayerToGroup(p); 
    if(st != SUCCESS) // if AddPlayerToGroup didn't succeed so return the StatusType that the function returned
        return st;
    return SUCCESS;
}
StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    if (PlayerID <= 0)
        return INVALID_INPUT;
    if (!players_by_id.Exists(PlayerID))
        return FAILURE;
    Player &p = players_by_id.Find(PlayerID);
    Group &g = groups.Find(p.getGroupId());
    StatusType st = g.RemovePlayerFromGroup(p);
    if(st != SUCCESS) // check if the RemovePlayerFromGroup succeed
        return st;
    AVLTree<Player>& p_tree = players_by_level.Find(p.getLevel());
    if(!p_tree.Remove(p.getId())) // Remove will return false only if the tree is empty, not supposed to happened
        return FAILURE;
    if(!players_by_id.Remove(p.getId())) // Remove will return false only if the tree is empty, not supposed to happened
        return FAILURE;
    return SUCCESS;
}
StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    if(GroupID <= 0 || ReplacementID <= 0 || GroupID == ReplacementID)
        return INVALID_INPUT;
    // no group return FAILURE
    if(groups.Exists(GroupID) && groups.Exists(ReplacementID))
    {
        Group &current_group = groups.Find(GroupID), &replacment_group = groups.Find(ReplacementID);
        AVLTree<Player> players = current_group.GetPlayerById();
        while(!players.IsEmpty()) // untill all players have been transfer
        {
            Player* root_player = players.GetRootData();
            StatusType stAdd = replacment_group.AddPlayerToGroup(*root_player);
            if(stAdd != SUCCESS) // if the add failed
                return stAdd;
            StatusType stRemove = current_group.RemovePlayerFromGroup(*root_player);
            if(stRemove != SUCCESS) // if the remove failed
                return stRemove;
            players.Remove(root_player->getId()); // the tree isn't empty so should return true always
        }
        groups.Remove(GroupID);
        return SUCCESS;
    }
    return FAILURE;
}
StatusType PlayersManager::IncreaseLevel(int PlayerID, int LevelIncrease)
{
    // **********not finished**********
    /*
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
    g.AddPlayerToGroup(p);*/
}
StatusType PlayersManager::GetHighestLevel(int GroupID, int *PlayerID)
{
    if(GroupID == 0 || PlayerID == nullptr)
        return INVALID_INPUT;
    *PlayerID = -1; // if there are players it gonna change later
    if(GroupID < 0)
    {
        AVLTree<Player> *highest_level_tree = players_by_level.GetHighest();
        if(!highest_level_tree) // no players in the system so *PlayerID == -1
            return SUCCESS;
        Player *highest_player = highest_level_tree->GetLowest();
        if (!highest_player) // no players in the system so *PlayerID == -1
            return SUCCESS;
        *PlayerID = highest_player->getId(); // return the lowest id's player in the highest level's tree
        return SUCCESS;
    }
    if(!groups.Exists(GroupID)) // the group doesn't exist
        return FAILURE;
    Group &g = groups.Find(GroupID); // not gonna throw because it is Exists
    AVLTree<AVLTree<Player>> group_players_level_tree = g.GetPlayerByLevel(); // get the AVLTree of players by level of this group
    AVLTree<Player> *group_highest_level_tree = group_players_level_tree.GetHighest(); // get the highest node in the tree
    if(!group_highest_level_tree) // there is no players in this group
        return SUCCESS;
    Player *highest_player = group_highest_level_tree->GetLowest(); // get the lowest node in the tree => the highest player
    if(!highest_player) // there is no players in this group
        return SUCCESS;
    *PlayerID = highest_player->getId();
    return SUCCESS;
}
StatusType PlayersManager::GetGroupsHighestLevel(int numOfGroups, int **Players)
{
}

StatusType GetAllPlayersByLevel_AUX(AVLTree<AVLTree<Player>> tree, int** Players, int* numOfPlayers)
{
    *numOfPlayers = tree.GetTreeSize();
    if (*numOfPlayers == 0)
    {
        Players = nullptr;
        return SUCCESS;
    }
    Players = new int *[*numOfPlayers];
    int index = 0;
    RTLInOrder(tree.GetRoot(), Players, &index);
    return SUCCESS;
}

StatusType PlayersManager::GetAllPlayersByLevel(int GroupID, int **Players, int *numOfPlayers)
{
    if(Players == nullptr || numOfPlayers == nullptr)
        return INVALID_INPUT;
    if(GroupID < 0)
    {
        return GetAllPlayersByLevel_AUX(players_by_level, Players, numOfPlayers);
    }
    if (!groups.Exists(GroupID)) // the group doesn't exist
        return FAILURE;
    Group &g = groups.Find(GroupID); // not gonna throw because its Exists
    return GetAllPlayersByLevel_AUX(g.GetPlayerByLevel(), Players, numOfPlayers);
}
void PlayersManager::Quit()
{
    //Like this?
    players_by_id.~AVLTree();
    players_by_level.~AVLTree();
    groups.~AVLTree();
}