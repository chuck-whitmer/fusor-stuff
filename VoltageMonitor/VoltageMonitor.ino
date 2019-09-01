#include <Arduino.h>
#include <U8g2lib.h>

#include <Wire.h>

class stat
{
  public:
  int n;
  float sum;
  float sum2;

  void Reset()
  {
    n = 0;
    sum = 0.0;
    sum2 = 0.0;
  }

  stat()
  {
    Reset();
  }

  void accumulate(float x)
  {
    n++;
    sum += x;
    sum2 += x*x;
  }

  float average()
  {
    return sum / n;
  }

  float variance()
  {
    float v = (sum2 - (sum * sum / n)) / (n-1);
    return v>0.0 ? v : 0.0;
  }

  float standardDeviation()
  {
    return sqrt(variance());
  }
};

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

stat variacOutput;
stat nstOutput;
stat cwOutput;

int displayHeight;
int displayWidth;
int lineHeight;
#define variacAdcPin 0
#define nstAdcPin 1
#define cwAdcPin 2

long nextDisplayUpdate;

float DividerOffset(float r1, float r2, float rS, float rL, float v1);
float DividerMultiplier(float r1, float r2, float rS, float rL);

float variacOffset = 0.540; // DividerOffset(270.0, 33.0, 8.2e3, 3.3e6, 5.0); // Was 0.543.
float variacMultiplier = DividerMultiplier(270.0, 33.0, 8.2e3, 3.3e6);
float nstOffset = 0.542; // DividerOffset(270.0, 33.0, 8.2e3, 200e6, 5.0); // Was 0.545 but measured 0.542.
float nstMultiplier = DividerMultiplier(270.0, 33.0, 8.2e3, 200e6) / 1000.0; // Make it KV.
float cwOffset = 1.017; // DividerOffset(330.0, 82.0, 10e3, 400e6, 5.0); // Was 0.995, but measured 1.017
float cwMultiplier = DividerMultiplier(330.0, 82.0, 10e3, 400e6) / 1000.0; // Make it KV.
const float adcToVolts = 1.067 / 1023; // This device is not 1.1v. Depends on a particular diode.

void setup(void) 
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r);
  u8g2.setFontPosTop();
  displayHeight = u8g2.getDisplayHeight();
  displayWidth = u8g2.getDisplayWidth();
  lineHeight = displayHeight/4;
  analogReference(INTERNAL1V1); // ADCs compare to 1.1v
  nextDisplayUpdate = millis() + 1000;
  Serial1.begin(9600);
}

#define MODE_HOLD 0
#define MODE_XMIT 1

void loop(void) 
{
  unsigned long timeStamp = micros();

  // Read the variac voltage
  // The variac divider uses 8.2k and 3.3m resistors. Scale by: (3.3m + 8.2k)/(8.2k)(1.1 / 1023) = 0.43381
  //  float variacReading = analogRead(variacAdcPin) * 0.43381;
  float variacReading = (analogRead(variacAdcPin)*adcToVolts - variacOffset) * variacMultiplier;
  variacOutput.accumulate(variacReading);  

  // Read the NST voltage
  // The NST divider uses 8.2k and 200m resistors. Scale by: (200m + 8.2k)/(8.2k)(1.1 / 1023) = 26.227
  //  float nstReading = analogRead(nstAdcPin) * 0.026227; // Make it KV
  float nstReading = (analogRead(nstAdcPin)*adcToVolts - nstOffset) * nstMultiplier; // In KV.
  nstOutput.accumulate(nstReading);

  // Read the CW voltage
  float cwReading = (analogRead(cwAdcPin)*adcToVolts - cwOffset) * cwMultiplier; // In KV.
  cwOutput.accumulate(cwReading);

  static int runMode = MODE_HOLD;

  if (Serial1.available())
  {
    char *command = ReadCommand();
    runMode = MODE_HOLD;
    if (command != NULL)
    {
      if (strcmp(command,"go")==0)
      {
        runMode = MODE_XMIT;
        Serial1.println("Start logging");
      }
      else if (strcmp(command,"stop")==0)
      {
        Serial1.println("Stop logging");    
      }
      else if (strcmp(command,"params")==0)
      {
        char fmtBuffer[20];
        Serial1.print("variac  ");
        dtostrf(variacOffset,8,3,fmtBuffer);
        Serial1.print(fmtBuffer);
        Serial1.print("  ");
        dtostrf(variacMultiplier,8,2,fmtBuffer);
        Serial1.println(fmtBuffer);
        Serial1.print("nst  ");
        dtostrf(nstOffset,8,3,fmtBuffer);
        Serial1.print(fmtBuffer);
        Serial1.print("  ");
        dtostrf(nstMultiplier,8,2,fmtBuffer);
        Serial1.println(fmtBuffer);
        Serial1.print("cw  ");
        dtostrf(cwOffset,8,3,fmtBuffer);
        Serial1.print(fmtBuffer);
        Serial1.print("  ");
        dtostrf(cwMultiplier,8,2,fmtBuffer);
        Serial1.println(fmtBuffer);
        Serial1.print("Internal ADC reference voltage:  ");
        dtostrf(adcToVolts*1023,8,3,fmtBuffer);
        Serial1.println(fmtBuffer);
      }
      else
      {
        Serial1.println("???");
      }
    }
  }

  if (millis() > nextDisplayUpdate)
  {
    nextDisplayUpdate += 500;
    UpdateDisplay(runMode == MODE_XMIT); // Also clears counters
  }
  delay(5);  
}

void UpdateDisplay(bool xmit)
{
    float variacAverage = variacOutput.average();
    float variacRMS = variacOutput.standardDeviation();
    int variacN = variacOutput.n;
    variacOutput.Reset();
    
    float nstAverage = nstOutput.average();
    float nstRMS = nstOutput.standardDeviation();
    int nstN = nstOutput.n;
    nstOutput.Reset();

    float cwAverage = cwOutput.average();
    float cwRMS = cwOutput.standardDeviation();
    cwOutput.Reset();

    u8g2.clearBuffer();         // clear the internal memory
    if (xmit) 
      Serial1.print(millis());

    char buf[20];
    int row = 0;
    int middleCol = displayWidth*3/5;
    u8g2.drawStr(0,row,"VAR");
    if (xmit)
    {
      Serial1.print(" VAR ");
      dtostrf(variacAverage,7,2,buf); // typical -1.00
      Serial1.print(buf);
    }
    dtostrf(variacAverage,6,1,buf);
    int len = u8g2.getStrWidth(buf);
    u8g2.drawStr(middleCol-len,row,buf);  
    if (xmit)
    {
      Serial1.print(" ");
      dtostrf(variacRMS,7,2,buf); // typical 140.00
      Serial1.print(buf);
    }
    dtostrf(variacRMS,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(displayWidth-len,row,buf);  

    row = lineHeight;
    u8g2.drawStr(0,row,"NST");
    if (xmit)
    {
      Serial1.print(" NST ");
      dtostrf(nstAverage,7,3,buf); // typical -0.100
      Serial1.print(buf);
    }
    dtostrf(nstAverage,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(middleCol-len,row,buf);  
    if (xmit)
    {
      Serial1.print(" ");
      dtostrf(nstRMS,6,3,buf); // typical 7.000
      Serial1.print(buf);
    }
    dtostrf(nstRMS,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(displayWidth-len,row,buf);  

    row = 2*lineHeight;
    u8g2.drawStr(0,row,"CW");
    if (xmit)
    {
      Serial1.print(" CW ");
      dtostrf(cwAverage,7,2,buf); // typical -41.00
      Serial1.print(buf);
    }
    dtostrf(cwAverage,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(middleCol-len,row,buf);  
    if (xmit)
    {
      Serial1.print(" ");
      dtostrf(cwRMS,5,2,buf); // typical 1.00
      Serial1.print(buf);
    }
    dtostrf(cwRMS,6,1,buf);
    len = u8g2.getStrWidth(buf);
    u8g2.drawStr(displayWidth-len,row,buf);  

    row = 3*lineHeight;
    u8g2.drawStr(0,row,itoa(variacN,buf,10));
    if (xmit)
    {
      Serial1.print("  ");
      Serial1.println(buf);
    }
    
    u8g2.sendBuffer();          // transfer internal memory to the display    
}

#define BUFSIZE 30

char *ReadCommand()
{
  static char readBuffer[BUFSIZE];
  static int bufPosition;  // Where the next character goes. 
  static bool needsReset = true;
  bool gotEnd = false;

  if (needsReset)
  {
    bufPosition = 0;
    needsReset = false;
  }
  
  while (Serial1.available())
  {
    char c = Serial1.read();
    if (c=='\r')
    {
      readBuffer[bufPosition] = '\0';
      needsReset = true;
      Serial1.write('\r');
      return readBuffer;
    }
    if (bufPosition < BUFSIZE-1)
    {
      readBuffer[bufPosition++] = c;
      Serial1.write(c); // echo
    }
  }
  return NULL;
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
