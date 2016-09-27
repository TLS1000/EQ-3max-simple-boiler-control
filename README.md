# EQ-3max-simple-boiler-control
simple system with arduino uno and/or ESP8266 that provides boilerON/OFFcontrol based on room thermostats valve positions 

There is the Thermeq3 project with arduino YUN which has a lot of fancy features, but it's too complicated for me. Also Yun controller is not cheap. This initial code was made for an Arduino UNO that sends AT-commands to ESP8266 WIFI chip, and interfacing via Hobbyking LCD. So for a few euro/dollar you're all set.

My system is running for more than a year now, a cube with seven thermostats. There is also domestic hot water control. The code uses three outputs: 'burner control' 'circulation pump for heating' and ' circulation pump for domestic hot water. 

Basically the uno sets up a telnet connection to the cube, does a base-64 translation, and finds all 'thermostats' connected to the cube. If valve position is above a certain percentage, burner and circulation pump are activated. 

If I were to do it again, I would simply use wemos R2D1 board. Maybe someone can do this and post an update.

System is not perfect, main issue is that the cube doesn't respond anymore of a telnet session is not closed properly, for example because of a mains failure. So every few months I need to restart the cube. I don't know if you can instruct the cube to reset via telnet.

You'll see that my program is very simple, maily because I'm a limited programmer :-). So hopefully the code can improve via this repository....

