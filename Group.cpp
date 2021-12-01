#include "Group.h"
#include "AVLTree.h"

Group::Group(int id) : id(id)
{
    players_by_id = AVLTree<Player>();
    players_by_level = AVLTree<AVLTree<Player>>();
}
StatusType Group::AddPlayerToGroup(Player p)
{
    if (players_by_id.Exists(p.getId()))
        return FAILURE;
    players_by_id.Insert(p.getId(), p);
    AVLTree<Player> p_tree;
    if (players_by_level.Exists(p.getLevel()))
    {
        p_tree = players_by_level.Find(p.getLevel());
    }
    else
    {
        p_tree = AVLTree<Player>();
        players_by_level.Insert(p.getLevel(), p_tree);
    }
    p_tree.Insert(p.getId(), p);
    return SUCCESS;
}
StatusType Group::RemovePlayerFromGroup(Player p)
{
    if (!players_by_id.Exists(p.getId()))
        return FAILURE;
    players_by_id.Remove(p.getId());
    AVLTree<Player> p_tree = players_by_level.Find(p.getLevel());
    p_tree.Remove(p.getId());
    if (p_tree.IsEmpty()) // no players in this level - remove the node
    {
        players_by_level.Remove(p.getLevel());
    }
    return SUCCESS;
}
