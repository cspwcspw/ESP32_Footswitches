# ESP32_Footswitches

*_An ESP32 provides cheap home-built foot switches for my Roland Cube 40XL amplifier._*

_Pete Wentworth_    mailto:cspwcspw@gmail.com  

## Overview

Here you'll find a [great promotional demo](https://www.youtube.com/watch?v=m5H6hk8yA-s) 
of Alex Hutchinson 
introducing the Cube 40XL guitar amp. He makes good 
use of the foot switches to select the six externally 
controllable features. Roland recommend using three 
Boss FS-6 dual foot switches (with a battery in each). 
That will set you back more than $200 on Amazon. 

![FS-6](Images/FS6_Footswitch.png "Boss FS-6 Dual Foot Switch")

Instead, we'll control the six switch functions directly from
GPIO lines on an ESP32. Once we have software that can
switch each amp function on or off, we can experiment with
different input controllers to our ESP32: our own switches,
of some kind, or software configurations, touch screen, 
commands over USB, WiFi or Bluetooth, etc.

I'll show a simple proof-of-concept controller later: a
very low-end and cheap implementation. Probably not 
the right thing for your concert in Hyde Park,
but good enough for home use. 

## Can we switch the amplifier functions from an ESP32?

There are three stereo jack sockets in the back of the amp,
and a matching stereo socket in each FS-6 dual footswitch.

The amp logic is similar to a GPIO pin with INPUT_PULLUP mode
set: the amp pulls each of the lines to 3.3v. The line level
activates or deactivates the function or mode of the amp.
For example, if we let the SOLO line float high, the amp switches
into SOLO mode. If we ground the line, SOLO mode disengages, 
and the amp reverts to whatever is set on the panel.  So the 
first step is a minimal ESP32 "blink" sketch that verifies we can 
toggle SOLO mode, and see the LED on the amp blinking:

```
// Wiring: Connect the GND pin of the ESP32 to the ground pin 
// (shaft) of a stereo jack plug.  Then connect GPIO 18 to the 
// RING on the jack plug. Plug into the SOLO foot switch socket.

#define SoloPin  18      // GPIO pin to control SOLO function 
void setup() { }

void loop() {
   // Simulate closing the footswitch by pulling the line low.
   // This deactivates SOLO mode on the amp. 
   pinMode(SoloPin, OUTPUT);
   digitalWrite(SoloPin, LOW);
   delay(1000);

   // Simulate opening the foot switch by putting the line 
   // into high impedance state. The amp will pull it up  
   // and SOLO mode will be turned on.
   pinMode(SoloPin, INPUT);
   delay(3000); 
}

```
Yay, this works!  I've managed to turn my guitar
amplifier into a blinking LED.
  
## Latching or instantaneous functions?

Some functions, like SOLO or REVERB work best with a 
latching switch.  That means you press and release
the foot switch, and REVERB turns on.  Next time you 
press and release the foot switch, REVERB turns off.

Other effects need an instantaneous (sometimes called momentary)
foot switch: the effect is only active while you keep
the switch pressed down. For example, the looper stop
switch is instantaneous - a single click on the footswitch 
will stop the looper playback.  But if you hold it down 
for a couple of seconds, it clears the looper memory. 

Each switch on the Boss FS-6 footswich can be selected to 
be either instantaneous or latching. 

For us to get the same effect in software is easy. If 
we have a push button switch with no mechanical 
latching (e.g. a door bell switch or a mouse button), 
we will detect and respond to its two different transitions -
__switchGotClosed__ and __switchGotOpened__, differently if we
want latching or not. 

The software here is organized in 
three components: a class for 
managing an output line to the
amp, and a class for handling an 
input (a switch, a mouse button, 
a touch controller, etc), and the main loop
that instantiates the input and output objects and 
determines the cross-connects between inputs and outputs,
and passes event messages between the components. 

One design decision is where to 
locate this "latching" functionality in our ESP32 
software.  It seems that it is the line that needs to
latch (as opposed to a mechanical latching switch like a 
bedside lamp), so in this software latching is the 
responsibility of the output line driver component. 

## A six-switch pedalboard

I bought a handful of proximity touch sensor switches 
from Banggood, typically about $9 for 20 switches.  

![touchSensors](Images/touch_sensors.jpg "Touch Sensors")

The ones I have learn their capacitance when they are first
powered on, and then detect when a finger gets close enough.
They too can work as latching or instantaneous
switches, but it involves cutting a track on the board to 
change function. These ones also have little on-board LEDs that
light when the switch is triggered. I 
left mine all instantaneous, and will deal
with latching in the software. They can sense touch through or
behind a non-conductive cover plate of a couple of millimeters 
of perspex, glass, wood, etc., so in principle, we should be able
to make our pedal board tough, waterproof, and
make it look great.

The board itself was a discarded piece of shelf wood.  I cut some
untidy recesses in the front so that the six switches would
sit flush with the top wood surface, drilled holes through for the
wiring. 

![frontRecesses](Images/front_recesses.jpg "Front Recesses")

I routed some channels in the back of the board for 
wiring. 

![rearChannels](Images/rear_channels.jpg "Rear channels")

Each sensor needs Vcc, Ground, and has an output line which goes
high or low. The Vcc and Ground pins can all use a 
a common bus.  So in total I needed an 8-wire cable from the board
back to the ESP32: six switch outputs, Vcc, and Ground. Ethernet 
cable has 8 cores, and I had some lying around. Here is the 
wiring harness being put in place:  

![rearWiring](Images/rear_wiring.jpg "Rear wiring")

And here we have the first few switches soldered in place on the front of
the board.

![frontSoldering](Images/front_soldering.jpg "Front soldering")

Here is the final foot switch, with a couple of plectrums glued over
the touch sensors. 

![finalFront](Images/final_front_of_board.jpg "Front of prototype board")

Then the three output jacks that plug into the amp.

![jackplugs](Images/jackplugs.jpg "Jackplugs for amp")

The ESP32 has the upper cable carrying the inputs from the pedalboard,
and the lower cable carrying the outputs to the jack plugs.  The ESP32 Devkit 
is a bit too wide to play nicely with standard breadboard spacing, 
so I cut the breadboard in half length-ways, spread it out, and attach 
it to a piece of wood. It gives a nice way to secure cabling with 
cable ties, and there is a convenient "tunnel" under the ESP32 that 
can make wiring a bit tidier. 

![breadboard](Images/breadboard.jpg "Bread board prototype")

The ESP32 code for the Arduino IDE is in the repository. 

## Version 2

The board creates some white noise in the amp if I power it
from the USB on my laptop. Presumably I have an earth loop - the amp is 
earthed, the laptop is earthed, and I've created an earth path
via USB to the ESP32 and then to the switches.  The noise goes
away if I run from a USB power bank or battery power.  

So I added six opto-couplers to isolates the ESP32 from
the amplifier.  As a by-product, the opto-couplers (I managed to
source some TLP620's from a friend over the weekend) invert the 
signal. So the logic is simpler: the microcontroller drives the 
output line (the LED side of the optocoupler) either HIGH or LOW, 
the output side of the transistor turns on (current flows, amplifier
sees the line is LOW) or the opto transistor turns off -
the amplifier pulls the line up and the amp sees a HIGH. 

The amplifier earth is now isolated from the ESP32 earth, 
which removes my earth loop and solved my noise issue. 

As part of integrating the opto-couplers I also soldered 
everything down onto a prototyping PCB, making version two
somewhat neater than the one on my breadboard.

![version2](Images/version2.jpg "Soldered Version 2")



## Conclusion



It is a prototype, intended as a proof-of-concept. 
Ideally we need some pretty trim on the front to make it look better, 
and we could screw an extra piece of plywood over the wiring at
the back (even adding some nice rubber feet).  We might even want
to move the ESP32 and a power source down into a recess in the back
of the board, and add output LEDs to reflect the state of the lines
on the pedalboard.  (There are enough GPIOs for individually wiring
six switch inputs, six line drivers, and six indicator LEDs).
To date I've not done that.

The switches are not reliable with rubber soles on your shoes.
So perhaps this design works best for bare-footed guitarists. 

Obviously we can replace them with other switches or proximity sensors - 
I have a handful of TLP909 proximity sensors somewhere that 
are candidates.  They have an infrared emitter and an 
infrared detector, and are triggered by photoreflectivity.
Typically they are used in photocopiers to detect paper. 
So provided your shoe soles are white, you should be fine.

I await some stomp-box switches that I bought for less than $2 each,
and I might have a go at "Version 3". 

![stompSwitches](Images/stomp_switches.png "Stomp Switches")

So all in all, $20 with the logic of an ESP32 gave me a 
fun project that enables foot switch control of the 
features on my amp 
(and is extensible to do even more in future) at a fraction of 
the cost of buying the recommended accessories.


*Last Revision 30 September 2019*  
mailto:cspwcspw@gmail.com 



