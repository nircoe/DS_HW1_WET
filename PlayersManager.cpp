#include "PlayersManager.h"
#include <iostream>

PlayersManager::PlayersManager() : players_by_id(AVLTree<shared_ptr<Player>>()),
                                   players_by_level(AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>>()),
                                   groups(AVLTree<shared_ptr<Group>>()),
                                   empty_groups(AVLTree<shared_ptr<Group>>()) {}
PlayersManager::~PlayersManager()
{
    DeletePlayersByIdTree<shared_ptr<Player>>(this->players_by_id.GetRoot());
    //DeletePlayersByLevelTree<shared_ptr<AVLTree<shared_ptr<Player>>>>(this->players_by_level.GetRoot());
    DeleteGroupsTree<shared_ptr<Group>>(this->groups.GetRoot());
    DeleteGroupsTree<shared_ptr<Group>>(this->empty_groups.GetRoot());
}
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
    if(!players_by_id.Insert(PlayerID,p))
        return ALLOCATION_ERROR;
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
    StatusType st = g.get()->RemovePlayerFromGroup(PlayerID, p.get()->GetLevel());
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
    if (p_tree.get()->GetTreeSize() == 0)
        players_by_level.Remove(p.get()->GetLevel());
    if (!players_by_id.Remove(p.get()->GetId()))
        return FAILURE;
    p.reset();
    return SUCCESS;
}
static void SetReplacedGroup(shared_ptr<AVLTree<shared_ptr<Player>>> level_tree, shared_ptr<Group> replacement_group)
{
    shared_ptr<Player> *players = level_tree.get()->GetDataArray();
    int n = level_tree.get()->GetTreeSize();
    for (int i = 0; i < n; i++)
        players[i].get()->SetGroup(replacement_group);
    delete[] players;
}

StatusType PlayersManager::ReplaceGroup(int GroupID, int ReplacementID)
{
    if (GroupID <= 0 || ReplacementID <= 0 || GroupID == ReplacementID)
        return INVALID_INPUT;
    if (empty_groups.Exists(GroupID)) // group is empty so just remove it
    {
        empty_groups.Remove(GroupID);
        return SUCCESS;
    }
    if (!groups.Exists(GroupID)) //couldn`t find groupId
        return FAILURE;
    shared_ptr<Group> replacement_group, current_group;
    if (groups.Exists(ReplacementID))
    {
        replacement_group = groups.Find(ReplacementID);
    }
    else if (empty_groups.Exists(ReplacementID))
    {
        replacement_group = empty_groups.Find(ReplacementID);
    }
    else //couldn`t find replacementId
        return FAILURE;
    current_group = groups.Find(GroupID); //pull groups from tree
    // create and merge trees by level:
    int n1 = current_group.get()->GetPlayerByLevel()->GetTreeSize(),
        n2 = replacement_group.get()->GetPlayerByLevel()->GetTreeSize();
    shared_ptr<AVLTree<shared_ptr<Player>>> *group_levels_array, *replacement_levels_array, *merged_levels_array;
    group_levels_array = current_group.get()->GetPlayerByLevel()->GetDataArray();
    replacement_levels_array = replacement_group.get()->GetPlayerByLevel()->GetDataArray();
    merged_levels_array = new shared_ptr<AVLTree<shared_ptr<Player>>>[n1 + n2];
    int *merged_keys = new int[n1 + n2];
    //merge arrays
    int i1 = 0, i2 = 0, j = 0;
    int l1, l2;
    while (i1 < n1 && i2 < n2)
    {
        l1 = group_levels_array[i1].get()->GetRootData().get()->GetLevel();
        l2 = replacement_levels_array[i2].get()->GetRootData().get()->GetLevel();
        if (l1 < l2)
        {
            merged_levels_array[j] = group_levels_array[i1];
            merged_keys[j] = l1;
            SetReplacedGroup(merged_levels_array[j], replacement_group);
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
            //create merged tree
            AVLTree<shared_ptr<Player>> *tr = MergeTrees<shared_ptr<Player>>(*group_levels_array[i1].get(), *replacement_levels_array[i2].get());
            //put it in shared_ptr
            shared_ptr<AVLTree<shared_ptr<Player>>> sp = make_shared<AVLTree<shared_ptr<Player>>>(*tr);
            delete tr;
            //put sp in merged_level_trees
            merged_levels_array[j] = sp;
            merged_keys[j] = l1;
            SetReplacedGroup(merged_levels_array[j], replacement_group);
        }
        j++;
    }
    while (i1 < n1)
    {
        merged_levels_array[j] = group_levels_array[i1];
        merged_keys[j] = group_levels_array[i1].get()->GetRootData().get()->GetLevel();
        SetReplacedGroup(merged_levels_array[j], replacement_group);
        i1++;
        j++;
    }
    while (i2 < n2)
    {
        merged_levels_array[j] = replacement_levels_array[i2];
        merged_keys[j] = replacement_levels_array[i2].get()->GetRootData().get()->GetLevel();
        i2++;
        j++;
    }
    AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> merged_tree(merged_keys, merged_levels_array, n1 + n2);
    int g_size = current_group.get()->GetSize() + replacement_group.get()->GetSize();
    replacement_group->SetTree(merged_tree, g_size);
    delete[] group_levels_array;
    delete[] replacement_levels_array;
    delete[] merged_levels_array;
    delete[] merged_keys;
    if (!groups.Remove(GroupID))
        return FAILURE;
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
    if (p_tree.get()->GetTreeSize() == 0)
    {
        players_by_level.Remove(p_level);
        p_tree.reset();
    }
    players_by_id.Remove(p_id);
    g.get()->RemovePlayerFromGroup(p_id, p_level);
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
        if (!players_by_level.Insert(p_level, new_p_tree))
            return ALLOCATION_ERROR;
        if(!new_p_tree.get()->Insert(p_id, p))
            return ALLOCATION_ERROR;
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
        if(players_by_level.GetTreeSize() == 0)
            return SUCCESS;
        shared_ptr<AVLTree<shared_ptr<Player>>> highest_level_tree = players_by_level.GetHighest();
        if (!highest_level_tree.get()) // no players in the system so *PlayerID == -1
            return SUCCESS;
        shared_ptr<Player> highest_player = highest_level_tree.get()->GetLowest();
        if (!highest_player) // no players in the system so *PlayerID == -1
            return SUCCESS;
        *PlayerID = highest_player.get()->GetId(); // return the lowest id's player in the highest level's tree
        return SUCCESS;
    }
    if(empty_groups.Exists(GroupID))
        return SUCCESS;
    if (!groups.Exists(GroupID)) // the group doesn't exist
        return FAILURE;
    shared_ptr<Group> g = groups.Find(GroupID);                                                               // not gonna throw because it is Exists
    AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> *group_players_level_tree = g.get()->GetPlayerByLevel();  // get the AVLTree of players by level of this group
    shared_ptr<AVLTree<shared_ptr<Player>>> group_highest_level_tree = group_players_level_tree->GetHighest(); // get the highest node in the tree
    if (!group_highest_level_tree.get())                                                                      // there is no players in this group
        return SUCCESS;
    shared_ptr<Player> highest_player = group_highest_level_tree.get()->GetLowest(); // get the lowest node in the tree => the highest player
    if (!highest_player.get())                                                             // there is no players in this group
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

int *PlayersManager::GetAllPlayersByLevel_AUX(AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> &tree, int numOfPlayers)
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
    return GetAllPlayersByLevel_AUX(*(g.get()->GetPlayerByLevel()), *numOfPlayers);
}

void PlayersManager::Quit(PlayersManager *pm)
{
    delete pm;
}