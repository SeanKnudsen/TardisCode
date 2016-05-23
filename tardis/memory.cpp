#include <EEPROM.h>
#include "memory.h"
int counter;
TARDIS_MEMORY defaultValues;
TARDIS_MEMORY values;
int temp;
//  Constructor
Memory::Memory(){}



// Call this from setup()
void Memory::setup()
{
 // defaultValues = { CONFIG_VERSION, {false, false, false, false, false, false} };
  loadConfig();
}

void Memory::loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[2] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[3])
  {
    // if correct password "soma" was returned from the eeprom, we know we've written to it before. Let's load where we were at!
    for (unsigned int t=0; t<sizeof(values); t++)
      *((char*)&values + t) = EEPROM.read(CONFIG_START + t);
  }
  else
  {
    //values = defaultValues;
    saveConfig();
  }
}

void Memory::saveConfig() {
  for (unsigned int t=0; t<sizeof(values); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&values + t));
}

uint8_t Memory::indexOfCurrentDoor()
{
  int i = 0;
  int doorIndex = 0;
  for(i = 0; i<6; i++)
  {
    if(values.doorsOpened[i] = false)
    {
      doorIndex = i;
    }
  }
  return (uint8_t)doorIndex;
}

