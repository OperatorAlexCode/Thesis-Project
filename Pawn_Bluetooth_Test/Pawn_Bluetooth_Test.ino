#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoBLE.h>

BLEService GamePawn("10e62b35-1ed8-4149-aeca-4df2e8b24132");

BLEIntCharacteristic switchCharacteristic("10e62b35-1ed8-4149-aeca-4df2e8b24132", BLERead | BLEWrite);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C
Screen(U8G2_R0,/*clock=*/22,/*data=*/21,U8X8_PIN_NONE);

const uint8_t testIcon[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0xC0, 
  0x03, 0x00, 0x00, 0xC0, 0x83, 0x07, 0xF8, 0xC7, 0x83, 0x07, 0xF8, 0xC7, 
  0x03, 0x03, 0xC0, 0xC0, 0x03, 0x03, 0xC0, 0xC0, 0x03, 0x03, 0xC0, 0xC0, 
  0x03, 0x03, 0xC0, 0xC0, 0x03, 0x03, 0xC0, 0xC0, 0x03, 0x03, 0xC0, 0xC0, 
  0x83, 0x07, 0xC0, 0xC0, 0x83, 0x07, 0xC0, 0xC0, 0x03, 0x00, 0x00, 0xC0, 
  0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xC0, 
  0xC3, 0x0F, 0x1C, 0xCE, 0xC3, 0x0F, 0x3C, 0xCF, 0xC3, 0x00, 0xFC, 0xCF, 
  0xC3, 0x00, 0xEC, 0xCD, 0xC3, 0x07, 0xCC, 0xCC, 0xC3, 0x07, 0xCC, 0xCC, 
  0xC3, 0x00, 0x0C, 0xCC, 0xC3, 0x00, 0x0C, 0xCC, 0xC3, 0x0F, 0x0C, 0xCC, 
  0xC3, 0x0F, 0x0C, 0xCC, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xC0, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };

int itemsInInventory = 0;
int maxItems = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Player 1");
  BLE.setAdvertisedService(GamePawn);

  GamePawn.addCharacteristic(switchCharacteristic);

  BLE.addService(GamePawn);

  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Screen.begin();
  Screen.setFont(u8g2_font_7x14B_tr);
  Screen.clearBuffer();
  Screen.drawBox(2, 2, 100, 50);
  Screen.sendBuffer();

  Serial.println("BLE Peripheral");
}

void loop() {
  // put your main code here, to run repeatedly:

    // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central)
  {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      if (switchCharacteristic.written())
      if (switchCharacteristic.value() == 1)
      {
        Increment();
        switchCharacteristic.writeValue(0);
      }

      // if the remote device wrote to the characteristic,
      Screen.clearBuffer();
      Screen.drawStr(0, 10, "Player 1");
      for (int x = 0; x < itemsInInventory; x++)
        {
          int difference = 42-32;
          int xPos = x * 42 + difference/2 ;
          int yPos = 16+difference/2;
          Screen.drawXBMP(xPos, yPos, 32, 32, testIcon);
        }

        Screen.sendBuffer();
        delay(500);
    }
  }

  Serial.print(F("Disconnected from central: "));
  Serial.println(central.address());
  Screen.clearBuffer();
  Screen.drawBox(2, 2, 100, 64);
  Screen.sendBuffer();
}

void Increment()
{
    itemsInInventory++;
    itemsInInventory = itemsInInventory % (maxItems+1);
}