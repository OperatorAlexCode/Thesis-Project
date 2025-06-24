#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <SPI.h>
#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
//#include <SSD1306Wire.h>

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);
//SSD1306Wire display(0x3c, 21, 22);
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

byte lastUid[10];
byte lastSize = 0;

void setup() {
  //display.init();
  //display.flipScreenVertically();
  //display.setFont(ArialMT_Plain_10);
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
   SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522 board.

 
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  // display.clear();                       
  // display.drawString(0,0,"goodbye world");
  // display.display();                 //fuckar rfid läsaren
  if(isNewCard()) { 
    dump();
    print();
  }
  delay(20);
}

// Dump debug info about the card; PICC_HaltA() is automatically called.
void dump()   {
  MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));
}

void print()  {
  //display.clear();
  String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) {
       uidString += "0"; 
      }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(uidString);
}


boolean isNewCard()  {
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      return false;
  }
    // Compare with last UID
  if (mfrc522.uid.size != lastSize || memcmp(mfrc522.uid.uidByte, lastUid, mfrc522.uid.size) != 0) {
    memcpy(lastUid, mfrc522.uid.uidByte, mfrc522.uid.size);
    lastSize = mfrc522.uid.size;
    mfrc522.PICC_HaltA();
    return true;
  }

  mfrc522.PICC_HaltA();
  return false;
}