
#pragma once

// Pete Wentworth cspwcspw@gmail.com.  Sept 2019
// Released under Apache License 2.0

// Each object of this class represents a single control line (output)
// to my switchable functions on my Roland Cube 40XL amplifier.

// If your output stage has opto coupling, the signal is inverted, and 
// the ESP32 logic is a bit different.  If you are directly connected 
// then comment out this define.
#define OptoInverters 

class AmpControlLine
{
  public:
    bool IsPulledDown;
    bool IsLatching;
    bool IsInverted;
    String Name;

  private:
    int pinToAmp;

    void pullLineDown()
    {
      // The amp puts a 3.3v pull-up on each control line.  A switch on
      // a pedalboard can ground the line to activate the function.


#ifdef OptoInverters
     pinMode(pinToAmp, OUTPUT);
     digitalWrite(pinToAmp, HIGH);  //The optoInverters invert the signal
#else
      // So our microcontroller never drives the output line HIGH. It
      // either "disconnects" itself (by making the gpio pin an input
      // without pullup or pulldown), or it makes the pin an output and pulls it LOW.

      // pull the line down to ground
      pinMode(pinToAmp, OUTPUT);
      digitalWrite(pinToAmp, LOW);
#endif
      IsPulledDown = true;
    }

    void letLineFloat()
    {
      
#ifdef OptoInverters
      digitalWrite(pinToAmp, LOW);  // Turns off LED. The output will float
#else
      // Don't pull this up or down, just let go of the line completely.
      // When the line is high-impedance, the amp will pull it high.
      pinMode(pinToAmp, INPUT);     // make the ESP32 line float.
#endif

      IsPulledDown = false;
    }

  public:

    AmpControlLine() { }

    void Configure(String lineName, int gpioPin, bool wantLatching, bool wantInvertedPolarity)
    {
      Name = lineName;
      pinToAmp = gpioPin;
      IsLatching = wantLatching;
      IsInverted = wantInvertedPolarity;
      if (IsInverted) {
        letLineFloat();
      }
      else {
        pullLineDown();  // Turn off lead / solo / efx / reverb / delay
      }
    }

    void SwitchGotClosed()
    {
      // A latching line toggles on every down event.
      // We need not bother about polarity - an extra button click will negate the state.
      if (IsLatching)
      {
        if (IsPulledDown)  {
          letLineFloat();
        }
        else {
          pullLineDown();
        }
        return;
      }

      // Deal with instaneous line
      if (IsInverted) {
        if (!IsPulledDown) return; // spurious event, we are already up.
        letLineFloat();
      }
      else {
        if (IsPulledDown) return; // spurious event, we are already down.
        pullLineDown();
      }
    }

    void SwitchGotOpened()
    {
      // A latching line ignores up events
      if (IsLatching) return;

      // Deal with instaneous line
      if (IsInverted) {
        if (IsPulledDown) return; // spurious event, we are already down.
        pullLineDown();
      }
      else
      {
        if (! IsPulledDown) return; // spurious event, we are already up.
        letLineFloat();
      }
    }

};
