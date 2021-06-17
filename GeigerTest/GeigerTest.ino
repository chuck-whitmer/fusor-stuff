#include <Arduino.h>
#include <U8g2lib.h>

#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

int displayHeight;
int displayWidth;
int lineHeight;

int info1, info2;
long lastInfoMicros;
long nextTick;
void setup(void) 
{
  u8g2.begin();
//  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r);
//  u8g2.setFont(u8g2_font_9x15_tr);
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.setFontPosTop();
  displayHeight = u8g2.getDisplayHeight();
  displayWidth = u8g2.getDisplayWidth();
  lineHeight = displayHeight/2;
  Serial3.begin(9600);
  lastInfoMicros = micros()-1100000;
  nextTick = (millis()/100)*100+100;
}

void loop(void) 
{
  static bool haveByte1 = false;
  static int byte1, byte2;
  static long time1;

  if (Serial3.available())
  {
    if (!haveByte1)
    {
      byte1 = Serial3.read();
      haveByte1 = true;
      time1 = micros();
    }
    else
    {
      byte2 = Serial3.read();
      long time2 = micros();
      if (time2-time1<100000)
      {
        haveByte1 = false;
        lastInfoMicros = time2;
        info1 = byte1;
        info2 = byte2;
        UpdateDisplay();
      }
      else
      {
        byte1 = byte2;
        time1 = time2;
        haveByte1 = true;
      }
    }
  }
  
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
  if (micros()-lastInfoMicros < 1000000)
  {
    sprintf(buf, "%d", info1);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(leftCol-len,row,buf);
    
    sprintf(buf, "%d", info2);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(middleCol-len,row,buf);
  }
  row = displayHeight/2;
  dtostrf(millis()/1000.0,6,1,buf);
  len = u8g2.getStrWidth(buf);
  u8g2.drawStr(middleCol-len,row,buf);

  u8g2.sendBuffer(); // transfer internal memory to the display    
}
