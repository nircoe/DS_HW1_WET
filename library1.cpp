#include "library1.h"
#include "PlayersManager.h"

void *Init()
{
    PlayersManager *DS = new PlayersManager();
    return (void *)DS;
}
StatusType AddGroup(void *DS, int GroupID)
{
    return ((PlayersManager *)DS)->AddGroup(GroupID);
}

