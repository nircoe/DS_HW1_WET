#include "Group.h"
#include "AVLTree.h"

Group::Group(int id) : id(id) , size(0)
{
    players_by_id = new AVLTree<Player>();
    players_by_level = new AVLTree<AVLTree<Player>>();
}
Group::~Group()
{
    try
    {
        delete this->players_by_id;
        delete this->players_by_level;
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
        players_by_id->Reset();
        players_by_level->Reset();
    }
    if (players_by_id->Exists(p->getId()))
        return FAILURE;
    if (!players_by_id->Insert(p->getId(), p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    AVLTree<Player> *p_tree;
    if (players_by_level->Exists(p->getLevel()))       //this level tree exists
        p_tree = players_by_level->Find(p->getLevel()); // not gonna throw because its Exists
    else
    {
        p_tree = new AVLTree<Player>();
        if (!players_by_level->Insert(p->getLevel(), p_tree)) // if Insert return false => allocation error
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
    if (!players_by_id->Exists(p->getId()) /* || !players_by_level.Exists(p.getLevel())*/)
        return FAILURE;                                             // if the player exist in the id's tree so the level tree should exist too
    players_by_id->Remove(p->getId());                              // not gonna return false because p.getId Exists in the tree (so the tree is not empty)
    AVLTree<Player> *p_tree = players_by_level->Find(p->getLevel()); // not gonna throw because it is Exists
    p_tree->Remove(p->getId());                                     // doesn't matter if return true or false
    if (p_tree->IsEmpty())                                          //no more players in this level tree, so we can remove it
        players_by_level->Remove(p->getLevel());
    size--;
    if (size == 0)
    {
        players_by_id->Reset();
        players_by_level->Reset();
    }
    return SUCCESS;
}

AVLTree<Player> *Group::GetPlayerById()
{
    /*if (this != 0)
        return &players_by_id;
    return nullptr;*/
    return (this != 0) ? players_by_id : nullptr;
}

AVLTree<AVLTree<Player>> *Group::GetPlayerByLevel()
{
    /*if (this != 0)
        return &players_by_level;
    return nullptr;*/
    return (this != 0) ? players_by_level : nullptr;
}
void Group::SetTrees(AVLTree<Player> *by_id, AVLTree<AVLTree<Player>> *by_level)
{
    this->players_by_id = by_id;
    this->players_by_level = by_level;
}
