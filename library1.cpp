#include "library1.h"
#include "PlayersManager.h"

void *Init()
{
    try
    {
        PlayersManager *DS = new PlayersManager();
        return (void *)(DS);
    }
    catch (const std::exception &e)
    {
        return NULL;
    }
}
StatusType AddGroup(void *DS, int GroupID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    return ((PlayersManager *)DS)->AddGroup(GroupID);
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int Level)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    return ((PlayersManager *)DS)->AddPlayer(PlayerID, GroupID, Level);
}

StatusType RemovePlayer(void *DS, int PlayerID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    return ((PlayersManager *)DS)->RemovePlayer(PlayerID);
}

StatusType ReplaceGroup(void *DS, int GroupID, int ReplacementID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    return ((PlayersManager *)DS)->ReplaceGroup(GroupID, ReplacementID);
}

StatusType IncreaseLevel(void *DS, int PlayerID, int LevelIncrease)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    return ((PlayersManager *)DS)->IncreaseLevel(PlayerID, LevelIncrease);
}

StatusType GetHighestLevel(void *DS, int GroupID, int *PlayerID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    return ((PlayersManager *)DS)->GetHighestLevel(GroupID, PlayerID);
}

StatusType GetGroupsHighestLevel(void *DS, int numOfGroups, int **Players)
{
    if (DS == nullptr || Players == nullptr)
        return INVALID_INPUT;
    StatusType st;
    *Players = ((PlayersManager *)DS)->GetGroupsHighestLevel(numOfGroups, &st);
    return st;
}

StatusType GetAllPlayersByLevel(void *DS, int GroupID, int **Players, int *numOfPlayers)
{
    if (DS == nullptr || Players == nullptr)
        return INVALID_INPUT;
    StatusType st;
    *Players = ((PlayersManager *)DS)->GetAllPlayersByLevel(GroupID, numOfPlayers, &st);
    return st;
}

void Quit(void **DS)
{
    PlayersManager *pm = (PlayersManager *)(*DS);
    pm->Quit(pm);
    *DS = nullptr;
}