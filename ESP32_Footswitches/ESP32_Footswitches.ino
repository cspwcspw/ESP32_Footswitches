
// Pete Wentworth cspwcspw@gmail.com.  Sept 2019
// Released under Apache License 2.0

// Uses an ESP-32.
// Provides a 6-channel controller for my Roland Cube 40XL amp.
// This can replace three expensive BOSS FS-6 footswitches,
// which cost about $60-$70 each on Amazon, and need batteries.
// Because the ESP32 allows us to control the amp from software,
// it opens to possibilities for controllers via usb, wifi, or bluetooth
// e.g. keyboard input, automatic setup for each new song,
// gesture recognition to control the amp, or plain old footswitches!

#include "PedalSwitch.h"
#include "AmpControlLine.h"



#define numControls 6

String Names[numControls] = {"Lead", "Solo", "EFX", "Reverb", "LooperStart_alias_Delay", "LooperStop_alias_Tap"};
int OutPinMap[numControls] = { 5, 18, 21, 19, 23, 22 };
bool OutputsLatched[numControls] = { true, true, true, true, true, false };
bool InvertPolarity[numControls] = { false, false, false, false, false, true };
AmpControlLine outLines[numControls];

int InPinMap [numControls] = { 33, 32, 35, 34, 39, 36 };
PedalSwitch inLines[numControls];


void showStatus()
{ // For testing and debugging
  Serial.print("Sw: ");
  for (int i = 0; i < numControls; i++)
  {
    Serial.print(inLines[i].IsSwitchClosed ? "Dn " : "Up ");
  }
  Serial.print(" Amp: ");
  for (int i = 0; i < numControls; i++)
  {
    Serial.print(outLines[i].IsPulledDown ? "0 " : "1 ");
  }
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("\nPete's BossPedals101  V1.0 (%s %s)\n%s\n", __DATE__, __TIME__, __FILE__);
  for (int i = 0; i < numControls; i++)
  {
    inLines[i].Configure(InPinMap[i]);
    outLines[i].Configure(Names[i], OutPinMap[i], OutputsLatched[i], InvertPolarity[i]);
  }

  // Some special initialization for the Looper and Stop/Tap line to clear the looper memory:
  // Simulate holding down the switch for a few seconds.
  delay(100);
  // Record a short phrase 
    outLines[4].SwitchGotClosed();  delay(500); outLines[4].SwitchGotClosed(); 
    delay(4000);
  showStatus();
  outLines[5].SwitchGotClosed();
  showStatus();
  delay(3000);
  outLines[5].SwitchGotOpened();

  showStatus();
}



void loop() {
  long timeNow = millis();
  bool gotBoardChange = false;
  for (int i = 0; i < numControls; i++) {
    int response = inLines[i].DidSwitchChange(timeNow);
    if (response != 0) {
      gotBoardChange = true;
      if (response == 1) {
        outLines[i].SwitchGotClosed();
      }
      else {
        outLines[i].SwitchGotOpened();
      }
    }
  }
  if (gotBoardChange) {
    showStatus();
  }
}
