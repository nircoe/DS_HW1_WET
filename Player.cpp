#include "Player.h"

Player::Player() : player_id(-1), player_level(-1), player_group(nullptr) {}
Player::Player(int p_id, int p_level, shared_ptr<Group> p_group) : player_id(p_id), player_level(p_level), player_group(p_group) {}
int Player::GetId()
{
    return (this != 0) ? this->player_id : -1;
}
int Player::GetLevel()
{
    return (this != 0) ? this->player_level : -1;
}
shared_ptr<Group> Player::GetGroup()
{
    return (this != 0) ? this->player_group : nullptr;
}
void Player::SetGroup(shared_ptr<Group> new_group)
{
    this->player_group = new_group;
}
void Player::IncrementLevel(int increment)
{
    this->player_level += increment;
}