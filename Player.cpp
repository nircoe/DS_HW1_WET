#include "Player.h"

Player::Player(int id, int level) : id(id), level(level) {}
int Player::getId()
{
    return this->id;
}
int Player::getLevel()
{
    return this->level;
}
void Player::incrementLevel(int increment)
{
    this->level += increment;
}