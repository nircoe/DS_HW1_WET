/***************************************************************************/
/*                                                                         */
/* 234218 Data DSs 1, Winter 2021-2022                                     */
/* Homework : Wet 1                                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* File Name : main1.cpp                                                   */
/*                                                                         */
/* Holds the "int main()" function and the parser of the shell's           */
/* command line.                                                           */
/***************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library1.h"
#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

    /* The command's strings */
    typedef enum
    {
        NONE_CMD = -2,
        COMMENT_CMD = -1,
        INIT_CMD = 0,
        ADDGROUP_CMD = 1,
        ADDPLAYER_CMD = 2,
        REMOVEPLAYER_CMD = 3,
        REPLACEGROUP_CMD = 4,
        INCREASELEVEL_CMD = 5,
        GETHIGHESTLEVEL_CMD = 6,
        GETALLPLAYERS_CMD = 7,
        GETGROUPSHIGHEST_CMD = 8,
        QUIT_CMD = 9
    } commandType;

    static const int numActions = 10;
    static const char *commandStr[] = {
        "Init",
        "AddGroup",
        "AddPlayer",
        "RemovePlayer",
        "ReplaceGroup",
        "IncreaseLevel",
        "GetHighestLevel",
        "GetAllPlayersByLevel",
        "GetGroupsHighestLevel",
        "Quit"};

    static const char *ReturnValToStr(int val)
    {
        switch (val)
        {
        case SUCCESS:
            return "SUCCESS";
        case ALLOCATION_ERROR:
            return "ALLOCATION_ERROR";
        case FAILURE:
            return "FAILURE";
        case INVALID_INPUT:
            return "INVALID_INPUT";
        default:
            return "";
        }
    }

/* we assume maximum string size is not longer than 256  */
#define MAX_STRING_INPUT_SIZE (255)
#define MAX_BUFFER_SIZE (255)

#define StrCmp(Src1, Src2) (strncmp((Src1), (Src2), strlen(Src1)) == 0)

    typedef enum
    {
        error_free,
        error
    } errorType;
    static errorType parser(const char *const command);

#define ValidateRead(read_parameters, required_parameters, ErrorString) \
    if ((read_parameters) != (required_parameters))                     \
    {                                                                   \
        printf(ErrorString);                                            \
        return error;                                                   \
    }

    static bool isInit = false;

    /***************************************************************************/
    /* main                                                                    */
    /***************************************************************************/

    int main(int argc, const char **argv)
    {
        char buffer[MAX_STRING_INPUT_SIZE];

        // Reading commands
        while (fgets(buffer, MAX_STRING_INPUT_SIZE, stdin) != NULL)
        {
            fflush(stdout);
            if (parser(buffer) == error)
                break;
        };
        return 0;
    }

    /***************************************************************************/
    /* Command Checker                                                         */
    /***************************************************************************/

    static commandType CheckCommand(const char *const command,
                                    const char **const command_arg)
    {
        if (command == NULL || strlen(command) == 0 || StrCmp("\n", command))
            return (NONE_CMD);
        if (StrCmp("#", command))
        {
            if (strlen(command) > 1)
                printf("%s", command);
            return (COMMENT_CMD);
        };
        for (int index = 0; index < numActions; index++)
        {
            if (StrCmp(commandStr[index], command))
            {
                *command_arg = command + strlen(commandStr[index]) + 1;
                return ((commandType)index);
            };
        };
        return (NONE_CMD);
    }

    /***************************************************************************/
    /* Commands Functions                                                      */
    /***************************************************************************/

    static errorType OnInit(void **DS, const char *const command);
    static errorType OnAddGroup(void *DS, const char *const command);
    static errorType OnAddPlayer(void *DS, const char *const command);
    static errorType OnRemovePlayer(void *DS, const char *const command);
    static errorType OnReplaceGroup(void *DS, const char *const command);
    static errorType OnIncreaseLevel(void *DS, const char *const command);
    static errorType OnGetHighestLevel(void *DS, const char *const command);
    static errorType OnGetAllPlayersByLevel(void *DS, const char *const command);
    static errorType OnGetGroupsHighestLevel(void *DS, const char *const command);
    static errorType OnQuit(void **DS, const char *const command);

    /***************************************************************************/
    /* Parser                                                                  */
    /***************************************************************************/

    static errorType parser(const char *const command)
    {
        static void *DS = NULL; /* The general data structure */
        const char *command_args = NULL;
        errorType rtn_val = error;

        commandType command_val = CheckCommand(command, &command_args);

        switch (command_val)
        {

        case (INIT_CMD):
            rtn_val = OnInit(&DS, command_args);
            break;
        case (ADDGROUP_CMD):
            rtn_val = OnAddGroup(DS, command_args);
            break;
        case (ADDPLAYER_CMD):
            rtn_val = OnAddPlayer(DS, command_args);
            break;
        case (REMOVEPLAYER_CMD):
            rtn_val = OnRemovePlayer(DS, command_args);
            break;
        case (REPLACEGROUP_CMD):
            rtn_val = OnReplaceGroup(DS, command_args);
            break;
        case (INCREASELEVEL_CMD):
            rtn_val = OnIncreaseLevel(DS, command_args);
            break;
        case (GETHIGHESTLEVEL_CMD):
            rtn_val = OnGetHighestLevel(DS, command_args);
            break;
        case (GETALLPLAYERS_CMD):
            rtn_val = OnGetAllPlayersByLevel(DS, command_args);
            break;
        case (GETGROUPSHIGHEST_CMD):
            rtn_val = OnGetGroupsHighestLevel(DS, command_args);
            break;
        case (QUIT_CMD):
            rtn_val = OnQuit(&DS, command_args);
            break;

        case (COMMENT_CMD):
            rtn_val = error_free;
            break;
        case (NONE_CMD):
            rtn_val = error;
            break;
        default:
            assert(false);
            break;
        };
        return (rtn_val);
    }

    /***************************************************************************/
    /* OnInit                                                                  */
    /***************************************************************************/
    static errorType OnInit(void **DS, const char *const command)
    {
        if (isInit)
        {
            printf("Init was already called.\n");
            return (error_free);
        };
        isInit = true;

        *DS = Init();
        if (*DS == NULL)
        {
            printf("Init failed.\n");
            return error;
        };
        printf("Init done.\n");

        return error_free;
    }

    /***************************************************************************/
    /* OnAddGroup                                                             */
    /***************************************************************************/
    static errorType OnAddGroup(void *DS, const char *const command)
    {
        int groupID;
        ValidateRead(sscanf(command, "%d", &groupID), 1, "AddGroup failed.\n");
        StatusType res = AddGroup(DS, groupID);

        if (res != SUCCESS)
        {
            printf("AddGroup: %s\n", ReturnValToStr(res));
            return error_free;
        }
        else
        {
            printf("AddGroup: %s\n", ReturnValToStr(res));
        }

        return error_free;
    }

    /***************************************************************************/
    /* OnAddPlayer                                                          */
    /***************************************************************************/
    static errorType OnAddPlayer(void *DS, const char *const command)
    {
        int playerID;
        int groupID;
        int level;
        ValidateRead(
            sscanf(command, "%d %d %d", &playerID, &groupID, &level),
            3, "AddPlayer failed.\n");
        StatusType res = AddPlayer(DS, playerID, groupID, level);

        if (res != SUCCESS)
        {
            printf("AddPlayer: %s\n", ReturnValToStr(res));
            return error_free;
        }

        printf("AddPlayer: %s\n", ReturnValToStr(res));
        return error_free;
    }

    /***************************************************************************/
    /* OnRemovePlayer                                                            */
    /***************************************************************************/
    static errorType OnRemovePlayer(void *DS, const char *const command)
    {
        int playerID;
        ValidateRead(sscanf(command, "%d", &playerID), 1,
                     "RemovePlayer failed.\n");
        StatusType res = RemovePlayer(DS, playerID);
        if (res != SUCCESS)
        {
            printf("RemovePlayer: %s\n", ReturnValToStr(res));
            return error_free;
        }

        printf("RemovePlayer: %s\n", ReturnValToStr(res));
        return error_free;
    }

    /***************************************************************************/
    /* OnReplaceGroup                                                            */
    /***************************************************************************/
    static errorType OnReplaceGroup(void *DS, const char *const command)
    {
        int groupID;
        int replacementID;
        ValidateRead(sscanf(command, "%d %d", &groupID, &replacementID), 2,
                     "ReplaceGroup failed.\n");
        StatusType res = ReplaceGroup(DS, groupID, replacementID);

        if (res != SUCCESS)
        {
            printf("ReplaceGroup: %s\n", ReturnValToStr(res));
            return error_free;
        }

        printf("ReplaceGroup: %s\n", ReturnValToStr(res));
        return error_free;
    }

    /***************************************************************************/
    /* OnIncreaseLevel                                                         */
    /***************************************************************************/
    static errorType OnIncreaseLevel(void *DS, const char *const command)
    {
        int playerID;
        int levelIncrease;
        ValidateRead(sscanf(command, "%d %d", &playerID, &levelIncrease), 2,
                     "IncreaseLevel failed.\n");
        StatusType res = IncreaseLevel(DS, playerID, levelIncrease);

        if (res != SUCCESS)
        {
            printf("IncreaseLevel: %s\n", ReturnValToStr(res));
            return error_free;
        }

        printf("IncreaseLevel: %s\n", ReturnValToStr(res));
        return error_free;
    }

    /***************************************************************************/
    /* OnGetHighestLevel                                                         */
    /***************************************************************************/
    static errorType OnGetHighestLevel(void *DS, const char *const command)
    {
        int groupID;
        ValidateRead(sscanf(command, "%d", &groupID), 1, "GetHighestLevel failed.\n");
        int playerID;
        StatusType res = GetHighestLevel(DS, groupID, &playerID);

        if (res != SUCCESS)
        {
            printf("GetHighestLevel: %s\n", ReturnValToStr(res));
            return error_free;
        }

        cout << "Highest level player is: " << playerID << endl;
        return error_free;
    }

    /***************************************************************************/
    /* OnGetAllPlayersByLevel                                                        */
    /***************************************************************************/

    void PrintAll(int *playerIDs, int numOfPlayers)
    {
        if (numOfPlayers > 0)
        {
            cout << "Rank	||	Player" << endl;
        }

        for (int i = 0; i < numOfPlayers; i++)
        {
            cout << i + 1 << "\t||\t" << playerIDs[i] << endl;
        }
        cout << "and there are no more players!" << endl;

        free(playerIDs);
    }

    static errorType OnGetAllPlayersByLevel(void *DS, const char *const command)
    {
        int groupID;
        ValidateRead(sscanf(command, "%d", &groupID), 1,
                     "GetAllPlayersByLevel failed.\n");
        int *playerIDs;
        int numOfPlayers;
        StatusType res = GetAllPlayersByLevel(DS, groupID, &playerIDs, &numOfPlayers);

        if (res != SUCCESS)
        {
            printf("GetAllPlayersByLevel: %s\n", ReturnValToStr(res));
            return error_free;
        }

        PrintAll(playerIDs, numOfPlayers);
        return error_free;
    }

    /***************************************************************************/
    /* OnGetGroupsHighestLevel                                                        */
    /***************************************************************************/

    void PrintGroupsHighest(int *playerIDs, int numOfGroups)
    {
        if (numOfGroups > 0)
        {
            cout << "GroupIndex	||	Player" << endl;
        }

        for (int i = 0; i < numOfGroups; i++)
        {
            cout << i + 1 << "\t||\t" << playerIDs[i] << endl;
        }
        cout << "and there are no more players!" << endl;

        free(playerIDs);
    }

    static errorType OnGetGroupsHighestLevel(void *DS, const char *const command)
    {
        int numOfGroups;
        ValidateRead(sscanf(command, "%d", &numOfGroups), 1,
                     "GetGroupsHighestLevel failed.\n");
        int *playerIDs;
        StatusType res = GetGroupsHighestLevel(DS, numOfGroups, &playerIDs);

        if (res != SUCCESS)
        {
            printf("GetGroupsHighestLevel: %s\n", ReturnValToStr(res));
            return error_free;
        }

        PrintGroupsHighest(playerIDs, numOfGroups);
        return error_free;
    }

    /***************************************************************************/
    /* OnQuit                                                                  */
    /***************************************************************************/
    static errorType OnQuit(void **DS, const char *const command)
    {
        Quit(DS);
        if (*DS != NULL)
        {
            printf("Quit failed.\n");
            return error;
        };

        isInit = false;
        printf("Quit done.\n");

        return error_free;
    }

#ifdef __cplusplus
}
#endif