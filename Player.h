#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    int id;
    int level;

public:
    Player(int id, int level);
    Player(const Player &) = default;
    Player() = delete;
    ~Player() = default;

    int getId();
    int getLevel();
    void incrementLevel(int increment);
};

#endif