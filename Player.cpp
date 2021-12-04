#include "Player.h"

Player::Player(int id, int level, Group *group) : id(id), level(level), group(group) {}
int Player::getId()
{
    return this->id;
}
int Player::getLevel()
{
    return this->level;
}
Group* Player::getGroup()
{
    return this->group;
}
void Player::setGroup(Group *new_group)
{
    group = new_group;
}
void Player::incrementLevel(int increment)
{
    level += increment;
}