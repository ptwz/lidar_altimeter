# Scope
This is a litte private project to create a small, cheap device to emulate the RADAR altimeters found in some aircraft.

# Prerequisites
## Hardware
1. Arduino Nano/Uno board
2. Garmin LIDAR Lite v3 module
3. Some capacitors (470uF/16V, 10uF/16V)
4. One resistor 1kOhm
This will give you a "tabletop" prototype. Proper hardware design follows.

## Software
1. Arduino IDE
2. The LIDAR Lite v3 library for Arduino (https://github.com/garmin/LIDARLite\_v3\_Arduino\_Library)
Optional (for creating own sound samples):
1. MacOS X "say" command
2. SOX (Sound Exchange)
3. xxd (From the VIM package)

# Build it
```   
   1. LIDAR Lite        to       Arduino Nano
   RED                           5V
   BLACK                         GND
   GREEN                         A5 (SCL)
   BLUE                          A4 (SDA)

   -Hook up 470uF/16V parallel to 5V and GND (watch the polarity obviously)
   
   2. Sound
                      ______       10uF
   Arduino pin 9 -----| 1k |---*---#|-------o
                      ------   |   +          PC Speaker (Amplified)
                        1uF + ===           o
                              ---           |
                               |            |
                               |            |
                        GND   ___          ___
```

# Use
Currently this is in trial stage, so I don't have any real world application experience. But basically it is all about mounting the Arduino, Sensors and Power supply on your aeroplane and feed the audio signal to the external in of your radio or into an amplified speaker.
