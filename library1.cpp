#include "library1.h"
#include "PlayersManager.h"

void *Init()
{
    try
    {
        PlayersManager *DS = new PlayersManager();
        return (void *)(DS);
    }
    catch (const std::exception &e) { return NULL; }
}
StatusType AddGroup(void *DS, int GroupID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    try { return ((PlayersManager *)DS)->AddGroup(GroupID); }
    catch (const std::bad_alloc &e) { return ALLOCATION_ERROR; }
    catch (const std::exception &e) { return FAILURE; }  
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int Level)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    try { return ((PlayersManager *)DS)->AddPlayer(PlayerID, GroupID, Level); }
    catch(const std::bad_alloc& e) { return ALLOCATION_ERROR; }
    catch(const std::exception& e) { return FAILURE; }
}

StatusType RemovePlayer(void *DS, int PlayerID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    try { return ((PlayersManager *)DS)->RemovePlayer(PlayerID); }
    catch(const std::bad_alloc& e) { return ALLOCATION_ERROR; }
    catch(const std::exception& e) { return FAILURE; }
}

StatusType ReplaceGroup(void *DS, int GroupID, int ReplacementID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    try { return ((PlayersManager *)DS)->ReplaceGroup(GroupID, ReplacementID); }
    catch(const std::bad_alloc& e) { return ALLOCATION_ERROR; }
    catch(const std::exception& e) { return FAILURE; }
}

StatusType IncreaseLevel(void *DS, int PlayerID, int LevelIncrease)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    try { return ((PlayersManager *)DS)->IncreaseLevel(PlayerID, LevelIncrease); }
    catch(const std::bad_alloc& e) { return ALLOCATION_ERROR; }
    catch(const std::exception& e) { return FAILURE; }
}

StatusType GetHighestLevel(void *DS, int GroupID, int *PlayerID)
{
    if (DS == nullptr)
        return INVALID_INPUT;
    try { return ((PlayersManager *)DS)->GetHighestLevel(GroupID, PlayerID); }
    catch (const std::bad_alloc &e) { return ALLOCATION_ERROR; }
    catch (const std::exception &e) { return FAILURE; }
}

StatusType GetGroupsHighestLevel(void *DS, int numOfGroups, int **Players)
{
    if (DS == nullptr || Players == nullptr)
        return INVALID_INPUT;
    try
    {
        StatusType st;
        *Players = ((PlayersManager *)DS)->GetGroupsHighestLevel(numOfGroups, &st);
        return st;
    }
    catch (const std::bad_alloc &e) { return ALLOCATION_ERROR; }
    catch (const std::exception &e) { return FAILURE; }
}

StatusType GetAllPlayersByLevel(void *DS, int GroupID, int **Players, int *numOfPlayers)
{
    if (DS == nullptr || Players == nullptr)
        return INVALID_INPUT;
    try
    {
        StatusType st;
        *Players = ((PlayersManager *)DS)->GetAllPlayersByLevel(GroupID, numOfPlayers, &st);
        return st;
    }
    catch (const std::bad_alloc &e) { return ALLOCATION_ERROR; }
    catch (const std::exception &e) { return FAILURE; }
}

void Quit(void **DS)
{
    if(DS == NULL) return;
    PlayersManager *pm = (PlayersManager *)(*DS);
    pm->Quit(pm);
    *DS = nullptr;
}