#include <Arduino.h>
#include <U8g2lib.h>

#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

int displayHeight;
int displayWidth;
int lineHeight;
int variacAdcPin = 0;

void setup(void) {
  u8g2.begin();
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r);
  u8g2.setFontPosTop();
  displayHeight = u8g2.getDisplayHeight();
  displayWidth = u8g2.getDisplayWidth();
  lineHeight = displayHeight/4;
  analogReference(INTERNAL1V1);
}

char buf[20];

void loop(void) {
  u8g2.clearBuffer();         // clear the internal memory
  for (int i=0; i<4; i++)
  {
    int row = i*lineHeight;
    u8g2.drawStr(0,row,itoa(maxCharHeight,buf,10));
    u8g2.drawStr(64,row,itoa(width,buf,10));
    u8g2.drawStr(96,row,itoa(height,buf,10));
  }
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);  
}
