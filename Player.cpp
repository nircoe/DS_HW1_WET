#include "Player.h"

Player::Player(int id, int level, Group *group) : id(id), level(level), group(group) {}
int Player::getId()
{
    if(this != 0)
        return this->id;
    return -1;
}
int Player::getLevel()
{
    if (this != 0)
        return this->level;
    return -1;
}
Group* Player::getGroup()
{
    if (this != 0)
        return this->group;
    return nullptr;
}
void Player::setGroup(Group *new_group)
{
    group = new_group;
}
void Player::incrementLevel(int increment)
{
    level += increment;
}