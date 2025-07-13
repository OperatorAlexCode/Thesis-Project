#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoBLE.h>
#include "Icons.h"
#include <Adafruit_NeoPixel.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <SPI.h>
#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
//#include <SSD1306Wire.h>

enum Item
{
  None,
  Medkit,
  Beer,
  Keycard
};

// Player 1
BLEService GamePawn("10e62b35-1ed8-4149-aeca-4df2e8b24132");
// Player 2
//BLEService GamePawn("d2d5dba7-9225-46b5-ab2e-ddef6cf090c8");

BLEStringCharacteristic RfidId(GamePawn.uuid(), BLERead | /*BLEWrite |*/ BLENotify, 14);
BLEBoolCharacteristic DisableScanner(GamePawn.uuid(), /*BLERead |*/ BLEWrite | BLENotify);
BLEBoolCharacteristic CallbackResponse(GamePawn.uuid(), BLERead | BLEWrite | BLENotify);
BLEIntCharacteristic HealthCharacteristic(GamePawn.uuid(), BLERead | /*BLEWrite |*/ BLENotify);
BLEIntCharacteristic AddItemCharacteristic(GamePawn.uuid(), /*BLERead |*/ BLEWrite);
BLEIntCharacteristic UseItemCharacteristic(GamePawn.uuid(), /*BLERead |*/ BLEWrite);
BLEByteCharacteristic Position(GamePawn.uuid(), BLERead | BLEWrite);
BLECharacteristic InventoryCharacteristic(GamePawn.uuid(), BLERead | BLEWrite, 3);
BLEIntCharacteristic TakeDamage(GamePawn.uuid(), BLEWrite);
BLEIntCharacteristic Heal(GamePawn.uuid(), BLEWrite);

MFRC522DriverPinSimple ss_pin(5);
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
MFRC522 mfrc522{driver};  

U8G2_SSD1306_128X64_NONAME_F_SW_I2C Screen(U8G2_R0,/*clock=*/22,/*data=*/21,U8X8_PIN_NONE);

Adafruit_NeoPixel HealthBar(8,15, NEO_GRB + NEO_KHZ800);

int Health = 0;
int MaxHealth = 6;

int itemsInInventory = 0;
int maxItems = 3;

int PlayerId = 1;

const int InventorySize = 3;
Item Inventory[3] /*= {Item::Medkit, Item::None, Item::Beer}*/;

byte lastUid[10];
byte lastSize = 0;

void setup() {
  // put your setup code here, to run once:
  Initialize();

  Serial.begin(115200);
  while (!Serial);
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522 board.
  //MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
  //Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  // begin initialization
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  String name = String("Player ");
  name += String(PlayerId);

  // set advertised local name and service UUID:
  BLE.setLocalName(name.c_str());
  BLE.setAdvertisedService(GamePawn);

  GamePawn.addCharacteristic(RfidId);
  GamePawn.addCharacteristic(DisableScanner);
  GamePawn.addCharacteristic(CallbackResponse);
  GamePawn.addCharacteristic(HealthCharacteristic);
  GamePawn.addCharacteristic(AddItemCharacteristic);
  GamePawn.addCharacteristic(UseItemCharacteristic);
  GamePawn.addCharacteristic(Position);
  GamePawn.addCharacteristic(InventoryCharacteristic);
  GamePawn.addCharacteristic(TakeDamage);
  GamePawn.addCharacteristic(Heal);

  HealthCharacteristic.writeValue(MaxHealth);
  DisableScanner.writeValue(false);

  BLE.addService(GamePawn);
  
  AddItemCharacteristic.setEventHandler(BLEWritten, AddItemEvent);
  UseItemCharacteristic.setEventHandler(BLEWritten, UseItemEvent);
  Position.setEventHandler(BLEWritten, PositionChangeEvent);
  TakeDamage.setEventHandler(BLEWritten, TakeDamageEvent);
  //HealthCharacteristic.setEventHandler(BLERead, TestEvent);
  Heal.setEventHandler(BLEWritten, HealEvent);

  // start advertising
  BLE.advertise();
  
  Serial.println("Trying to connect");
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
    UpdateHealthBar();
    UpdateDisplay();
    //HealthCharacteristic.writeValue((byte)Health);

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      if(isNewCard()) {
        Serial.println("Reading tag");
        Serial.println("Id: " + GetId());
        Serial.println(String("Scanner status: ") + String(DisableScanner.value()));

        if (!DisableScanner.value())
        {
          Serial.println("Sending tag");
          RfidId.writeValue(GetId());
        }

        else
          Serial.println("Scanner disabled");
      }
    }

    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    Serial.println("Trying to Reconnect");
    Screen.clearBuffer();
    Screen.drawBox(2, 2, 100, 64);
    Screen.sendBuffer();
  }

  //Serial.println("Failed to connect");
}

void Initialize()
{
  Screen.begin();
  Screen.setFont(u8g2_font_7x14B_tr);
  Screen.clearBuffer();
  Screen.drawBox(2, 2, 100, 50);
  Screen.sendBuffer();

  HealthBar.begin();

  for (int x = 0; x < GetArrayLength(sizeof(Inventory),sizeof(Inventory[0])); x++)
    Inventory[x] = Item::None;

  //for (int x = 0; x < InventorySize; x++)
  //  if (Inventory[x] != Item::None)
  //    itemsInInventory++;

  Health = MaxHealth;
  //UpdateHealthBar();
}

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
  //delay(2000);
}

String GetId() {
  String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) {
       uidString += "0"; 
      }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  return uidString;
}

bool isNewCard() {
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

void UpdateDisplay()
{
  Serial.println("Updating Display");
  Screen.clearBuffer();

  int xPos;
  int yPos;

  GetPosition(Position.value(), xPos, yPos);

  String string = String(PlayerId) + String("|");
  string += String("x:") + String(xPos) + String(",y:") + String(yPos);

  //Screen.drawStr(0, 10, (String("Player ")+String(PlayerId)).c_str());

  Screen.drawStr(0, 10, string.c_str());
  /*for (int x = 0; x < itemsInInventory; x++)
  {
    int difference = 42-32;
    int xPos = x * 42 + difference/2;
    int yPos = 16+difference/2;
    Screen.drawXBMP(xPos, yPos, 32, 32, ItemPlaceholder);
  }*/

  int items = 0;

  for (int x = 0; x < InventorySize; x++)
  {
    if (Inventory[x] != Item::None)
    {
      int difference = 42-32;
      int xPos = x * 42 + difference/2;
      int yPos = 16+difference/2;

      switch(Inventory[x])
      {
        case Item::Medkit:
          Screen.drawXBMP(xPos, yPos, 32, 32, MedKitIcon);
          break;
        case Item::Beer:
          Screen.drawXBMP(xPos, yPos, 32, 32, BeerIcon);
          break;
        case Item::Keycard:
          Screen.drawXBMP(xPos, yPos, 32, 32, KeycardIcon);
          break;
      }
    }
  }

  int marginTop = 1, marginSide = 1;
  int width = 8, height = 14;

  for (int x = 0; x < Health; x++)
  {
    int xPos = 128-(width+marginSide)*(x+1);

    Screen.drawBox(xPos, marginTop, width, height);
  }
  
  Screen.sendBuffer();
  mfrc522.PCD_Init();
}

void UpdateHealthBar()
{
  Serial.println("Updating Health Bar");
  HealthBar.clear();

  // Simple
  /*for (int x = 0; x < Health;x++)
  {
    HealthBar.setPixelColor(x,HealthBar.Color(20,0,0));
  */

  // Step Gradient
  /*for (int x = 0; x < Health;x++)
  {
    uint32_t color = HealthBar.Color(0, 20, 0);

    if (x < 2)
      color = HealthBar.Color(20, 0, 0);
    else if (x < 4)
      color = HealthBar.Color(20, 10, 0);
        
      HealthBar.setPixelColor(x, color);
  }*/

  // Whole Color Gradient
  /*uint32_t color = HealthBar.Color(20, 0, 0);

  if (Health > 1)
  color = HealthBar.Color(20/Health, (Health-1)*4, 0);

  for (int x = 0; x < 8;x++)
  {
    HealthBar.setPixelColor(x, color);
  }*/

  // True Gradient
  uint32_t color = HealthBar.Color(20, 0, 0);

  if (Health > 1)
    color = HealthBar.Color(20/(Health-1), (Health-1)*4, 0);

  for (int x = 0; x < Health;x++)
  {
    HealthBar.setPixelColor(x, color);
  }

  HealthBar.show();
}

void AddItemEvent(BLEDevice central, BLECharacteristic characteristic) {
  //Serial.print("Adding Item: ");
  Serial.println(AddItemCharacteristic.value());
  AddItem(AddItemCharacteristic.value());
  UpdateHealthBar();
  UpdateDisplay();

  byte newInv[3] = { (byte)Inventory[0], (byte)Inventory[1], (byte)Inventory[2] };
  InventoryCharacteristic.writeValue(newInv,3);
  //HealthCharacteristic.writeValue(Health);
}

void UseItemEvent(BLEDevice central, BLECharacteristic characteristic) {
  //Serial.print("Using Item: ");
  Serial.println(UseItemCharacteristic.value());
  UseItem(UseItemCharacteristic.value());
  UpdateHealthBar();
  UpdateDisplay();

  byte newInv[3] = { (byte)Inventory[0], (byte)Inventory[1], (byte)Inventory[2] };
  InventoryCharacteristic.writeValue(newInv,3);
  //HealthCharacteristic.writeValue(Health);
}

void PositionChangeEvent(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Position updated");
  UpdateDisplay();
  //HealthCharacteristic.writeValue(Health);
}

void TakeDamageEvent(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Taking damage");
  ChangeHealth(-TakeDamage.value());
  UpdateDisplay();
  UpdateHealthBar();
}

void TestEvent(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Health characteristic read");
}

void HealEvent(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("healing");
  ChangeHealth(Heal.value());
  UpdateDisplay();
  UpdateHealthBar();
}

void AddItem(Item itemToAdd)
{
  if (itemsInInventory < InventorySize)
    for (int x = 0; x < InventorySize;x++)
      {
        if (Inventory[x] == Item::None)
        {
          Inventory[x] = (Item)itemToAdd;
          itemsInInventory++;
          CallbackResponse.writeValue(true);
          return;
        }
      }
  
  CallbackResponse.writeValue(false);
}

void AddItem(int itemToAdd)
{
  AddItem((Item)itemToAdd);
}

void UseItem(int itemIndex)
{
  Serial.print("Using Item: ");
  Serial.print(itemIndex);
  bool itemUsed = false;
  if (itemIndex >= 0 && itemIndex < GetArrayLength(sizeof(Inventory),sizeof(Inventory[0])))
    switch(Inventory[itemIndex])
    {
      case Item::Medkit:
        //Health = constrain(Health+2,0,MaxHealth);
        Serial.println(String(" | Medkit"));
        ChangeHealth(2);
        itemUsed = true;
        break;
      case Item::Beer:
        //Health = constrain(Health-1,0,MaxHealth);
        Serial.println(String(" | Beer bottle"));
        ChangeHealth(1);
        itemUsed = true;
        break;
      case Item::Keycard:
        Serial.println(String(" | Key card"));
        itemUsed = true;
        break;
    }

  if (itemUsed)
  {
    //CallbackResponse.writeValue(Inventory[itemIndex]);
    Inventory[itemIndex] = Item::None;
    itemsInInventory--;
  }
  
  //else
  //  CallbackResponse.writeValue(0);

  CallbackResponse.writeValue(itemUsed);
}

int GetArrayLength(int arraySize, int byteSize)
{
  return arraySize/byteSize;
}

void ChangeHealth(int change) {
  Health += change;
  Health = constrain(Health,0,MaxHealth);
  HealthCharacteristic.writeValue((byte)Health);
}

void GetPosition(byte value, int &x, int &y) {
  x = (value & 0b11110000) >> 4;
  y = value & 0b00001111;
}