#include "Player.h"

Player::Player(int id, int level, int groupId) : id(id), level(level), groupId(groupId) {}
int Player::getId()
{
    return this->id;
}
int Player::getLevel()
{
    return this->level;
}
int Player::getGroupId()
{
    return this->groupId;
}
int Player::setGroupId(int new_id)
{
    groupId = new_id;
}
void Player::incrementLevel(int increment)
{
    level += increment;
}