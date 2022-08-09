# AffoFly_Motion

## Hardware
 - Arduino Pro Mini 5V 16MHZ
 - GY-521 (MPU6050)
 - 74HC595N
 - LED x5
 - 330R x5
 - Piezo Buzzer (active 5V)
 - Joystick (a good quality one without spring in throttle)
 - NRF24L+ PA with antena
 - 3.3V Voltage Regulator (eg. HC7333A or AMS1117)
 - Electrolytic Capacitor 10uf
 - Ceramic Capacitor 0.1uf
 - 18650 Lithum Battery
 - 5V Voltage Step Up Module (anything boosts battery voltage to 5V)
 - Battery Management Module (those commonly used with TC4056A chip will do)
 - Of course some wires, solder and an enclouse at your design.

If you are intend to only work on a breadboard with jumper wires for experiment or development, then the voltage regulator, voltage booster, battery and BMS is not really required. Instead, you will use a USB-TTL to program the Arduino and it normally comes with both 5V and 3.3V output, eg. FTDI or CH340 module.
