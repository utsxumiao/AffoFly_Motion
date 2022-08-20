# AffoFly_Motion

## Parts List
 - Arduino Pro Mini 3.3V 8MHZ
 - GY-521 (MPU6050)
 - Optional: WS2182B 5V LED x5
 - Optional: Piezo Buzzer (active 3.3V)
 - Joystick (a good quality one without spring in throttle)
 - Switch (power and aux1, aux2)
 - Button (aux3, aux4)
 - NRF24L+ PA with antena
 - Electrolytic Capacitor 470uf
 - Electrolytic Capacitor 10uf
 - Ceramic Capacitor 0.1uf
 - 3.7v Lithum Battery (eg. 18650)
 - Battery Management Module (those commonly used with TC4056A chip will do)
 - 2.54mm Female Pin Connector x6 (for Arduino programming, relay input)
 - Of course some wires, soldering wires and an enclouse at your design.

If you are intend to only work on a breadboard with jumper wires for experiment or development, then the voltage regulator, voltage booster, battery and BMS is not really required. Instead, you will use a USB-TTL to program the Arduino and it normally comes with both 5V and 3.3V output, eg. FTDI or CH340 module.
