#include <Arduino.h>
#include <U8g2lib.h>

#include <Wire.h>


U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

// End of constructor list

int height;
int width;
int maxCharHeight;

void setup(void) {
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  height = u8g2.getDisplayHeight();
  width = u8g2.getDisplayWidth();
  maxCharHeight = u8g2.getMaxCharHeight();
}

char buffer[20];

void loop(void) {
  u8g2.clearBuffer();         // clear the internal memory
  for (int i=1; i<=4; i++)
  {
    int row = i*maxCharHeight-1;
    u8g2.drawStr(0,row,itoa(maxCharHeight,buffer,10));
    u8g2.drawStr(64,row,itoa(width,buffer,10));
    u8g2.drawStr(96,row,itoa(height,buffer,10));
  }
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);  
}
