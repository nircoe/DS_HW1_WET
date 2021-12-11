#include "Group.h"
#include "AVLTree.h"

Group::Group(int g_id) : group_id(g_id) , group_size(0)
{
    group_players_by_level = AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>>();
}
Group::Group(int g_id, int g_size, AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> &g_players_by_level)
{
    group_id = g_id;
    group_size = g_size;
    group_players_by_level = g_players_by_level;
}
int Group::GetId()
{
    return (this != 0) ? this->group_id : -1;
}
int Group::GetSize()
{
    return (this != 0) ? this->group_size : -1;
}
StatusType Group::AddPlayerToGroup(shared_ptr<Player> p)
{
    int p_level = p.get()->GetLevel();
    shared_ptr<AVLTree<shared_ptr<Player>>> p_tree;
    if (group_players_by_level.Exists(p_level))  //this level tree exists
        p_tree = group_players_by_level.Find(p_level);
    else
    {
        p_tree = make_shared<AVLTree<shared_ptr<Player>>>();
        if (!group_players_by_level.Insert(p_level, p_tree)) // if Insert return false => allocation error
        {
            p_tree.reset();
            return ALLOCATION_ERROR;
        }
    }
    if (!p_tree.get()->Insert(p.get()->GetId(), p)) // if Insert return false => allocation error
        return ALLOCATION_ERROR;
    this->group_size++;
    return SUCCESS;
}
StatusType Group::RemovePlayerFromGroup(int p_id, int p_level)
{
    shared_ptr<AVLTree<shared_ptr<Player>>> p_tree = group_players_by_level.Find(p_level); // not gonna throw because it is Exists
    p_tree.get()->Remove(p_id);                               // doesn't matter if return true or false
    if (p_tree.get()->GetTreeSize() == 0)  //no more players in this level tree, so we can remove it
    {
        group_players_by_level.Remove(p_level);
    }
    this->group_size--;
    return SUCCESS;
}
AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> *Group::GetPlayerByLevel()
{
    return &(this->group_players_by_level);
}
void Group::SetTree(AVLTree<shared_ptr<AVLTree<shared_ptr<Player>>>> &by_level, int new_size)
{
    this->group_size = new_size;
    this->group_players_by_level = by_level;
}