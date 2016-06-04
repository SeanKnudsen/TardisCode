#include <arduino.h>
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
  defaultValues = { 's', 'o', 'm', 'a', false, false, false, false, false, false};
  Serial.println("Memory: setup");
  loadConfig();

}

void Memory::loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == 's' &&
      EEPROM.read(CONFIG_START + 1) == 'o' &&
      EEPROM.read(CONFIG_START + 2) == 'm' &&
      EEPROM.read(CONFIG_START + 3) == 'a')
  {
    // if correct password "soma" was returned from the eeprom, we know we've written to it before. Let's load where we were at!
    for (unsigned int t=0; t<sizeof(values); t++)
    {
      *((char*)&values + t) = EEPROM.read(CONFIG_START + t);
    }
      Serial.println("Memory: setup - loaded memory successfully");
  }
  else
  {
    values = defaultValues;
      Serial.println("Memory: loadConfig - need to save values because we didn't recognize what was read.");
    saveConfig();
  }
}

void Memory::saveConfig() {

  Serial.println("Memory: saveConfig - do the save");
  
  for (unsigned int t=0; t<sizeof(values); t++)
  {
    Serial.print("Write "); Serial.print("location - "); Serial.print(CONFIG_START + t); Serial.print("\r\n");
    Serial.print("Write "); Serial.print("value    - "); Serial.print(*((char*)&values + t)); Serial.print("\r\n");
    EEPROM.write(CONFIG_START + t, *((char*)&values + t));
  }
}

uint8_t Memory::indexOfCurrentDoor()
{
  int i = 0;
  int doorIndex = 0;
  for(i = 0; i<6; i++)
  {
    if(values.doorsOpened[i] == false)
    {
      Serial.print("Memory: set index of current door."); Serial.print(i); Serial.print("\r\n");
      doorIndex = i;
      break;
    }
  }
  return (uint8_t)doorIndex;
}

