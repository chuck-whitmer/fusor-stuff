#include <Arduino.h>
#include <U8g2lib.h>
#include "FastTrig.h"
#include "LinearAlgebra.h"
#include "SinFit60Hz.h"
#include "stat.h"

#include <Wire.h>

#define variacAdcPin 0
#define nstAdcPin 1
#define cwAdcPin 2

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

int displayHeight;
int displayWidth;
int lineHeight;

double eInfo1 = 1.0;
double eInfo2 = 2.0;
double eInfo3 = 3.0;
long lastInfoMicros;
long nextTick;

stat cwOutput;
SinFit60Hz variac;
SinFit60Hz nst;

float DividerOffset(float r1, float r2, float rS, float rL, float v1);
float DividerMultiplier(float r1, float r2, float rS, float rL);

float variacOffset = 0.540; // DividerOffset(270.0, 33.0, 8.2e3, 3.3e6, 5.0); // Was 0.543.
float variacMultiplier = DividerMultiplier(270.0, 33.0, 8.2e3, 3.3e6);
float nstOffset = 0.542; // DividerOffset(270.0, 33.0, 8.2e3, 200e6, 5.0); // Was 0.545 but measured 0.542.
float nstMultiplier = DividerMultiplier(270.0, 33.0, 8.2e3, 200e6) / 1000.0; // Make it KV.
float cwOffset = 1.017; // DividerOffset(330.0, 82.0, 10e3, 400e6, 5.0); // Was 0.995, but measured 1.017
float cwMultiplier = DividerMultiplier(330.0, 82.0, 10e3, 400e6) / 1000.0; // Make it KV.
const float adcToVolts = 1.067 / 1023; // This device is not 1.1v. Depends on a particular diode.

const int tickStep = 250;

void setup(void) 
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.setFontPosTop();
  displayHeight = u8g2.getDisplayHeight();
  displayWidth = u8g2.getDisplayWidth();
  lineHeight = displayHeight/2;
  lastInfoMicros = micros()-1100000;
  analogReference(INTERNAL1V1); // ADCs compare to 1.1v
  nextTick = (millis()/tickStep)*tickStep+tickStep;
}

void loop(void) 
{
  // Read the variac voltage
  float variacReading = (analogRead(variacAdcPin)*adcToVolts - variacOffset) * variacMultiplier;
  variac.Accumulate(micros(), variacReading);

  // Read the NST voltage
  float nstReading = (analogRead(nstAdcPin)*adcToVolts - nstOffset) * nstMultiplier; // In KV.
  nst.Accumulate(micros(),nstReading);

  // Read the CW voltage
  float cwReading = (analogRead(cwAdcPin)*adcToVolts - cwOffset) * cwMultiplier; // In KV.
  cwOutput.accumulate(cwReading);
  
  if (millis()>=nextTick) 
  {
    double a,b,c;
    variac.SolveFit(a, b, c);
    eInfo1 = sqrt(a*a+b*b);
    nst.SolveFit(a, b, c);
    eInfo2 = sqrt(a*a+b*b);
    eInfo3 = cwOutput.average();

    variac.Reset();
    nst.Reset();
    cwOutput.Reset();
    
    UpdateDisplay();
    nextTick += tickStep;
  }
}

void UpdateDisplay(void)
{
  u8g2.clearBuffer();         // clear the internal memory
  
  static char buf[20];
  static int leftCol = displayWidth/2-10;
  static int rightCol = displayWidth;
  static int row1 = 0;
  static int row2 = displayHeight/2;
  
  int len;
  int slen;
   
  dtostrf(eInfo1,6,1,buf);
  len = u8g2.getStrWidth(buf);
  u8g2.drawStr(leftCol-len,row1,buf);
    
  dtostrf(eInfo2,6,1,buf);
  slen = strlen(buf);
  buf[slen++]='k'; buf[slen]='\0';
  len = u8g2.getStrWidth(buf);
  u8g2.drawStr(leftCol-len,row2,buf);
  
  dtostrf(eInfo3,6,1,buf);
  slen = strlen(buf);
  buf[slen++]='k'; buf[slen]='\0';
  len = u8g2.getStrWidth(buf);
  u8g2.drawStr(rightCol-len,row1,buf);
  
  dtostrf(millis()/1000.0,6,1,buf);
  len = u8g2.getStrWidth(buf);
  u8g2.drawStr(rightCol-len,row2,buf);

  u8g2.sendBuffer(); // transfer internal memory to the display    
}

/*
We measure high voltages with the assistance of a divider circuit.
There is a central node where three resistors connect.
R1 connects the node with a supply voltage, typically +5V.
R2 connects the node to ground.
R3 is the sum of a small resistor RS and a larger resistor RL, and it connects
the node to the high voltage we wish to measure.
The smaller voltage measured by the ADC is over R2+RS.
 */

float DividerOffset(float r1, float r2, float rS, float rL, float v1)
{
  float r3 = rS + rL;
  float rr123 = r1*r2 + r1*r3 + r2*r3;
  return v1*r2*rL/rr123;
}

float DividerMultiplier(float r1, float r2, float rS, float rL)
{
  float r3 = rS + rL;
  float rr123 = r1*r2 + r1*r3 + r2*r3;
  float rr12s = r1*r2 + r1*rS + r2*rS;
  return rr123 / rr12s;
}
