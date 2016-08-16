# Arduino Pro Micro 3.3V FM Radio
This code supports an FM radio built on the following components:
* Arduino Pro Micro 3.3V
  * <https://www.sparkfun.com/products/12587>
* Si4703 FM Tuner Evaluation Board
  * <https://www.sparkfun.com/products/12938>
* 128x32 I2C OLED Graphic Display based on SSD1306 Driver
  * <https://www.adafruit.com/products/931>

## Controls
I used three 10k potentiometers, connected to Pro Micro ADC inputs,
to control the channel and volume.

Traditional FM radio supports 100 stations (at least in the United States).
I found that the bins were too narrow when I tried to use one potentiometer
for channel setting. My solution (admittedly a bit odd) was to use two: one
potentiometer acts as the coarse control, adjusting the whole number setting
(88, 89, 90, ... 101); the other acts as the fine control, adjusting the fractional number setting (88.1, 88.3, 88.5 ... 88.9). The third potentionmeter 
adjusts volue, ranging from 0 to 15 (this range is defined by the FM tuner 
evaluation board).

## Processing Loop
The code performs the following actions:

1. Update the volume
2. Update the channel
3. Update the display
4. Sleep for 100ms

Updating the volume is a simple conversion of the volume potentiometer into a value of 0 to 15. The channel is updated by reading the coarse and fine 
potentiometers, and combining them to form the correct channel. Updating the
display involves writing the channel at the top, and writing the volume at the 
bottom in a smaller font.

That's it. Pretty simple. A lot of code was taken from Adafruit and Sparkfun.
Adafruit's SSD1306 and GFX libraries, and Sparkfun's Si4703 breakout library 
made this project possible.
