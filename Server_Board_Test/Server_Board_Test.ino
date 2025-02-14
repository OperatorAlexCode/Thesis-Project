#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <WiFiNINA.h>
#include <ArduinoBLE.h>

//U8G2_SH1107_SEEED_128X128_F_HW_I2C Screen(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const uint8_t testIcon[] = {
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
  0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
  0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
  0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, };

const int buttonPin = 3;
int buttonState = 0;
int state = 0;
bool isPressed = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  pinMode(LEDR, OUTPUT); 
  pinMode(buttonPin, INPUT);

  BLE.begin();
  
  Serial.println("Bluetooth® Low Energy Central - LED control");

  // start scanning for peripherals
  BLE.scanForUuid("10e62b35-1ed8-4149-aeca-4df2e8b24132");

  //Screen.begin();

  //BLE.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Screen.clearBuffer();
  //Screen.drawXBMP(0, 0, 32, 32, testIcon);
  //Screen.sendBuffer();
  //delay(100);

  BLEDevice pawn = BLE.available();

  if (pawn)
  {
    Serial.print("Found ");
    Serial.print(pawn.address());
    Serial.print(" '");
    Serial.print(pawn.localName());
    Serial.print("' ");
    Serial.print(pawn.advertisedServiceUuid());
    Serial.println();
    
    if (pawn.localName() != "Player 1")
      {
        Serial.println("Can't find pawn"); 
        return;
      }
    else
      {
        Serial.println("Found pawn"); 
        BLE.stopScan();
      }

    if (pawn.connect())
      {
        Serial.println("Connected"); 
        digitalWrite(LEDR, HIGH);
      }
    else
    {
      Serial.println("Can't connect"); 
      return;
    }

    if (pawn.discoverAttributes())
    Serial.println("Attributes discovered");
    else
    {
      pawn.disconnect();
      return;
    }

    BLECharacteristic characteristic = pawn.characteristic("10e62b35-1ed8-4149-aeca-4df2e8b24132");

    while (pawn.connected())
    {
      // Button toggle
      buttonState = digitalRead(buttonPin);

      if (buttonState == LOW && !isPressed)
      {
        //digitalWrite(LEDR, (PinStatus)((++state)%2));
        isPressed = true;
        characteristic.writeValue((byte)(0x01));
      }
      else if (buttonState == HIGH)
      {
        isPressed = false;
      }
    }
  }

  BLE.scanForUuid("10e62b35-1ed8-4149-aeca-4df2e8b24132");
  digitalWrite(LEDR, LOW);
}
