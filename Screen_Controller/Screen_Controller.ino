#include <Arduino.h>
#include <U8g2lib.h>
#include <vector>
#include "TCA9548A.h"
#include <Wire.h>
//#include <SoftwareWire.h>
//#include <SoftwareI2C.h>
#include <ArduinoBLE.h>
#include "RoomIcons.h"

U8G2_SH1107_SEEED_128X128_F_HW_I2C Screen(U8G2_R3, /* reset=*/ U8X8_PIN_NONE);

BLEService ScreenController("646c3367-5f9c-4b50-bc95-4701b2d8ba50");

BLEByteCharacteristic SetTileRoom(ScreenController.uuid(), BLERead | BLEWrite);
BLEByteCharacteristic SetTileState(ScreenController.uuid(), BLERead | BLEWrite);

enum Room
{
  Bunks = 0,
  AiCore = 1,
  Kitchen = 2,
  Reactor = 3,
  Medbay = 4,
  CargoHold = 5,
  Armory = 6,
  LifeSupport = 7,
  RecyclingCenter = 8,
  RecreationalCenter = 9,
  DrillControls = 10,
  OreRefinery = 11,
  TrainingCenter = 12,
  Airlock = 13,
  Aquaponics = 14,
  Storage = 15
};

enum RoomState {
  None,
  Locked,
  GasLeak
};

Room RoomSprites[16];
//oomState RoomStates[16];

//SoftwareWire Wire1(8,7);
//SoftwareWire Wire2(6,5);

TCA9548A Multi(0x70);
TCA9548A Multi2(0x71);

//TCA9548A<SoftwareWire> Multi1;
//TCA9548A<SoftwareWire> Multi2;

//TCA9548A<TwoWire> Multi1;
//TCA9548A<TwoWire> Multi2;

#define WIRE1 Wire
//#define WIRE2 Wire

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Serial open");

  while(!BLE.begin());

  Serial.println("BLE begun");

  WIRE1.begin();
  //Wire2.begin();

  Multi.begin(WIRE1);
  Multi2.begin(WIRE1);

  Multi.openAll();
  Multi2.openAll();
  Screen.begin();
  Screen.setFont(u8g2_font_7x14B_tr);
  Screen.clearBuffer();
  Multi.closeAll();
  Multi2.closeAll();

  BLE.setLocalName("Screen Controller");
  BLE.setAdvertisedService(ScreenController);

  SetTileRoom.setEventHandler(BLEWritten,SetTileRoomEvent);
  SetTileState.setEventHandler(BLEWritten,SetTileStateEvent);

  ScreenController.addCharacteristic(SetTileRoom);
  ScreenController.addCharacteristic(SetTileState);

  BLE.addService(ScreenController);

  BLE.advertise();
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.println("Connected to central");
    while (central.connected()) {
    }
  }
}

void GetValues(byte recievedValue, byte &value, byte &index) {
  value = 0b00001111 & recievedValue;
  index = (0b11110000 & recievedValue) >> 4;
}

void SetTileRoomEvent(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("Recieved value:");
  for (int x = 0; x < 8; x++)
    Serial.print(bitRead(SetTileRoom.value(),x));

  Serial.println(" | Set Room");

  byte value /*= 0b00001111 & SetTileRoom*/;
  byte index /*= (0b11110000 & SetTileRoom) >> 4*/;

  GetValues(SetTileRoom.value(),value,index);

  for (int x = 0; x < 4; x++)
    Serial.print(bitRead(value,x));
  
  Serial.print(" | ");
  
  for (int x = 0; x < 4; x++)
    Serial.print(bitRead(index,x));

  Serial.println("");

  if (index < 16 && value < 16)
    SetTile(index,(Room)value);
}

void SetTileStateEvent(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("Recieved value:");
  for (int x = 0; x < 8; x++)
    Serial.print(bitRead(SetTileState.value(),x));

  Serial.println(" | Set State");

  byte value /*= 0b00001111 & SetTileRoom*/;
  byte index /*= (0b11110000 & SetTileRoom) >> 4*/;

  GetValues(SetTileState.value(),value,index);

  for (int x = 0; x < 4; x++)
    Serial.print(bitRead(value,x));
  
  Serial.print(" | ");
  
  for (int x = 0; x < 4; x++)
    Serial.print(bitRead(index,x));

  Serial.println("");

  if (index < 16)
    SetTile(index, RoomSprites[index],(RoomState)value);
}

void Clear() {
  Multi.openAll();
  Multi2.openAll();

  //Multi1.openChannel(TCA_CHANNEL_0);
  //Multi1.openChannel(TCA_CHANNEL_1);
  //Multi1.openChannel(TCA_CHANNEL_2);

  /*Screen1.clearBuffer();
  Screen1.drawXBMP(0, 0, 32, 32, testIcon);
  Screen1.sendBuffer();*/

  Screen.clearDisplay();

  Multi.closeAll();
  Multi2.closeAll();
}

void SetTile(int tileToSet, Room room) {
  SetTile(tileToSet, room, RoomState::None);
}

void SetTile(int tileToSet, Room room, RoomState state) {
  if (tileToSet >= 8)
    Multi2.openChannel(tileToSet%8);

  else
    Multi.openChannel(tileToSet%8);

  Screen.clearBuffer();

  switch(room) 
  {
    case Room::Bunks:
      Screen.drawXBMP(0,0,128,128,bunks);
      break;
    case Room::AiCore:
      Screen.drawXBMP(0,0,128,128,aiCore);
      break;
    case Room::Kitchen:
      Screen.drawXBMP(0,0,128,128,kitchen);
      break;
    case Room::Reactor:
      Screen.drawXBMP(0,0,128,128,reactor);
      break;
    case Room::Medbay:
      Screen.drawXBMP(0,0,128,128,medbay);
      break;
    case Room::CargoHold:
      Screen.drawXBMP(0,0,128,128,cargoHold);
      break;
    case Room::Armory:
      Screen.drawXBMP(0,0,128,128,armory);
      break;
    case Room::LifeSupport:
      Screen.drawXBMP(0,0,128,128,lifeSupport);
      break;
    case Room::RecyclingCenter:
      Screen.drawXBMP(0,0,128,128,recyclingCenter);
      break;
    case Room::RecreationalCenter:
      Screen.drawXBMP(0,0,128,128,recreationalCenter);
      break;
    case Room::DrillControls:
      Screen.drawXBMP(0,0,128,128,drillControls);
      break;
    case Room::OreRefinery:
      Screen.drawXBMP(0,0,128,128,oreRefinery);
      break;
    case Room::TrainingCenter:
      Screen.drawXBMP(0,0,128,128,trainingCenter);
      break;
    case Room::Airlock:
      Screen.drawXBMP(0,0,128,128,airLock);
      break;
    case Room::Aquaponics:
      Screen.drawXBMP(0,0,128,128,aquaponics);
      break;
    case Room::Storage:
      Screen.drawXBMP(0,0,128,128,storage);
      break;
    default:
      String output = String("Screen ") + String(tileToSet);
      Screen.drawStr(0, 10, output.c_str());
      break;
  }

  switch (state) {
    case RoomState::Locked:
      for (int x = 0; x < 3; x++)
        Screen.drawFrame(x, x, 128-x*2, 128-x*2);
      
      //Screen.drawUTF8(0, 10, “🔒”);
      break;
    case RoomState::GasLeak:
      int circleradius = 8;
      int circleCenter = circleradius+1;

      Screen.drawDisc(0,0,circleradius);
      Screen.drawDisc(127,0,circleradius);
      Screen.drawDisc(0,127,circleradius);
      Screen.drawDisc(127,127,circleradius);

      for (int x = 0; x < 8; x++)
      {
        Screen.drawDisc(x*16, 0, circleradius);
        Screen.drawDisc(x*16, 127, circleradius);
        Screen.drawDisc(0, x*16, circleradius);
        Screen.drawDisc(127, x*16, circleradius);
      }

      //Screen.drawDisc(circleCenter,circleCenter,circleradius);
      //Screen.drawDisc(128-circleCenter,circleCenter,circleradius);
      //Screen.drawDisc(circleCenter,128-circleCenter,circleradius);
      //Screen.drawDisc(128-circleCenter,128-circleCenter,circleradius);
      break;
  }

  Screen.sendBuffer();

  if (tileToSet >= 8)
    Multi2.closeChannel(tileToSet%8);

  else
    Multi.closeChannel(tileToSet%8);

  RoomSprites[tileToSet] = room;

  //delay(10);
}