#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoBLE.h>
#include "Icons.h"
#include <Adafruit_NeoPixel.h>

enum Item
{
  None,
  Medkit,
  Beer
};

BLEService GamePawn("10e62b35-1ed8-4149-aeca-4df2e8b24132");

BLEIntCharacteristic Button1Characteristic("10e62b35-1ed8-4149-aeca-4df2e8b24132", BLERead | BLEWrite);
BLEIntCharacteristic Button2Characteristic("10e62b35-1ed8-4149-aeca-4df2e8b24132", BLERead | BLEWrite);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C Screen(U8G2_R0,/*clock=*/22,/*data=*/21,U8X8_PIN_NONE);

Adafruit_NeoPixel HealthBar = Adafruit_NeoPixel(8,15);

int Health = 0;
int MaxHealth = 6;

int itemsInInventory = 0;
int maxItems = 3;

int PlayerId = 1;

const int InventorySize = 3;
Item Inventory[3] /*= {Item::Medkit, Item::None, Item::Beer}*/;

void setup() {
  // put your setup code here, to run once:
  Initialize();

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

  GamePawn.addCharacteristic(Button1Characteristic);
  GamePawn.addCharacteristic(Button2Characteristic);

  BLE.addService(GamePawn);
  Button1Characteristic.writeValue(0);
  Button2Characteristic.writeValue(0);

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
    UpdateDisplay();

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      HealthBar.clear();

      // Simple
      /*for (int x = 0; x < Health;x++)
      {
        HealthBar.setPixelColor(x,HealthBar.Color(20,0,0));
      }*/

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

      // Button 1
      if (Button1Characteristic.written())
      if (Button1Characteristic.value() == 1)
      {
        AddItemTest();
        Button1Characteristic.writeValue(0);
        UpdateDisplay();
      }

      // Button 2
      if (Button2Characteristic.written())
      if (Button2Characteristic.value() == 1)
      {
        UseItemTest();
        //UseItem(random(0,InventorySize));
        Button2Characteristic.writeValue(0);
        UpdateDisplay();
      }

      // if the remote device wrote to the characteristic,
    }

    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    Serial.println("Trying to Reconnect");
    Screen.clearBuffer();
    Screen.drawBox(2, 2, 100, 64);
    Screen.sendBuffer();
  }
}

void Initialize()
{
  HealthBar.begin();

  Screen.begin();
  Screen.setFont(u8g2_font_7x14B_tr);
  Screen.clearBuffer();
  Screen.drawBox(2, 2, 100, 50);
  Screen.sendBuffer();

  for (int x = 0; x < GetArrayLength(sizeof(Inventory),sizeof(Inventory[0])); x++)
    Inventory[x] = Item::None;

  for (int x = 0; x < InventorySize; x++)
    if (Inventory[x] != Item::None)
    itemsInInventory++;

  Health = MaxHealth;
}

void UpdateDisplay()
{
  Screen.clearBuffer();
  Screen.drawStr(0, 10, "Player 1");
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
      int xPos = items++ * 42 + difference/2;
      int yPos = 16+difference/2;

      switch(Inventory[x])
      {
        case Item::Medkit:
          Screen.drawXBMP(xPos, yPos, 32, 32, MedKitIcon);
          break;
        case Item::Beer:
          Screen.drawXBMP(xPos, yPos, 32, 32, BeerIcon);
          break;
      }
    }
  }

  int marginTop = 1, marginSide = 1;
  int width = 10, height = 14;

  for (int x = 0; x < Health; x++)
  {
    int xPos = 128-(width+marginSide)*(x+1);

    Screen.drawBox(xPos, marginTop, width, height);
  }
  
  Screen.sendBuffer();
}

void Increment()
{
  itemsInInventory++;
  itemsInInventory = itemsInInventory % (maxItems+1);
}

void AddItemTest()
{
  if (itemsInInventory+1 <= InventorySize)
    AddItem((Item)random(1,3));

  else
  {
    for (int x = 0; x < InventorySize; x++)
      Inventory[x] = Item::None;
    
    itemsInInventory = 0;
  }

  Health--;
  
  if (Health < 0)
    Health = MaxHealth;
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
          break;
        }
      }
}

void AddItem(int itemToAdd)
{
  AddItem((Item)itemToAdd);
}

void UseItem(int itemIndex)
{
  bool itemUsed = false;
  switch(Inventory[itemIndex])
  {
    case Item::Medkit:
    Health = constrain(Health+2,0,MaxHealth);
    itemUsed = true;
    break;
    case Item::Beer:
    Health = constrain(Health-1,0,MaxHealth);
    itemUsed = true;
    break;
  }

  if (itemUsed)
  {
    Inventory[itemIndex] = Item::None;
    itemsInInventory--;
  }
}

void UseItemTest()
{
  for (int x = 0; x < InventorySize; x++)
    if (Inventory[x] != Item::None)
    {
      UseItem(x);
      break;
    }
}

int GetArrayLength(int arraySize, int byteSize)
{
  return arraySize/byteSize;
}