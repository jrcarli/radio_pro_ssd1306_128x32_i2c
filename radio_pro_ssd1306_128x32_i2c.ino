/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Si4703_Breakout.h>

#define OLED_RESET 9
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define RADIO_RESET 8
#define SDIO 2
#define SCLK 3

Si4703_Breakout radio(RADIO_RESET, SDIO, SCLK);
int channel = 881;
int channelIndex = 0;
int volume = 0;
char rdsBuffer[10] = {0};

int volumePin = A0;
int coarseChannelPin = A1;
int fineChannelPin = A2;

int channels[100] = {
  881,  883,  885,  887,  889,
  891,  893,  895,  897,  899,
  901,  903,  905,  907,  909,
  911,  913,  915,  917,  919,
  921,  923,  925,  927,  929,
  931,  933,  935,  937,  939,
  941,  943,  945,  947,  949,
  951,  953,  955,  957,  959,
  961,  963,  965,  967,  969,
  971,  973,  975,  977,  979,
  981,  983,  985,  987,  989,
  991,  993,  995,  997,  999,
  1001,  1003,  1005,  1007,  1009,
  1011,  1013,  1015,  1017,  1019,
  1021,  1023,  1025,  1027,  1029,
  1031,  1033,  1035,  1037,  1039,
  1041,  1043,  1045,  1047,  1049,
  1051,  1053,  1055,  1057,  1059,
  1061,  1063,  1065,  1067,  1069,
  1071,  1073,  1075,  1077,  1079
};

char mychars[] = "0123456789.";

void d(int x) {
    display.print(mychars[x]);
}

void str(int channel) {
    int x = channel;
    int loops = 1;
    while(x > 0) {
        if(x > 1000) { d(1); d(0); x = x - 1000; if(x<10) { d(0); loops++; } }
        else if(x > 900) { d(9); x = x - 900; if(x < 10) { d(0); loops++; } }
        else if(x > 800) { d(8); x = x - 800; }
        else if(x > 90) { d(9); x = x - 90; }
        else if(x > 80) { d(8); x = x - 80; }
        else if(x > 70) { d(7); x = x - 70; }
        else if(x > 60) { d(6); x = x - 60; }
        else if(x > 50) { d(5); x = x - 50; }
        else if(x > 40) { d(4); x = x - 40; }
        else if(x > 30) { d(3); x = x - 30; }
        else if(x > 20) { d(2); x = x - 20; }
        else if(x > 10) { d(1); x = x - 10; }
        else if(x==9) { d(9); x = x - 9; }
        else if(x==8) { d(8); x = x - 8; }
        else if(x==7) { d(7); x = x - 7; }
        else if(x==6) { d(6); x = x - 6; }
        else if(x==5) { d(5); x = x - 5; }
        else if(x==4) { d(4); x = x - 4; }
        else if(x==3) { d(3); x = x - 3; }
        else if(x==2) { d(2); x = x - 2; }
        else if(x==1) { d(1); x = x - 1; }
        
        if(loops == 2) { d(10); }
        loops++;
    }    
}

int getPosition(int pin, int positions) {
  int i = 0;
  int j = 0;
  int val = analogRead(pin);
  int steps = 1024 / positions; // has slough at top mark  
  for(i=0;i<1024;i+=steps) {
    if( (val >= i) && (val < (i+steps)) ) {
      break;
    }
    j++;
  }
  if(j >= positions) {
    return positions-1;
  }
  else {
    return j;
  }
}

void updateChannel() {  
  int msb = getPosition(coarseChannelPin,20);  
  int lsb = getPosition(fineChannelPin,5);
    
  channelIndex = (msb * 5) + lsb;
  channel = channels[channelIndex];
  radio.setChannel(channel);  
}

void updateVolume() {  
  volume = getPosition(volumePin,16);  
  radio.setVolume(volume);
}

void updateDisplay() {
  // Clear the buffer.
  display.clearDisplay();  

  // Display station at top 
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);  
  str(channels[channelIndex]);
  display.println(" FM");
  
  // Display volume in smaller font below 
  display.setTextSize(1);  
  display.println("");
  display.print("Volume ");
  display.println(volume);
  display.display();  
}

void setup()   {         
  radio.powerOn();
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)  
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);  
}

void loop() {    
  updateVolume();
  updateChannel();  
  updateDisplay();
  delay(100);
}
