#ifndef _SERIALCOM_H_
#define _SERIALCOM_H_

#include "arduino.h"
#include <string.h>

#define ECHOINPUT 1
#define ARRAY_SIZE 20
#define PACKET_LENGTH 4
#define COMMAND_COUNT 3

#define OPEN_DOOR '1' // 1-6
#define LED '2'       // 1-4


//2 1
//2 2
//2 3

class SerialCom {
  public:
    SerialCom();
    void setup();
    void read();
    void checkForCommand();
    void clearArray();
    void writeStr(int str);
    
    bool gotCommand;
    int cmdCommand;
    int cmdParam;
  private:
    bool isCommand(char command);
};


#endif
