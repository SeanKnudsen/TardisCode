#include "serialCom.h"
#include <string.h>
//  Constructor
  //SerialCom::SerialCom(){}
SerialCom::SerialCom() { }
  char inputArray[ARRAY_SIZE];
  int arrayLength = 0;
  bool gotCommand = false;
      int cmdCommand = 0;
    int cmdParam = 0;

// Call this from setup()
void SerialCom::setup()
{
  Serial.begin(115200);
  clearArray();
  
}

void SerialCom::read()
{
  char c;
  if(Serial.available())
  {
   c = Serial.read();
   inputArray[arrayLength] = c;
   arrayLength++;
   
     // if you want to debug, this is a good time to do it!
  if (c&&(ECHOINPUT))
  {
  }
  }
   


}

//    solenoids[i]->update(now);

// Command struct
// "2 1\n" is "LED 1"
// DOORS 1 4
//       2 5
//       3 6
void SerialCom::checkForCommand()
{
  char temp;
  read();
  if(arrayLength >=PACKET_LENGTH)
  {
    //check for command
    temp = inputArray[0];
    if(isCommand(temp))
    {
      if((inputArray[1] == ' ') && (inputArray[3] == '\r'))
      {
        gotCommand = true;
        cmdCommand = inputArray[0] - '0';
        cmdParam = inputArray[2] - '0';

        
      }
    }
    clearArray();
  }
  
}

void SerialCom::writeStr(int str)
{
}

void SerialCom::clearArray()
{
   int i;

  //init array with zeros
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    inputArray[i] = 0;
  }
  arrayLength = 0;
}

bool SerialCom::isCommand(char command)
{
  return (command == LED || command == OPEN_DOOR);
}



