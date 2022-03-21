#include <Arduino.h>
#include <U8g2lib.h>
#include "FastTrig.h"
#include "LinearAlgebra.h"
#include "SinFit60Hz.h"

#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

int displayHeight;
int displayWidth;
int lineHeight;

double eInfo1 = 1.0;
double eInfo2 = 2.0;
double eInfo3 = 3.0;
long lastInfoMicros;
long nextTick;

void setup(void) 
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.setFontPosTop();
  displayHeight = u8g2.getDisplayHeight();
  displayWidth = u8g2.getDisplayWidth();
  lineHeight = displayHeight/2;
  lastInfoMicros = micros()-1100000;
  nextTick = (millis()/100)*100+100;
}

void loop(void) 
{
  
  if (millis()>=nextTick) 
  {
    UpdateDisplay();
    nextTick += 100;
  }
}

void UpdateDisplay(void)
{
  u8g2.clearBuffer();         // clear the internal memory
  
  static char buf[20];
  static int leftCol = displayWidth/3;
  static int middleCol = displayWidth*2/3;
  int row = 0;
  int len;
 
  dtostrf(eInfo1,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(leftCol-len,row,buf);
    
  dtostrf(eInfo2,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(middleCol-len,row,buf);
  
  dtostrf(eInfo3,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(displayWidth-len,row,buf);
  
  row = displayHeight/2;
  dtostrf(millis()/1000.0,6,1,buf);
  len = u8g2.getStrWidth(buf);
  u8g2.drawStr(middleCol-len,row,buf);

  u8g2.sendBuffer(); // transfer internal memory to the display    
}
