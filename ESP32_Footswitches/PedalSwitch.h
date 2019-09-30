
#pragma once

// Pete Wentworth cspwcspw@gmail.com.  Sept 2019
// Released under Apache License 2.0

// Each object of this class tracks a single instantaneous / momentary switch on a pedal board

class PedalSwitch
{
  public:

    bool IsSwitchClosed;

  private:
    long mostRecentEventTime;
    const long debounceTime = 10;
    int pinFromPedalBoard;

public:

      PedalSwitch() {}

      void Configure(int gpioPinFromPedalBoard)
      {
        pinFromPedalBoard = gpioPinFromPedalBoard;
        pinMode(pinFromPedalBoard, INPUT);
        IsSwitchClosed = false;
      }

      // Returns 0: no events happened
      // Returns 1: switch transitioned down
      // Returns 2: switch transitioned up
      int DidSwitchChange(long timeNow)
      {
        if (timeNow - mostRecentEventTime < debounceTime) return 0;
        bool isPedalDown = digitalRead(pinFromPedalBoard);
        
        // Now we need to detect switch transitions, either to open or closed state
        if (isPedalDown == IsSwitchClosed) return 0;   // no change since last poll
        mostRecentEventTime = timeNow; 
        IsSwitchClosed = isPedalDown;  // Remember the state to allow us to spot future transitions.
        if (isPedalDown) return 1;
        return 2;
      }
    };
