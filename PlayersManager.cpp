#include "PlayersManager.h"
#include <iostream>

//auxilary functions:

PlayersManager::PlayersManager() : players_by_id(AVLTree<shared_ptr<Player>>()),
                                   players_by_level(AVLTree<AVLTree<shared_ptr<Player>>>()),
                                   groups(AVLTree<shared_ptr<Group>>()),
                                   empty_groups(AVLTree<shared_ptr<Group>>()) {}

StatusType PlayersManager::AddGroup(int GroupID)
{
    if (GroupID <= 0)
        return INVALID_INPUT;
    if (groups.Exists(GroupID) || empty_groups.Exists(GroupID)) // group already exists
        return FAILURE;
    Group *g = new Group(GroupID);
    if (empty_groups.GetTreeSize() == 0)
        empty_groups.Reset();
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
    Group *g;
    bool is_first_player = false;
    if (groups.Exists(GroupID))
    {
        g = groups.Find(GroupID);
    }
    else if (empty_groups.Exists(GroupID))
    {
        g = empty_groups.Find(GroupID);
        is_first_player = true;
    }
    else
        return FAILURE;
    Group *new_group = g;
    if (is_first_player) // move group from empty groups to not empty groups tree
    {
        new_group = new Group(*g);
        if (!empty_groups.Remove(GroupID))
            return ALLOCATION_ERROR;
        if (empty_groups.GetTreeSize() == 0)
            empty_groups.Reset();
        if (groups.GetTreeSize() == 0)
            groups.Reset();
        if (!groups.Insert(GroupID, new_group))
            return ALLOCATION_ERROR;
        g = new_group;
    }
    //Group *g = groups.Find(GroupID); // doesn't gonna throw because its Exists
    Player *p = new Player(PlayerID, Level, g);
    if (players_by_id.GetTreeSize() == 0)
        players_by_id.Reset();
    if (!players_by_id.Insert(PlayerID, p)) // check if Insert return false => only on allocation error
        return ALLOCATION_ERROR;
    AVLTree<Player> *p_tree;
    if (players_by_level.Exists(Level))
        p_tree = players_by_level.Find(Level); // doesn't gonna throw because its Exists
    else
    {
        p_tree = new AVLTree<Player>();
        if (players_by_level.GetTreeSize() == 0)
            players_by_level.Reset();
        if (!players_by_level.Insert(Level, p_tree)) // if Insert return false => allocation error
            return ALLOCATION_ERROR;
    }
    if (p_tree->GetTreeSize() == 0)
        p_tree->Reset();
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
    StatusType st = g->RemovePlayerFromGroup(p);
    if (st != SUCCESS) // check if the RemovePlayerFromGroup succeed
        return st;
    if (g->GetPlayerById()->GetTreeSize() == 0) //we removed the last player
    {
        if (!groups.Remove(g->GetId()))
            return FAILURE;
        if (!empty_groups.Insert(g->GetId(), g))
            return FAILURE;
    }
    AVLTree<Player> *p_tree = players_by_level.Find(p->getLevel());
    if (!p_tree->Remove(p->getId())) // Remove will return false only if the tree is empty, not supposed to happened
        return FAILURE;
    if (p_tree->GetTreeSize() == 0)
        players_by_level.Remove(p->getLevel());
    if (players_by_level.GetTreeSize() == 0)
        players_by_level.Reset();
    if (!players_by_id.Remove(p->getId())) // Remove will return false only if the tree is empty, not supposed to happened
        return FAILURE;
    if (players_by_id.GetTreeSize() == 0)
        players_by_id.Reset();
    delete p;
    return SUCCESS;
}
StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    if (GroupID <= 0 || ReplacementID <= 0 || GroupID == ReplacementID)
        return INVALID_INPUT;
    if (empty_groups.Exists(GroupID)) // group is empty so just remove it
    {
        // empty_groups.Find(GroupID)->ClearGroup();
        empty_groups.Remove(GroupID);
        return SUCCESS;
    }
    if (!groups.Exists(GroupID)) //couldn`t find groupId
        return FAILURE;
    Group *replacement_group, *current_group;
    if (groups.Exists(ReplacementID))
    {
        replacement_group = groups.Find(ReplacementID).get();
    }
    else if (empty_groups.Exists(ReplacementID))
    {
        replacement_group = empty_groups.Find(ReplacementID).get();
    }
    else //couldn`t find replacementId
        return FAILURE;
    //pull groups from tree
    current_group = groups.Find(GroupID).get();
    /*
    //create and merge trees by id:
    //create arrays by the size of the groups:
    Player **group, **replacement, **merged;
    int *merged_keys;
    int n_group = current_group->GetPlayerById()->GetTreeSize();
    int n_replacement = replacement_group->GetPlayerById()->GetTreeSize();
    group = current_group->GetPlayerById()->GetDataArray();
    replacement = replacement_group->GetPlayerById()->GetDataArray();
    merged = new Player *[n_group + n_replacement];
    merged_keys = new int[n_group + n_replacement];
    //fill arrays with players sorted by Id
    //merge arrays
    int i = 0, j = 0, k = 0;
    while (i < n_group && j < n_replacement)
    {
        if (group[i]->getId() <= replacement[j]->getId())
        {
            merged[k] = group[i];
            merged_keys[k] = group[i]->getId();
            group[i]->setGroup(replacement_group);
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
        group[i]->setGroup(replacement_group);
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
    AVLTree<Player> *merged_tree_by_id = new AVLTree<Player>(merged_keys, merged, merged_size - 1);
    //AVLTree<Player> *merged_tree_by_id = merged_tree_by_id->SortedArrayToAVL(merged_keys, merged, merged_size - 1);
    for (int index = 0; index < n_group; index++)
        group[index] = nullptr;
    delete group;
    for (int index = 0; index < n_replacement; index++)
        replacement[index] = nullptr;
    delete replacement;
    for (int index = 0; index < n_group + n_replacement; index++)
        merged[index] = nullptr;
    delete merged;
    */
    // create and merge trees by level:
    int n1 = current_group->GetPlayerByLevel()->GetTreeSize(),
        n2 = replacement_group->GetPlayerByLevel()->GetTreeSize();
    shared_ptr<AVLTree<shared_ptr<Player>>> *group_levels_array, *replacement_levels_array, *merged_levels_array;
    group_levels_array = current_group->GetPlayerByLevel()->GetDataArray();
    replacement_levels_array = replacement_group->GetPlayerByLevel()->GetDataArray();
    merged_levels_array = new shared_ptr<AVLTree<shared_ptr<Player>>>[n1 + n2];
    int *merged_keys = new int[n1 + n2];
    //merge arrays
    int i1 = 0, i2 = 0, j = 0;
    int l1, l2;
    while (i1 < n1 && i2 < n2)
    {
        l1 = group_levels_array[i1].get()->GetRootData()->get()->getLevel();
        l2 = replacement_levels_array[i2].get()->GetRootData()->get()->getLevel();
        if (l1 < l2)
        {
            merged_levels_array[j] = group_levels_array[i1];
            merged_keys[j] = l1;
            i1++;
        }
        else if (l1 > l2)
        {
            merged_levels_array[j] = replacement_levels_array[i2];
            merged_keys[j] = l2;
            i2++;
        }
        else //trees of the same level, we need to merge them first
        {
            /*
            int n_group_sub = group_by_level[i]->GetTreeSize();
            int n_replacement_sub = replacement_by_level[j]->GetTreeSize();
            Player **group_sub, **replacement_sub, **merged_sub;
            int *merged_keys_sub;
            group_sub = group_by_level[i]->GetDataArray();
            replacement_sub = replacement_by_level[j]->GetDataArray();
            merged_sub = new Player *[n_group_sub + n_replacement_sub];
            merged_keys_sub = new int[n_group_sub + n_replacement_sub];
            //*group_sub = (Player *)malloc(n_group_sub * sizeof(**group_sub));
            //*replacement_sub = (Player *)malloc(n_replacement_sub * sizeof(**replacement_sub));
            //*merged_sub = (Player *)malloc((n_group_sub + n_replacement_sub) * sizeof(**merged_sub));
            //merged_keys_sub = (int *)malloc((n_group_sub + n_replacement_sub) * sizeof(*merged_keys_sub));
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
            AVLTree<Player> *merged_tree_sub = new AVLTree<Player>(merged_keys_sub, merged_sub, merged_sub_size - 1);
            //AVLTree<Player> *merged_tree_sub = merged_tree_sub->SortedArrayToAVL(merged_keys_sub, merged_sub, merged_sub_size - 1);
            for (int index = 0; index < n_group_sub; index++)
                group_sub[index] = nullptr;
            delete group_sub;
            for (int index = 0; index < n_replacement_sub; index++)
                replacement_sub[index] = nullptr;
            delete replacement_sub;
            for (int index = 0; index < n_group_sub + n_replacement_sub; index++)
                merged_sub[index] = nullptr;
            delete merged_sub;
            merged_by_level[k] = merged_tree_sub;
            merged_keys[k] = l1;
            delete merged_keys_sub;
            */
            //create merged tree
            AVLTree<shared_ptr<Player>> tr = MergeTrees<shared_ptr<Player>>(*group_levels_array[i1].get(), *replacement_levels_array[i2].get());
            //put it in shared_ptr
            shared_ptr<AVLTree<shared_ptr<Player>>> sp(&tr);
            //put sp in merged_level_trees
            merged_levels_array[j] = sp;
        }
        j++;
    }
    while (i1 < n1)
    {
        merged_levels_array[j] = group_levels_array[i1];
        merged_keys[j] = group_levels_array[i1].get()->GetRootData()->get()->getLevel();
        i1++;
        j++;
    }
    while (i2 < n2)
    {
        merged_levels_array[j] = replacement_levels_array[i2];
        merged_keys[j] = replacement_levels_array[i2].get()->GetRootData()->get()->getLevel();
        i2++;
        j++;
    }
    AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> merged_tree(merged_keys, merged_levels_array, n1 + n2);
    replacement_group->GetPlayerByLevel()->Reset();
    replacement_group->SetTree(merged_tree);
    if (!groups.Remove(GroupID))
        throw FAILURE_exception();

    delete[] group_levels_array;
    delete[] replacement_levels_array;
    delete[] merged_levels_array;
    delete[] merged_keys;
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
    Player *temp_p = new Player(*p);
    p_tree->RemoveWithoutDelete(temp_p->getId());
    if (p_tree->GetTreeSize() == 0)
    {
        p_tree->Reset();
        players_by_level.Remove(temp_p->getLevel());
    }
    players_by_id.RemoveWithoutDelete(temp_p->getId());
    g->RemovePlayerFromGroup(p);
    p = temp_p;
    p->incrementLevel(LevelIncrease);
    //re-inserting the player to the level trees
    if (players_by_level.Exists(p->getLevel())) //the new level tree already exists
    {
        players_by_level.Find(p->getLevel())->Insert(p->getId(), p);
    }
    else //creating a new player tree for this level
    {
        p_tree = new AVLTree<Player>;
        p_tree->Insert(p->getId(), p);
        players_by_level.Insert(p->getLevel(), p_tree);
    }
    g->AddPlayerToGroup(p);
    players_by_id.Insert(p->getId(), p);
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
    Group *g = groups.Find(GroupID);                                                    // not gonna throw because it is Exists
    AVLTree<AVLTree<Player>> *group_players_level_tree = g->GetPlayerByLevel();         // get the AVLTree of players by level of this group
    AVLTree<Player> *group_highest_level_tree = group_players_level_tree->GetHighest(); // get the highest node in the tree
    if (!group_highest_level_tree)                                                      // there is no players in this group
        return SUCCESS;
    Player *highest_player = group_highest_level_tree->GetLowest(); // get the lowest node in the tree => the highest player
    if (!highest_player)                                            // there is no players in this group
        return SUCCESS;
    *PlayerID = highest_player->getId();
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
    LTRInOrderForGroups<Group>(groups.GetRoot(), &Players, &index, numOfGroups);
    if (index < numOfGroups) // maybe delete Players
    {
        *st = FAILURE;
        return nullptr;
    }
    *st = SUCCESS;
    return Players;
}

int *PlayersManager::GetAllPlayersByLevel_AUX(AVLTree<AVLTree<Player>> *tree, int *numOfPlayers)
{
    *numOfPlayers = tree->GetTreeSize();
    if (*numOfPlayers == 0)
        return nullptr;
    int *Players = (int *)malloc((*numOfPlayers) * sizeof(int));
    int index = 0;
    RTLInOrderForPlayers<AVLTree<Player>>(tree->GetRoot(), &Players, &index);
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
        return GetAllPlayersByLevel_AUX(&players_by_level, numOfPlayers);
    }
    if (!groups.Exists(GroupID)) // the group doesn't exist
    {
        *st = FAILURE;
        return nullptr;
    }
    Group *g = groups.Find(GroupID); // not gonna throw because its Exists
    return GetAllPlayersByLevel_AUX(g->GetPlayerByLevel(), numOfPlayers);
}

void PlayersManager::Quit(PlayersManager *pm)
{
    delete pm;
}