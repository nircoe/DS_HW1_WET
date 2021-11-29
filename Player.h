#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    int id;
    int level;
    int groupId;

public:
    Player(int id, int level, int groupId);
    Player(const Player &) = default;
    Player() = delete;
    ~Player() = default;

    int getId();
    int getLevel();
    int getGroupId();
    int setGroupId(int new_id);
    void incrementLevel(int increment);
};

#endif