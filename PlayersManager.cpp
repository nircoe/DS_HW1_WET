#include "PlayersManager.h"
#include <iostream>

PlayersManager::PlayersManager() : players_by_id(AVLTree<Player>()), players_by_level(AVLTree<AVLTree<Player>>()), groups(AVLTree<Group>()) {}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0)
        return INVALID_INPUT;
    if (groups.Exists(GroupID)) // group already exists
        return FAILURE;
    if (!groups.Insert(GroupID, Group(GroupID))) // check if Insert return false => only on allocation error
        return ALLOCATION_ERROR;
    return SUCCESS;
}
StatusType PlayersManager::AddPlayer(int PlayerID, int GroupID, int Level)
{
    if (PlayerID <= 0 || GroupID <= 0 || Level < 0)
        return INVALID_INPUT;
    if (players_by_id.Exists(PlayerID) || !groups.Exists(GroupID))
        return FAILURE;
    Group *g = groups.Find(GroupID); // doesn't gonna throw because its Exists
    Player p = Player(PlayerID, Level, g);
    if (!players_by_id.Insert(PlayerID, p)) // check if Insert return false => only on allocation error
        return ALLOCATION_ERROR;
    AVLTree<Player> *p_tree;
    if (players_by_level.Exists(Level))
        p_tree = players_by_level.Find(Level); // doesn't gonna throw because its Exists
    else
    {
        p_tree = new AVLTree<Player>;
        if (!players_by_level.Insert(Level, *p_tree)) // if Insert return false => allocation error
            return ALLOCATION_ERROR;
    }
    if (!p_tree->Insert(PlayerID, p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    StatusType st = g->AddPlayerToGroup(p);
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
    Player *p = players_by_id.Find(PlayerID);
    Group *g = p->getGroup();
    StatusType st = g->RemovePlayerFromGroup(*p);
    if (st != SUCCESS) // check if the RemovePlayerFromGroup succeed
        return st;
    AVLTree<Player> *p_tree = players_by_level.Find(p->getLevel());
    if (!p_tree->Remove(p->getId())) // Remove will return false only if the tree is empty, not supposed to happened
        return FAILURE;
    if (!players_by_id.Remove(p->getId())) // Remove will return false only if the tree is empty, not supposed to happened
        return FAILURE;
    return SUCCESS;
}
StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    if (GroupID <= 0 || ReplacementID <= 0 || GroupID == ReplacementID)
        return INVALID_INPUT;
    // no group return FAILURE
    if (!groups.Exists(GroupID) || !groups.Exists(ReplacementID))
        return FAILURE;
    //pull groups from tree
    Group *current_group = groups.Find(GroupID);
    Group *replacement_group = groups.Find(ReplacementID);

    //create and merge trees by id:
    //create arrays by the size of the groups:
    Player **group, **replacement, **merged;
    int *merged_keys;
    int n_group = current_group->GetPlayerById().GetTreeSize();
    int n_replacement = replacement_group->GetPlayerById().GetTreeSize();
    *group = (Player *)malloc(n_group * sizeof(**group));
    *replacement = (Player *)malloc(n_replacement * sizeof(**replacement));
    *merged = (Player *)malloc((n_group + n_replacement) * sizeof(**merged));
    merged_keys = (int *)malloc((n_group + n_replacement) * sizeof(*merged_keys));
    //fill arrays with players sorted by Id
    group = current_group->GetPlayerById().GetDataArray();
    replacement = replacement_group->GetPlayerById().GetDataArray();
    //merge arrays
    int i = 0, j = 0, k = 0;
    while (i < n_group && j < n_replacement)
    {
        if (group[i]->getId() <= replacement[j]->getId())
        {
            merged[k] = group[i];
            merged_keys[k] = group[i]->getId();
            i++;
        }
        else
        {
            merged[k] = replacement[j];
            merged_keys[k] = replacement[j]->getId();
            j++;
        }
        k++;
    }
    while (i < n_group)
    {
        merged[k] = group[i];
        merged_keys[k] = group[i]->getId();
        k++;
        i++;
    }
    while (j < n_replacement)
    {
        merged[k] = replacement[j];
        merged_keys[k] = replacement[j]->getId();
        k++;
        j++;
    }
    int merged_size = n_group + n_replacement;
    AVLTree<Player> *merged_tree_by_id = merged_tree_by_id->SortedArrayToAVL(merged_keys, merged, merged_size - 1);

    // create and merge trees by level:
    n_group = current_group->GetPlayerByLevel().GetTreeSize();
    n_replacement = replacement_group->GetPlayerByLevel().GetTreeSize();
    AVLTree<Player> **group_by_level, **replacement_by_level, **merged_by_level;
    *group_by_level = (AVLTree<Player> *)malloc(n_group * sizeof(**group_by_level));
    *replacement_by_level = (AVLTree<Player> *)malloc(n_replacement * sizeof(**replacement_by_level));
    *merged_by_level = (AVLTree<Player> *)malloc((n_group + n_replacement) * sizeof(**merged_by_level));
    merged_keys = (int *)malloc((n_group + n_replacement) * sizeof(*merged_keys));
    //fill arrays with the AVLTrees
    group_by_level = current_group->GetPlayerByLevel().GetDataArray();
    replacement_by_level = replacement_group->GetPlayerByLevel().GetDataArray();

    //merge arrays
    i = 0;
    j = 0;
    k = 0;
    int l1, l2;
    while (i < n_group && j < n_replacement)
    {
        l1 = group_by_level[i]->GetRootData()->getLevel();
        l2 = replacement_by_level[j]->GetRootData()->getLevel();
        if (l1 < l2)
        {
            merged_by_level[k] = group_by_level[i];
            merged_keys[k] = l1;
        }
        else if (l1 > l2)
        {
            merged_by_level[k] = replacement_by_level[j];
            merged_keys[k] = l2;
        }
        else //trees of the same level, we need to merge them first
        {
            int n_group_sub = group_by_level[i]->GetTreeSize();
            int n_replacement_sub = replacement_by_level[j]->GetTreeSize();
            Player **group_sub, **replacement_sub, **merged_sub;
            int *merged_keys_sub;
            *group_sub = (Player *)malloc(n_group_sub * sizeof(**group_sub));
            *replacement_sub = (Player *)malloc(n_replacement_sub * sizeof(**replacement_sub));
            *merged_sub = (Player *)malloc((n_group_sub + n_replacement_sub) * sizeof(**merged_sub));
            merged_keys_sub = (int *)malloc((n_group_sub + n_replacement_sub) * sizeof(*merged_keys_sub));
            group_sub = group_by_level[i]->GetDataArray();
            replacement_sub = replacement_by_level[j]->GetDataArray();
            int i1 = 0, i2 = 0, i3 = 0;
            while (i1 < n_group_sub && i2 < n_replacement_sub)
            {
                if (group_sub[i1]->getId() <= replacement_sub[i2]->getId())
                {
                    merged_sub[i3] = group_sub[i1];
                    merged_keys_sub[i3] = group_sub[i1]->getId();
                    i1++;
                }
                else
                {
                    merged_sub[i3] = replacement_sub[i2];
                    merged_keys_sub[i3] = replacement_sub[i2]->getId();
                    i2++;
                }
                i3++;
            }
            while (i1 < n_group_sub)
            {
                merged_sub[i3] = group_sub[i1];
                merged_keys_sub[i3] = group_sub[i1]->getId();
                i1++;
                i3++;
            }
            while (i2 < n_replacement_sub)
            {
                merged_sub[i3] = replacement_sub[i2];
                merged_keys_sub[i3] = replacement_sub[i2]->getId();
                i2++;
                i3++;
            }
            int merged_sub_size = n_group_sub + n_replacement_sub;
            AVLTree<Player> *merged_tree_sub = merged_tree_sub->SortedArrayToAVL(merged_keys_sub, merged_sub, merged_sub_size - 1);
            merged_by_level[k] = merged_tree_sub;
            merged_keys[k] = l1;
        }
        k++;
    }
    while (i < n_group)
    {
        merged_by_level[k] = group_by_level[i];
        merged_keys[k] = group_by_level[i]->GetRootData()->getLevel();
        k++;
        i++;
    }
    while (j < n_replacement)
    {
        merged_by_level[k] = replacement_by_level[j];
        merged_keys[k] = replacement_by_level[j]->GetRootData()->getLevel();
        k++;
        j++;
    }
    merged_size = n_group + n_replacement;
    AVLTree<AVLTree<Player>> *merged_tree_by_level = merged_tree_by_level->SortedArrayToAVL(merged_keys, merged_by_level, merged_size - 1);
    replacement_group->SetTrees(*merged_tree_by_id, *merged_tree_by_level);
    return SUCCESS;
}
StatusType PlayersManager::IncreaseLevel(int PlayerID, int LevelIncrease)
{
    if (PlayerID <= 0 || LevelIncrease <= 0)
        return INVALID_INPUT;
    if (!players_by_id.Exists(PlayerID))
        return FAILURE;
    Player *p = players_by_id.Find(PlayerID);
    Group *g = p->getGroup();
    AVLTree<Player> *p_tree = players_by_level.Find(p->getLevel());
    //removing the player and increasing his level
    g->RemovePlayerFromGroup(*p);
    p_tree->Remove(p->getId());
    p->incrementLevel(LevelIncrease);

    //re-inserting the player to the level trees
    if (players_by_level.Exists(p->getLevel())) //the new level tree already exists
    {
        players_by_level.Find(p->getLevel())->Insert(p->getId(), *p);
    }
    else //creating a new player tree for this level
    {
        p_tree = new AVLTree<Player>;
        p_tree->Insert(p->getId(), *p);
        players_by_level.Insert(p->getLevel(), *p_tree);
        delete p_tree;
    }
    g->AddPlayerToGroup(*p);
    return SUCCESS;
}
StatusType PlayersManager::GetHighestLevel(int GroupID, int *PlayerID)
{
    if (GroupID == 0 || PlayerID == nullptr)
        return INVALID_INPUT;
    *PlayerID = -1; // if there are players it gonna change later
    if (GroupID < 0)
    {
        AVLTree<Player> *highest_level_tree = players_by_level.GetHighest();
        if (!highest_level_tree) // no players in the system so *PlayerID == -1
            return SUCCESS;
        Player *highest_player = highest_level_tree->GetLowest();
        if (!highest_player) // no players in the system so *PlayerID == -1
            return SUCCESS;
        *PlayerID = highest_player->getId(); // return the lowest id's player in the highest level's tree
        return SUCCESS;
    }
    if (!groups.Exists(GroupID)) // the group doesn't exist
        return FAILURE;
    Group *g = groups.Find(GroupID);                                                   // not gonna throw because it is Exists
    AVLTree<AVLTree<Player>> group_players_level_tree = g->GetPlayerByLevel();         // get the AVLTree of players by level of this group
    AVLTree<Player> *group_highest_level_tree = group_players_level_tree.GetHighest(); // get the highest node in the tree
    if (!group_highest_level_tree)                                                     // there is no players in this group
        return SUCCESS;
    Player *highest_player = group_highest_level_tree->GetLowest(); // get the lowest node in the tree => the highest player
    if (!highest_player)                                            // there is no players in this group
        return SUCCESS;
    *PlayerID = highest_player->getId();
    return SUCCESS;
}

StatusType PlayersManager::GetGroupsHighestLevel(int numOfGroups, int **Players)
{
    if (Players == nullptr || numOfGroups < 1)
        return INVALID_INPUT;
    Players = new int *[numOfGroups];
    int index = 0;
    LTRInOrderForGroups<Group>(groups.GetRoot(), Players, &index, numOfGroups);
    if (index < numOfGroups) // maybe delete Players
        return FAILURE;
    return SUCCESS;
}

StatusType PlayersManager::GetAllPlayersByLevel_AUX(AVLTree<AVLTree<Player>> tree, int **Players, int *numOfPlayers)
{
    *numOfPlayers = tree.GetTreeSize();
    if (*numOfPlayers == 0)
    {
        Players = nullptr;
        return SUCCESS;
    }
    Players = new int *[*numOfPlayers];
    int index = 0;
    RTLInOrderForPlayers<AVLTree<Player>>(tree.GetRoot(), Players, &index);
    return SUCCESS;
}

StatusType PlayersManager::GetAllPlayersByLevel(int GroupID, int **Players, int *numOfPlayers)
{
    if (Players == nullptr || numOfPlayers == nullptr)
        return INVALID_INPUT;
    if (GroupID < 0)
    {
        return GetAllPlayersByLevel_AUX(players_by_level, Players, numOfPlayers);
    }
    if (!groups.Exists(GroupID)) // the group doesn't exist
        return FAILURE;
    Group *g = groups.Find(GroupID); // not gonna throw because its Exists
    return GetAllPlayersByLevel_AUX(g->GetPlayerByLevel(), Players, numOfPlayers);
}
void PlayersManager::Quit()
{
    //Like this?
    players_by_id.~AVLTree();
    players_by_level.~AVLTree();
    groups.~AVLTree();
}