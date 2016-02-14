# NES_MIDI_remote
A MIDI controller inside a retro NES remote control. It also has a gyro inside, and 4 states.

MCU: teensy 2.0.
Gyro unit: MPU-6050 GY-521
Retro NES controller

8 state LEDS:
3 axis leds - indicate which axis are enabled (X, Y, Z)
4 state leds - indicate which state the controller is on (between 4)
1 reset led - indicates if the current state is the initial state - all axis enabled and state 1 is chosen
