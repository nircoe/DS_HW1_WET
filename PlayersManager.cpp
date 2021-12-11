#include "PlayersManager.h"
#include <iostream>

PlayersManager::PlayersManager() : players_by_id(AVLTree<shared_ptr<Player>>()),
                                   players_by_level(AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>>()),
                                   groups(AVLTree<shared_ptr<Group>>()),
                                   empty_groups(AVLTree<shared_ptr<Group>>()) {}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0)
        return INVALID_INPUT;
    if (groups.Exists(GroupID) || empty_groups.Exists(GroupID)) // group already exists
        return FAILURE;
    shared_ptr<Group> g = make_shared<Group>(GroupID);
    if (!empty_groups.Insert(GroupID, g)) // check if Insert return false => only on allocation error
        return ALLOCATION_ERROR;
    return SUCCESS;
}
StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level)
{
    if (PlayerID <= 0 || GroupID <= 0 || Level < 0)
        return INVALID_INPUT;
    if (players_by_id.Exists(PlayerID))
        return FAILURE;
    shared_ptr<Group> g(nullptr);
    bool is_first_player = false;
    if (groups.Exists(GroupID))
        g = groups.Find(GroupID);
    else if (empty_groups.Exists(GroupID))
    {
        g = empty_groups.Find(GroupID);
        is_first_player = true;
    }
    else
        return FAILURE;
    if (is_first_player) // move group from empty groups to not empty groups tree
    {
        if (!empty_groups.Remove(GroupID))
            return ALLOCATION_ERROR;
        if (!groups.Insert(GroupID, g))
            return ALLOCATION_ERROR;
    }
    shared_ptr<Player> p = make_shared<Player>(PlayerID, Level, g);
    shared_ptr<AVLTree<shared_ptr<Player>>> p_tree;
    if (players_by_level.Exists(Level))
        p_tree = players_by_level.Find(Level); // doesn't gonna throw because its Exists
    else
    {
        p_tree = make_shared<AVLTree<shared_ptr<Player>>>();
        if (!players_by_level.Insert(Level, p_tree)) // if Insert return false => allocation error
            return ALLOCATION_ERROR;
    }
    if (!p_tree.get()->Insert(PlayerID, p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    StatusType st = g.get()->AddPlayerToGroup(p);
    if (st != SUCCESS) // if AddPlayerToGroup didn't succeed so return the StatusType that the function returned
        return st;
    return SUCCESS;
}
StatusType PlayersManager::RemovePlayer(int PlayerID)
{
    if (PlayerID <= 0)
        return INVALID_INPUT;
    if (!players_by_id.Exists(PlayerID))
        return FAILURE;
    shared_ptr<Player> p = players_by_id.Find(PlayerID);
    shared_ptr<Group> g = p.get()->GetGroup();
    StatusType st = g.get()->RemovePlayerFromGroup(PlayerID,p.get()->GetLevel());
    if (st != SUCCESS) // check if the RemovePlayerFromGroup succeed
        return st;
    if (g.get()->GetSize() == 0) //we removed the last player
    {
        if (!groups.Remove(g.get()->GetId()))
            return FAILURE;
        if (!empty_groups.Insert(g->GetId(), g))
            return FAILURE;
    }
    shared_ptr<AVLTree<shared_ptr<Player>>> p_tree = players_by_level.Find(p.get()->GetLevel());
    if (!p_tree.get()->Remove(p.get()->GetId()))
        return FAILURE;
    if(p_tree.get()->GetTreeSize() == 0)
        players_by_level.Remove(p.get()->GetLevel());
    if (!players_by_id.Remove(p.get()->GetId()))
        return FAILURE;
    p.reset();
    return SUCCESS;
}
StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    return SUCCESS;
}
StatusType PlayersManager::IncreaseLevel(int PlayerID, int LevelIncrease)
{
    if (PlayerID <= 0 || LevelIncrease <= 0)
        return INVALID_INPUT;
    if (!players_by_id.Exists(PlayerID))
        return FAILURE;
    shared_ptr<Player> p = players_by_id.Find(PlayerID);
    int p_id = p.get()->GetId(), p_level = p.get()->GetLevel();
    shared_ptr<Group> g = p.get()->GetGroup();
    shared_ptr<AVLTree<shared_ptr<Player>>> p_tree = players_by_level.Find(p_level);
    //removing the player and increasing his level
    p_tree.get()->Remove(p_id);
    if(p_tree.get()->GetTreeSize() == 0)
    {
        players_by_level.Remove(p_level);
        p_tree.reset();
    }
    players_by_id.Remove(p_id);
    g.get()->RemovePlayerFromGroup(p_id,p_level);
    p.get()->IncrementLevel(LevelIncrease);
    p_level += LevelIncrease;
    //re-inserting the player to the level trees
    if (players_by_level.Exists(p_level)) //the new level tree already exists
    {
        players_by_level.Find(p_level)->Insert(p_id, p);
    }
    else //creating a new player tree for this level
    {
        shared_ptr<AVLTree<shared_ptr<Player>>> new_p_tree = make_shared<AVLTree<shared_ptr<Player>>>();
        new_p_tree.get()->Insert(p_id, p);
        players_by_level.Insert(p_level, new_p_tree);
    }
    g->AddPlayerToGroup(p);
    players_by_id.Insert(p_id, p);
    return SUCCESS;
}
StatusType PlayersManager::GetHighestLevel(int GroupID, int *PlayerID)
{
    if (GroupID == 0 || PlayerID == nullptr)
        return INVALID_INPUT;
    *PlayerID = -1; // if there are players it gonna change later
    if (GroupID < 0)
    {
        shared_ptr<AVLTree<shared_ptr<Player>>> highest_level_tree = players_by_level.GetHighest();
        if (!highest_level_tree.get()) // no players in the system so *PlayerID == -1
            return SUCCESS;
        shared_ptr<Player> highest_player = highest_level_tree.get()->GetLowest();
        if (!highest_player) // no players in the system so *PlayerID == -1
            return SUCCESS;
        *PlayerID = highest_player.get()->GetId(); // return the lowest id's player in the highest level's tree
        highest_level_tree.reset();
        highest_player.reset();
        return SUCCESS;
    }
    if (!groups.Exists(GroupID)) // the group doesn't exist
        return FAILURE;
    shared_ptr<Group> g = groups.Find(GroupID);                                                   // not gonna throw because it is Exists
    AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> group_players_level_tree = g.get()->GetPlayerByLevel();         // get the AVLTree of players by level of this group
    shared_ptr<AVLTree<shared_ptr<Player>>> group_highest_level_tree = group_players_level_tree.GetHighest(); // get the highest node in the tree
    if (!group_highest_level_tree.get())                                                      // there is no players in this group
        return SUCCESS;
    shared_ptr<Player> highest_player = group_highest_level_tree.get()->GetLowest(); // get the lowest node in the tree => the highest player
    if (!highest_player)                                            // there is no players in this group
        return SUCCESS;
    *PlayerID = highest_player.get()->GetId();
    return SUCCESS;
}

int *PlayersManager::GetGroupsHighestLevel(int numOfGroups, StatusType *st)
{
    if (numOfGroups < 1)
    {
        *st = INVALID_INPUT;
        return nullptr;
    }
    int *Players = (int *)malloc(numOfGroups * sizeof(int));
    int index = 0;
    LTRInOrderForGroups<shared_ptr<Group>>(groups.GetRoot(), &Players, &index, numOfGroups);
    if (index < numOfGroups)
        {
            *st = FAILURE;
            return nullptr;
        }
    *st = SUCCESS;
    return Players;
}

int* PlayersManager::GetAllPlayersByLevel_AUX(AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> tree, int numOfPlayers)
{
    if (numOfPlayers == 0)
        return nullptr;
    int *Players = (int *)malloc(numOfPlayers * sizeof(int));
    int index = 0;
    RTLInOrderForPlayers<shared_ptr<AVLTree<shared_ptr<Player>>>>(tree.GetRoot(), &Players, &index);
    return Players;
}

int *PlayersManager::GetAllPlayersByLevel(int GroupID, int *numOfPlayers, StatusType *st)
{
    *st = SUCCESS;
    if (numOfPlayers == nullptr)
    {
        *st = INVALID_INPUT;
        return nullptr;
    }
    if (GroupID < 0)
    {
        *numOfPlayers = players_by_id.GetTreeSize();
        return GetAllPlayersByLevel_AUX(players_by_level, *numOfPlayers);
    }
    if (!groups.Exists(GroupID)) // the group doesn't exist
    {
        *st = FAILURE;
        return nullptr;
    }
    shared_ptr<Group> g = groups.Find(GroupID); // not gonna throw because its Exists
    *numOfPlayers = g.get()->GetSize();
    return GetAllPlayersByLevel_AUX(g.get()->GetPlayerByLevel(), *numOfPlayers);
}

void PlayersManager::Quit(PlayersManager *pm)
{
    delete pm;
}