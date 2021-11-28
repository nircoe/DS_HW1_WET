#include "Group.h"

Group::Group(int id) : id(id)
{
    players_by_id = AVLTree<Player>();
    players_by_level = AVLTree<Player>();
}
StatusType Group::AddPlayerToGroup(Player p)
{
    if (players_by_id.Find(players_by_id.GetRoot(), p.getId()))
        players_by_id.Insert(p);
    players_by_level.Insert(p);
}
