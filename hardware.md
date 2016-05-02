**Prototype**

The prototype includes no other hardware than that required to power the Photon.
Instead of discrete LEDs or indicators, I chose to create a small web page that
displays red/green svg circles to represent each build status.

**Wall Mount**
I'd considered an LED matrix, since I'm providing both a root status indicator
as well as having to support sub-projects. Not exactly linear. However, making a
matrix large enough to mount on a wall was more work than it's worth to me. 
Something I can quickly glance at is the goal.

I am currently considering options for indicators. I want 3-5" lights of some kind.
Perhaps discs of plexiglass, side-lit with discrete bicolor LEDs or strips. Perhaps
mason or baby food jars with a led and reflector on the bottom. Ideas welcome.

The mount will include space for the photon, a hard reset button (in case the Photon goes off into the weeds,
and won't respond to a soft reset function call). Also want to include the mode
button, as we may need to do a factory reset (which requires both buttons). I intend
to power the whole thing with a bog-standard 5V wall wart.

Since I'm limited to 16 GPIO pins, and currently am monitoring 15 projects, so in 
view of possible future expansion, I will be using a [shift register](http://www.protostack.com/blog/2010/05/introduction-to-74hc595-shift-register-controlling-16-leds/)
to output data. Search Instructables for circuits that include LED drivers if you're
going to use LED strips as opposed to discrete LEDs.

