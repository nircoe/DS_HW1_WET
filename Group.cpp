#include "Group.h"
#include "AVLTree.h"

Group::Group(int id) : id(id) , size(0)
{
    group_players_by_id = new AVLTree<Player>();
    group_players_by_level = new AVLTree<AVLTree<Player>>();
    group_players_by_id->Reset();
    group_players_by_level->Reset();
}
Group::~Group()
{
    try
    {
        delete this->group_players_by_id;
        delete this->group_players_by_level;
        this->id = -1;
        this->size = -1;
    }
    catch(const std::exception& e) { }
}
int Group::GetId()
{
    return id;
}
StatusType Group::AddPlayerToGroup(Player *p)
{
    if(size == 0)
    {
        group_players_by_id->Reset();
        group_players_by_level->Reset();
    }
    if (group_players_by_id->Exists(p->getId()))
        return FAILURE;
    if (!group_players_by_id->Insert(p->getId(), p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    AVLTree<Player> *p_tree;
    if (group_players_by_level->Exists(p->getLevel()))  //this level tree exists
        p_tree = group_players_by_level->Find(p->getLevel()); // not gonna throw because its Exists
    else
    {
        p_tree = new AVLTree<Player>();
        if (!group_players_by_level->Insert(p->getLevel(), p_tree)) // if Insert return false => allocation error
        {
            delete p_tree;
            return ALLOCATION_ERROR;
        }
    }
    if (!p_tree->Insert(p->getId(), p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    size++;
    return SUCCESS;
}
StatusType Group::RemovePlayerFromGroup(Player *p)
{
    if (!group_players_by_id->Exists(p->getId()) /* || !players_by_level.Exists(p.getLevel())*/)
        return FAILURE;
    int p_level = p->getLevel(), p_id = p->getId();                                    // if the player exist in the id's tree so the level tree should exist too
    group_players_by_id->Remove(p_id);                        // not gonna return false because p.getId Exists in the tree (so the tree is not empty)
    AVLTree<Player> *p_tree = group_players_by_level->Find(p_level); // not gonna throw because it is Exists
    p_tree->Remove(p_id);                                     // doesn't matter if return true or false
    if (p_tree->IsEmpty())  //no more players in this level tree, so we can remove it
    {
        p_tree->Reset();
        group_players_by_level->Remove(p_level);
    }
    size--;
    if (size == 0)
    {
        group_players_by_id->Reset();
        group_players_by_level->Reset();
    }
    return SUCCESS;
}
StatusType Group::RemovePlayerFromGroupWithoutDelete(Player *p)
{
    if (!group_players_by_id->Exists(p->getId()) /* || !players_by_level.Exists(p.getLevel())*/)
        return FAILURE;
    int p_level = p->getLevel(), p_id = p->getId();                  // if the player exist in the id's tree so the level tree should exist too
    group_players_by_id->RemoveWithoutDelete(p_id);                               // not gonna return false because p.getId Exists in the tree (so the tree is not empty)
    AVLTree<Player> *p_tree = group_players_by_level->Find(p_level); // not gonna throw because it is Exists
    p_tree->RemoveWithoutDelete(p_id);                                            // doesn't matter if return true or false
    if (p_tree->IsEmpty())                                           //no more players in this level tree, so we can remove it
    {
        p_tree->Reset();
        group_players_by_level->RemoveWithoutDelete(p_level);
    }
    size--;
    if (size == 0)
    {
        group_players_by_id->Reset();
        group_players_by_level->Reset();
    }
    return SUCCESS;
}
AVLTree<Player> *Group::GetPlayerById()
{
    /*if (this != 0)
        return &players_by_id;
    return nullptr;*/
    return (this != 0) ? group_players_by_id : nullptr;
}

AVLTree<AVLTree<Player>> *Group::GetPlayerByLevel()
{
    /*if (this != 0)
        return &players_by_level;
    return nullptr;*/
    return (this != 0) ? group_players_by_level : nullptr;
}
void Group::SetTrees(AVLTree<Player> *by_id, AVLTree<AVLTree<Player>> *by_level)
{
    this->size = by_id->GetTreeSize();
    this->group_players_by_id = by_id;
    this->group_players_by_level = by_level;
}

void Group::ClearGroup()
{
    Player **players = this->group_players_by_id->GetDataArray();
    for (int i = 0; i < size;i++)
        RemovePlayerFromGroupWithoutDelete(players[i]);
    this->group_players_by_id->Reset();
    this->group_players_by_level->Reset();
    this->id = -1;
    this->size = 0;
}
