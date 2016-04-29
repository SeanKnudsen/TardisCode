#ifndef _BUTTON_H
#define _BUTTON_H

#define SHORT_PRESS_DURATION 100  //milliseconds
#define LONG_PRESS_DURATION 550  //milliseconds
#define MENU_PRESS_DURATION 1500 // milliseconds



class Button {
  public:

    enum PRESS_TYPE
    {
      None,
      Short,
      Long,
      Menu
    };
  
    Button();
    void setup();
    void clearButtonPress();
    void pinPoll();

    const int buttonPin = 47;

    

    bool ShortPress;
    bool LongPress;
    bool MenuPress;

  private:
};



#endif
