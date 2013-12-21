#include "bzfsAPI.h"
#include <iostream>
#include <stdio.h>

class shotLimitZone : public bz_Plugin, bz_CustomMapObjectHandler
{
public:
  virtual const char* Name (){return "Shot Limit Zones";}
  virtual void Init ( const char* config);
  virtual void Cleanup( void );
  virtual void Event ( bz_EventData *eventData);

  virtual bool MapObject (bz_ApiString object, bz_CustomMapObjectInfo *data);

  struct shotLimitZones
  {
    float position[3];
    float size[3];
    int shotLimit;
    std::string flagType;
  };
  std::vector<shotLimitZones> slzs;
};

BZ_PLUGIN(shotLimitZone)

void shotLimitZone::Init ( const char* /*commandLine*/ )
{
  bz_debugMessage(4,"shotLimitZone plugin loaded");
  Register(bz_eFlagGrabbedEvent);

  bz_registerCustomMapObject("shotLimitZone", this);
}

void shotLimitZone::Cleanup()
{
  Flush();
  bz_removeCustomMapObject("shotLimitZone");
}

bool shotLimitZone::MapObject (bz_ApiString object, bz_CustomMapObjectInfo *data)
{
  if (object != "shotLimitZone" || !data)
  {
    return 0;
  }

  shotLimitZones newSLZ;
  for (int i = 0; i < data->data.size(); i++)
  {
    std::string temp = data->data.get(i).c_str();
    bz_APIStringList *values = bz_newStringList();
    values->tokenize(temp.c_str(), " ", 0, true); //Tokenize by spaces...

    if (values->size() > 0)
    {
      //Make sure there is something other than blank space.
      std::string checkval = bz_tolower(values->get(0).c_str()); //We don't care how they write it, so long as it resembles a valid value.

      if (checkval == "position" && values->size() > 3)
      {
	newSLZ.position[0] = (float)atof(values->get(1).c_str());
	newSLZ.position[1] = (float)atof(values->get(2).c_str());
	newSLZ.position[2] = (float)atof(values->get(3).c_str());
      }
      if (checkval == "size" && values->size() > 3)
      {
	newSLZ.size[0] = (float)atof(values->get(1).c_str());
	newSLZ.size[1] = (float)atof(values->get(2).c_str());
	newSLZ.size[2] = (float)atof(values->get(3).c_str());
      }
      if (checkval == "shotLimit" && values->size() > 1)
      {
	newSLZ.shotLimit = (int)atof(values->get(1).c_str());
      }
      if (checkval == "flag" && values->size() > 1)
      {
	newSLZ.flagType = values->get(1).c_str();
      }
    }

    bz_deleteStringList(values);
  }

  bz_debugMessagef(1, "A shotLimitZone has been found, zone loaded with credentials:\nPos: [%lf,%lf,%lf]\nSize: [%lf,%lf,%lf]\nShot Limit: %lf\nFlag: %s",
		   newSLZ.position[0], newSLZ.position[1], newSLZ.position[2], newSLZ.size[0], newSLZ.size[1], newSLZ.size[2], newSLZ.shotLimit, newSLZ.flagType.c_str());
  slzs.push_back(newSLZ);
  return 1;
}

void shotLimitZone::Event(bz_EventData *eventData)
{
  switch (eventData->eventType)
  {
    case bz_eFlagGrabbedEvent:
    {

    }
    break;

    default:
      break;
  }
}