#include "M5StickCPlus.h"
#include "BluetoothSerial.h"
#include "ELMduino.h"

int alarmTemp = 97;

int cycle = 0;
int temp = 0;
int prevTemp = 0;
int prevBgColor = BLACK;
int bgColor = BLACK;
BluetoothSerial SerialBT;
ELM327 myELM327;

void setup(){
  M5.begin();
  Serial.begin(115200);
  Serial.println("Starting up...");
  // Connect to the bluetooth device
  SerialBT.begin("M5TempMon", true);
  if (!SerialBT.connect("OBDII"))
  {
    Serial.println("Couldn't connect to OBD scanner - Phase 1");
    while(1);
  }
  // Handshake with with the ELM
  if (!myELM327.begin(SerialBT, true, 2000))
  {
    Serial.println("Couldn't connect to OBD scanner - Phase 2");
    while (1);
  }
  Serial.println("Connected to ELM327");
}

void loop() {
  M5.update(); // Read the press state of the key
  float tempFloat = myELM327.engineCoolantTemp();
  // We got a reading
  if (myELM327.status == ELM_SUCCESS)
  {
    temp = (int)tempFloat;
    Serial.print("Temp: ");
    Serial.println(temp);
    
    // Over temperature
    if (temp >= alarmTemp) 
    {
      M5.Beep.tone(3000); 
      if (cycle % 2) 
      {
        bgColor = BLACK;
        M5.Lcd.setTextColor(RED);
      }
      else
      {
        bgColor = RED;
        M5.Lcd.setTextColor(BLACK);
      }
      cycle++;
    }
    // Normal temperature
    else
    {
      bgColor = BLACK;
      M5.Lcd.setTextColor(WHITE);
      M5.Beep.mute();
    }     
  }
  else 
  {
    Serial.println("Couldn't get coolant temp");
    M5.Beep.mute();
    if (cycle % 2) 
    {
      bgColor = BLACK;
      M5.Lcd.setTextColor(YELLOW);
    }
    else
    {
      bgColor = YELLOW;
      M5.Lcd.setTextColor(BLACK);
    }
    cycle++;
  }
  // Refresh the LCD if needed
  if (prevBgColor != bgColor) 
  {
    M5.Lcd.fillScreen(bgColor);
    prevBgColor = bgColor;
  }
  // Also refresh if our temp changed
  if (prevTemp != temp)
  {
    M5.Lcd.fillScreen(bgColor);
    prevTemp = temp;
  }
  if (temp > 99)
  {
    M5.Lcd.setTextSize(5);
  }
  else 
  {
    M5.Lcd.setTextSize(7);
  }
  M5.Lcd.drawString(String(temp), 16, 60, 2);
  delay(500);
 }
