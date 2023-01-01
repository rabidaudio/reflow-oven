https://learn.adafruit.com/adafruit-max31865-rtd-pt100-amplifier/rtd-wiring-config
https://learn.adafruit.com/adafruit-led-backpack/0-54-alphanumeric-9b21a470-83ad-459c-af02-209d8d82c462
https://www.sparkfun.com/tutorials/60
https://www.sparkfun.com/tutorials/58
https://www.youtube.com/watch?v=sdKK8a5j8Rg


preheat: get to 125C at 2-3 C/s (approx 50 seconds)
soak: 125C to 180C at 0.5-1 C/s (approx 140s)
reflow up: 180C to 235C at 2 C/s 
reflow down: 235C to 180C at -2 C/s
cool: 180C to room at -2 to -4 C/s



normal heatup speed seems to be 2.1 C/s


determined imperically:

56/256 duty cycle at 16384ms period seems to be a steady temperature at 140C
at 180C that duty cycle is more like 79/256


using 101 center @ 210, high 121, low 81





1. heat at no faster than 2.5 C (slow down if necessary)
2. in advance of 125, slow down to a rate of 0.5 C/s
3. in advance of 180, slam throttle
4. in advance of 235, cut throttle
5. below 180, keep cool rate slower than -4 C/s

