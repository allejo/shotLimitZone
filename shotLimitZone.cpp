/*
Shot Limit Zone
    Copyright (C) 2013-2014 Vladimir Jimenez

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bzfsAPI.h"

class shotLimitZone : public bz_Plugin, bz_CustomMapObjectHandler
{
public:
    virtual const char* Name (){return "Shot Limit Zones";}
    virtual void Init (const char* config);
    virtual void Cleanup (void);
    virtual void Event (bz_EventData *eventData);

    virtual bool MapObject (bz_ApiString object, bz_CustomMapObjectInfo *data);

    // Store all the custom shot limit zones in a struct so we can loop through them
    struct shotLimitZones
    {
        float position[3];
        float size[3];
        int shotLimit;
        std::string flagType;
    };
    std::vector<shotLimitZones> slzs;

    // We'll be keeping track of how many shots a player has remaining in a single array
    // If the value is greater than -1, then that means the player has grabbed a flag
    // with a shot limit so we'll keep count of how many.
    int playerShotsRemaining[256];

    // We'll be keeping track if we need to send a player a message after their first shot to
    // show them that they have a limited number of shots. If the shot limit is 10 and this
    // value is set to true, the we will warn them at 9 shots remaining.
    // FIXME: Unfortunately this is not very useful if the shot limit is 1. Perhaps notify
    // of shot limit on grab rather than on first shot.
    bool firstShotWarning[256];
};

BZ_PLUGIN(shotLimitZone)

void shotLimitZone::Init(const char* /*commandLine*/)
{
    // Register our events
    Register(bz_eFlagDroppedEvent);
    Register(bz_eFlagGrabbedEvent);
    Register(bz_ePlayerDieEvent);
    Register(bz_ePlayerJoinEvent);
    Register(bz_eShotFiredEvent);

    // Register our custom BZFlag zones
    bz_registerCustomMapObject("shotLimitZone", this);
}

void shotLimitZone::Cleanup()
{
    // Remove all the events this plugin was watching for
    Flush();

    // Remove the custom BZFlag zones
    bz_removeCustomMapObject("shotLimitZone");
}

bool shotLimitZone::MapObject(bz_ApiString object, bz_CustomMapObjectInfo *data)
{
    // We only need to keep track of our special zones, so ignore everything else
    if (object != "SHOTLIMITZONE" || !data)
    {
        return false;
    }

    // We found one of our custom zones so create something we can push to the struct
    shotLimitZones newSLZ;

    // Loop through the information in this zone
    for (int i = 0; i < data->data.size(); i++)
    {
        // Store the current line we're reading in an easy to access variable
        std::string temp = data->data.get(i).c_str();

        // Create a place to store the values of the custom zone block
        bz_APIStringList *values = bz_newStringList();

        // Tokenize each line by space
        values->tokenize(temp.c_str(), " ", 0, true);

        // If there is more than one value, that means they've put a field and a value
        if (values->size() > 0)
        {
            // Let's not care about case so just make everything lower case
            std::string checkval = bz_tolower(values->get(0).c_str());

            // Check if we found the position specifications
            if ((checkval == "position" || checkval == "pos") && values->size() > 3)
            {
                newSLZ.position[0] = (float)atof(values->get(1).c_str());
                newSLZ.position[1] = (float)atof(values->get(2).c_str());
                newSLZ.position[2] = (float)atof(values->get(3).c_str());
            }

            // Check if we found the size specifications
            if (checkval == "size" && values->size() > 3)
            {
                newSLZ.size[0] = (float)atof(values->get(1).c_str());
                newSLZ.size[1] = (float)atof(values->get(2).c_str());
                newSLZ.size[2] = (float)atof(values->get(3).c_str());
            }

            // Check if we found the shot limit specification
            if (checkval == "shotlimit" && values->size() > 1)
            {
                newSLZ.shotLimit = atoi(values->get(1).c_str());
            }

            // Check if we found the flag we'll be limiting
            if (checkval == "flag" && values->size() > 1)
            {
                newSLZ.flagType = bz_toupper(values->get(1).c_str());
            }
        }

        // Avoid memory leaks
        bz_deleteStringList(values);
    }

    // Send a debug message of the zone we're monitoring
    bz_debugMessagef(2, "A shotLimitZone has been found, zone loaded with credentials:\nPos: [%lf, %lf, %lf]\nSize: [%lf, %lf, %lf]\nShot Limit: %i\nFlag: %s",
             newSLZ.position[0], newSLZ.position[1], newSLZ.position[2], newSLZ.size[0], newSLZ.size[1], newSLZ.size[2], newSLZ.shotLimit, newSLZ.flagType.c_str());

    // Add the information we got and add it to the struct
    slzs.push_back(newSLZ);

    // Return true because we handled the map object
    return true;
}

void shotLimitZone::Event(bz_EventData *eventData)
{
    // Switch through the events we'll be watching
    switch (eventData->eventType)
    {
        case bz_eFlagDroppedEvent:
        {
            bz_FlagDroppedEventData_V1* flagDropData = (bz_FlagDroppedEventData_V1*)eventData;
            // The playerID here may be -1 if we just called bz_resetFlag or bz_removePlayerFlag. BZFS inadvertently triggers
            // another flag drop event due to only updating the flag status after callEvents.
            // Alternatively we could avoid this problem by calling those flag reset functions on subsequent event calls such as
            // on eTickEvent.
            if (flagDropData->playerID < 0)
            {
                return;
            }

            // If the player who dropped the flag had shots remaining with the flag, don't let them regrab it so reset the flag
            // as soon as they drop it
            if (playerShotsRemaining[flagDropData->playerID] > 0)
            {
                bz_resetFlag(flagDropData->flagID);
            }
        }
        break;

        case bz_eFlagGrabbedEvent:
        {
            bz_FlagGrabbedEventData_V1* flagData = (bz_FlagGrabbedEventData_V1*)eventData;

            playerShotsRemaining[flagData->playerID] = -1;
            firstShotWarning[flagData->playerID] = false;

            // Loop through all the shot limit zones that we have in memory to check if a flag was grabbed
            // inside of a zone
            for (int i = 0; i < slzs.size(); i++)
            {
                if (flagData->pos[0] >= slzs[i].position[0] - slzs[i].size[0] &&
                    flagData->pos[0] <= slzs[i].position[0] + slzs[i].size[0] &&
                    flagData->pos[1] >= slzs[i].position[1] - slzs[i].size[1] &&
                    flagData->pos[1] <= slzs[i].position[1] + slzs[i].size[1] &&
                    flagData->pos[2] >= slzs[i].position[2] &&
                    flagData->pos[2] <= slzs[i].position[2] + slzs[i].size[2])
                {
                    if (bz_getFlagName(flagData->flagID).c_str() == slzs[i].flagType)
                    {
                        // Keep track of shot limits here
                        playerShotsRemaining[flagData->playerID] = slzs[i].shotLimit;
                        firstShotWarning[flagData->playerID] = true;
                        break;
                    }
                }
            }
        }
        break;

        case bz_eShotFiredEvent:
        {
            bz_ShotFiredEventData_V1* shotData = (bz_ShotFiredEventData_V1*)eventData;
            int playerID = shotData->playerID;

            // If player shots remaining is -1, then we don't have to keep count of them but otherwise we do
            if (playerShotsRemaining[playerID] >= 0)
            {
                // They fired a shot so let's decrement the remaining shots
                playerShotsRemaining[playerID]--;

                if (playerShotsRemaining[playerID] == 0)
                {
                    // Decrement the shot count so we can drop down to -1 so we can ignore it in the future
                    playerShotsRemaining[playerID]--;

                    // Take the player's flag
                    bz_removePlayerFlag(playerID);
                }
                else if ((playerShotsRemaining[playerID] % 5 == 0 || playerShotsRemaining[playerID] <= 3 || firstShotWarning[playerID]) && playerShotsRemaining[playerID] > 0)
                {
                    // If the shot count is less than or equal to 3, is divisable by 5, or it's their first shot
                    // after the flag grab, notify the player
                    bz_sendTextMessagef(BZ_SERVER, playerID, "%i shot%s left", playerShotsRemaining[playerID], (playerShotsRemaining[playerID] > 1) ? "s" : "");

                    // If we have sent their first warning, then let's forget about it
                    if (firstShotWarning[playerID])
                    {
                        firstShotWarning[playerID] = false;
                    }
                }
            }
        }
        break;

        default:
        break;
    }
}