#ifndef _MEMORY_H_
#define _MEMORY_H_

//#define CONFIG_VERSION "soma"
#define CONFIG_START 0

typedef struct {
  char password[4];
  bool doorsOpened[6];
} TARDIS_MEMORY; 

class Memory {
  public:
    Memory();
  
    void setup();
    uint8_t indexOfCurrentDoor();
    void saveConfig();

    TARDIS_MEMORY values;
  private:
    void loadConfig(); 
};
#endif
