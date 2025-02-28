#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <SPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

U8G2_SSD1306_128X64_NONAME_F_SW_I2C
u8g2(U8G2_R0,/*clock=*/22,/*data=*/21,U8X8_PIN_NONE);

void setup() {
  u8g2.begin();
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  u8g2.setFont(u8g2_font_7x14B_tr);
  mfrc522.PCD_Init();    // Init MFRC522 board.
  SPI.begin(); // Init SPI bus
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  while(readID()) {
    dump();
    print();
    
    // delay(2000);
  }
  
  
}

// void loop() {
//   if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
//     return;
//   }

//   Serial.print("RFID tag detected: ");
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
//   }
//   Serial.println();
//   mfrc522.PICC_HaltA();
// }


// Dump debug info about the card; PICC_HaltA() is automatically called.
void dump()   {
  MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));
  //mfrc522.PCD_StopCrypto1();
}

void print()  {
  String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) {
       uidString += "0"; 
      }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
   Serial.println(uidString);
   u8g2.drawStr(0,10,uidString.c_str());
}


boolean readID()  {
    //mfrc522.PCD_StopCrypto1();
    //Check if a new tag is detected or not. If not return.
    // if (!mfrc522.PICC_IsNewCardPresent()){
    //   Serial.println("PICC_IsNewCardPresent");
    //   delay(2000);
    //   return false;
    // }
    // //Check if a new tag is readable or not. If not return.
    // if (!mfrc522.PICC_ReadCardSerial()){
    //   Serial.println("PICC_ReadCardSerial");
    //   delay(2000);
    //   return false;
    // }

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  // u8g2.clearDisplay();
  // delay(500);
  // u8g2.drawStr(0,10,"test");
  // u8g2.sendBuffer();
  // delay(500);
    
    mfrc522.PICC_HaltA(); // Stop reading
    return true;
}