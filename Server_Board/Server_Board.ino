#include <Arduino.h>
#include <U8g2lib.h>
#include <ArduinoBLE.h>
//#include <sam_arduino.h>
//#include <AudioTools.h>
#include <vector>
//#include "TCA9548A.h"
//#include <Wire.h>
//#include <SoftwareWire.h>
#define SPEAKER 6

/*class State {
  public:
  void Enter() {
  }

  void Update() {
  }

  void Exit() {
  }
}*/

enum Room {
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

const char* RoomNames[] {
  "Bunks",
  "AI Core",
  "Kitchen",
  "Reactor",
  "Medbay",
  "Cargo Hold",
  "Armory",
  "Life Support",
  "Recycling Center",
  "Recreational Center",
  "Drill Controls",
  "Ore Refinery",
  "Training Center",
  "Air lock",
  "Aquaponics",
  "Storage"
};

/*enum RoomNames[] {
  Bunks = "Bunks",
  AiCore = "AI Core",
  Kitchen = "Kitchen",
  Reactor = "Reactor",
  Medbay = "Medbay",
  CargoHold = "Cargo Hold",
  Armory = "Armory",
  LifeSupport = "Life Support",
  RecyclingCenter = "Recycling Center",
  RecreationalCenter = "Recreational Center",
  DrillControls = "Drill Controls",
  OreRefinery = "Ore Refinery",
  TrainingCenter = "Training Center",
  Airlock = "Air lock",
  Aquaponics = "Aquaponics",
  Storage = "Storage"
};*/

enum Item {
  None,
  Medkit,
  Beer
};

enum TurnPhase {
  Moving,
  InRoom,
  UseItem,
  NextAction,
  NextTurn
};

const char* Player1Id = "10e62b35-1ed8-4149-aeca-4df2e8b24132";
const char* Player2Id = "d2d5dba7-9225-46b5-ab2e-ddef6cf090c8";
const char* ScreenControllerId = "646c3367-5f9c-4b50-bc95-4701b2d8ba50";
const int SIZE = 4;

/*String Ids[SIZE][SIZE] = {
  { String("53123f2aa00001"), String("placeholder001"), String("534c4a2aa00001"), String("ff0ff20d5c0000") },
  { String("53751c2aa00001"), String("536e242aa00001"), String("ff0f820c5c0000"), String("ff0ff40d5c0000") },
  { String("53850f2aa00001"), String("53652e2aa00001"), String("ff0ff10d5c0000"), String("5399132aa00001") },
  { String("53764f2aa00001"), String("5331452aa00001"), String("534d182aa00001"), String("ff0ff30d5c0000") }
};*/

String Ids[SIZE][SIZE] = {
  { String("53123f2aa00001"), String("53751c2aa00001"), String("53850f2aa00001"), String("53764f2aa00001") },
  { String("placeholder001"), String("536e242aa00001"), String("53652e2aa00001"), String("5331452aa00001") },
  { String("534c4a2aa00001"), String("ff0f820c5c0000"), String("ff0ff10d5c0000"), String("534d182aa00001") },
  { String("ff0ff20d5c0000"), String("ff0ff40d5c0000"), String("5399132aa00001"), String("ff0ff30d5c0000") }
};

int matrix[SIZE][SIZE];
int numbers[SIZE * SIZE - 2]; // reservered for bunks/aiCore

int Player1PosX = 0, Player1PosY = 0;
int Player2PosX = 0, Player2PosY = 0;

int ActionsPerTurn = 2;
int ActionsLeft;
TurnPhase CurrentPhase = TurnPhase::Moving;

String KeypadOutput = "";

//I2SStream output;

//SAM Voice(Serial,true);
//SAM Voice(output);
//const char* text = "Hello, nice to meet you";
//int BassTab[] = { 1911, 1702, 1516, 1431, 1275, 1136, 1012 };

int PlayerTurn = 1;

bool GameFinished = false;

void shuffleArray(int *array, int n) {
  for (int i = n - 1; i > 0; --i) {
    int j = random(i + 1);
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}

void assignArray() {
  //randomSeed(analogRead(0));

  // Fill array with numbers 1 to 16
  for (int i = 0; i < SIZE * SIZE - 1; ++i) {
    numbers[i] = i + 2;
  }
  //numbers[SIZE * SIZE - 1] = 0;

  // Shuffle the numbers
  shuffleArray(numbers, SIZE * SIZE -2);

  // Fill the 4x4 matrix
  int index = 0;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      if (i == 0 && j == 0) {
        matrix[i][j] = 0;
      } else if (i == SIZE - 1 && j == SIZE - 1) {
        matrix[i][j] = 1;
      } else {
        matrix[i][j] = numbers[index++];
      }
    }
  }

  // Print the matrix
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      Serial.print(matrix[i][j]);
      Serial.print("\t");
    }
    Serial.println();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  Serial1.begin(9600);
  while (!Serial1);

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  Serial.println("Server initialized. Starting Game");

  assignArray();
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice board;
  BLEDevice pawn1;
  BLEDevice pawn2;

  Serial.println("Scanning for board");
  //if (!ConnectToPeripheral(ScreenControllerId,board))
  //  return;
  
  Serial.println("Scanning for pawn 1");
  if (!ConnectToPeripheral(Player1Id,pawn1))
    return;

  Serial.println("All devices connected");

  // Main game loop
  if (/*board &&*/ pawn1 /*&& pawn2*/)
  {
    //Serial.println("Initializing characteristics");
    //BLECharacteristic boardInput = board.characteristic(ScreenControllerId,0);
    
    BLECharacteristic scannedTag;
    BLECharacteristic disableScanner;
    BLECharacteristic callback;
    BLECharacteristic health;
    BLECharacteristic addItem;
    BLECharacteristic useItem;
    BLECharacteristic position;
    
    switch (PlayerTurn) {
          case 1:
            scannedTag = pawn1.characteristic(Player1Id, 0);
            disableScanner = pawn1.characteristic(Player1Id, 1);
            callback = pawn1.characteristic(Player1Id, 2);
            health = pawn1.characteristic(Player1Id, 3);
            addItem = pawn1.characteristic(Player1Id, 4);
            useItem = pawn1.characteristic(Player1Id, 5);
            position = pawn1.characteristic(Player1Id, 6);
            position.writeValue((byte)(Player1PosX << 4 + Player1PosY));
            break;
          case 2:
            scannedTag = pawn2.characteristic(Player2Id, 0);
            disableScanner = pawn2.characteristic(Player2Id, 1);
            callback = pawn2.characteristic(Player2Id, 2);
            health = pawn2.characteristic(Player2Id, 3);
            addItem = pawn2.characteristic(Player2Id, 4);
            useItem = pawn2.characteristic(Player2Id, 5);
            position = pawn2.characteristic(Player2Id, 6);
            position.writeValue((byte)(Player2PosX << 4 + Player2PosY));
            break;
        }

    //SetScreensTest(input);

    scannedTag.subscribe();
    callback.subscribe();

    //UpdateBoard(boardInput);

    Serial.println(String("Player ")+String(PlayerTurn)+String("'s turn"));

    while (/*board.connected() &&*/ pawn1.connected() /*&& pawn2.connected()*/ && !GameFinished)
    {
      String keypadOutput = GetKeypadOutputString();

      switch(CurrentPhase) {
        case TurnPhase::Moving:
            if (scannedTag.valueUpdated())
            {
              //Serial.print("Pawn moved to: ");
              //String value = String(readTag.value());
              //readTag.readValue(value);
              String value = reinterpret_cast<const char *>(scannedTag.value());

              if (value.length() > 14)
                value = value.substring(0,14);

              //Serial.println(value);

              // Move to room if it is adjacent and is able to be entered (i.e not locked or such)
              if (IsAdjacent(value, PlayerTurn, false))
              {
                int posX = 0;
                int posY = 0;

                GetPosition(value, posX, posY);

                switch (PlayerTurn)
                {
                  case 1:
                    Player1PosX = posX;
                    Player1PosY = posY;
                    break;
                  case 2:
                    Player2PosX = posX;
                    Player2PosY = posY;
                    break;
                }

                position.writeValue((byte)((posX << 4) + posY));

                Serial.print("Moving to: ");
                Serial.println(RoomNames[matrix[posX][posY]]);
                //Serial.println(String(" | ") + String(posX) + ", " + String(posY));
                CurrentPhase = TurnPhase::InRoom;
                ActionsLeft = ActionsPerTurn;
                disableScanner.writeValue((byte)1);
                Serial.println(String("Actions left: ")+String(ActionsLeft) +String(", What do you want to do?"));
              }

              else
                Serial.println("Room is not Adjacent!");
            }
            else if (keypadOutput == "*")
            {
              int posX = 0;
              int posY = 0;

              switch (PlayerTurn)
                {
                  case 1:
                    posX = Player1PosX;
                    posY = Player1PosY;
                    break;
                  case 2:
                    posX = Player2PosX;
                    posY = Player2PosY;
                    break;
                }

              Serial.print("Staying in: ");
              Serial.println(RoomNames[matrix[posX][posY]]);
              //Serial.println(String(" | ") + String(posX) + ", " + String(posY));
              CurrentPhase = TurnPhase::InRoom;
              ActionsLeft = ActionsPerTurn;
              disableScanner.writeValue((byte)1);
              Serial.println("What do you want to do?");
              Serial.println(String("Actions left: ")+String(ActionsLeft));
            }
          break;
        case TurnPhase::InRoom:
          if (keypadOutput != "")
          {
            if (keypadOutput == "#") {
              switch (KeypadOutput.toInt())
              {
                // Search
                case 1:
                  Serial.print("Searching Room");
                  // Simulates searching room
                  for (int x = 0; x < 3; x++)
                  {
                    delay(300);
                    Serial.print(". ");
                  }

                  if (random(0,10) <= 6)
                  {
                    Serial.println("\nFound Item!");
                    addItem.writeValue((byte)random(1,3));

                    while (!callback.valueUpdated());
                    //delay(200);

                    bool value = callback.value();

                    if (value)
                    {
                      Serial.println("Item added to inventory");
                    }
                    else
                      Serial.println("Unable to add item to inventoy!");
                  }
                  else
                    Serial.println("No Item Found");

                  if (--ActionsLeft == 0)
                  {
                    Serial.println("Next Players turn");
                    CurrentPhase = TurnPhase::NextTurn;
                  }
                  else
                    Serial.println(String("Actions left: ")+String(ActionsLeft));

                  break;
                // Use
                case 2:
                CurrentPhase = TurnPhase::UseItem;
                Serial.println("What item do you want to use?");
                break;
                // Nothing
                case 3:
                Serial.println("Skipping turn");
                ActionsLeft = 0;
                CurrentPhase = TurnPhase::NextTurn;
                break;
                // Special
                case 4:
                break;
              }

              KeypadOutput = "";
            } else if (keypadOutput != "*") {
              KeypadOutput += keypadOutput;
            } else {
              KeypadOutput = "";
            }
          }
          break;
        case TurnPhase::UseItem:
          if (keypadOutput != "")
          {
            if (keypadOutput == "#")
            {
              useItem.writeValue((byte)(max(0,KeypadOutput.toInt()-1)));
              
              while (!callback.valueUpdated());
              //delay(200);

              bool value = callback.value();

              if (value)
              {
                Serial.println("Using Item");
                ActionsLeft--;
              }
              else
                Serial.println("Item not found");
              
              if (ActionsLeft == 0)
              {
                Serial.println("Next turn");
                CurrentPhase = TurnPhase::NextTurn;
              }
              else
              {
                Serial.println(String("Actions left: ")+String(ActionsLeft));
                CurrentPhase = TurnPhase::InRoom;
              }

              KeypadOutput = "";
            } else if (keypadOutput != "*") {
              KeypadOutput += keypadOutput;
            } else {
              KeypadOutput = "";
            }
          }
          break;
        case TurnPhase::NextTurn:
          /*if (PlayerTurn = 2)
            PlayerTurn = 1;
          else
            PlayerTurn++;*/

          switch (PlayerTurn) {
            case 1:
              scannedTag = pawn1.characteristic(Player1Id, 0);
              disableScanner = pawn1.characteristic(Player1Id, 1);
              callback = pawn1.characteristic(Player1Id, 2);
              health = pawn1.characteristic(Player1Id, 3);
              addItem = pawn1.characteristic(Player1Id, 4);
              useItem = pawn1.characteristic(Player1Id, 5);
              position = pawn1.characteristic(Player1Id, 6);
              position.writeValue((byte)((Player1PosX << 4) + Player1PosY));
              break;
            case 2:
              scannedTag = pawn2.characteristic(Player2Id, 0);
              disableScanner = pawn2.characteristic(Player2Id, 1);
              callback = pawn2.characteristic(Player2Id, 2);
              health = pawn2.characteristic(Player2Id, 3);
              addItem = pawn2.characteristic(Player2Id, 4);
              useItem = pawn2.characteristic(Player2Id, 5);
              position = pawn2.characteristic(Player2Id, 6);
              position.writeValue((byte)((Player2PosX << 4) + Player2PosY));
              break;
          }
          
          ActionsLeft = ActionsPerTurn;
          disableScanner.writeValue((byte)0);
          Serial.println("Where do you want to move?");
          CurrentPhase = TurnPhase::Moving;
        break;
      }

      // Turn order:
      // 1. Player moved to other room (or not)
      // 2. Player performs action(s)
      // - Search | Look for items. May not be guaranteed
      // - Use | Uses item in inventory
      // - Nothing | Skips action/turn
      // - Special | Unique action for the tile (not all tiles have unique actions)
      // 3. If action is performed, result is displayed
      // 4. Move onto the next player, go to step 1
      // 5. If player is last, AI performs action
      // 6. Display result of AI event
      // 7. If players haven't lost, move onto first player
    }
  }

  //BLE.scanForUuid(Player1Id);

  if (board)
    board.disconnect();

  if (pawn1)
    pawn1.disconnect();

  if (pawn2)
    pawn2.disconnect();

  //BLE.scanForUuid(ScreenControllerId);

  //Stops game once finished
  while (GameFinished);
}

bool IsAdjacent(String scannedID, int player, bool move) {
  switch (player) {
    case 1:
      if(Player1PosY - 1 >= 0){
        if(Ids[Player1PosX][Player1PosY - 1] == scannedID){
          if(move)
            Player1PosY--;
          return true;
        }
      }
        if(Player1PosY  + 1 < SIZE){
          if(Ids[Player1PosX][Player1PosY + 1] == scannedID){
            if(move)
              Player1PosY++;
          return true;
        }
      }
      if(Player1PosX - 1 >= 0){
          if(Ids[Player1PosX - 1][Player1PosY] == scannedID){
            if(move)
              Player1PosX--;
            return true;
        }
      }
      if(Player1PosX  + 1 < SIZE){
        if(Ids[Player1PosX + 1][Player1PosY] == scannedID){
          if(move)
            Player1PosX++;
          return true;
          }
      }
    return false;
    case 2:
      if(Player2PosY - 1 >= 0){
        if(Ids[Player2PosX][Player2PosY - 1] == scannedID){
          if(move)
            Player1PosY--;
          return true;
        }
      }
        if(Player2PosY  + 1 < SIZE){
          if(Ids[Player2PosX][Player2PosY + 1] == scannedID){
            if(move)
              Player1PosY++;
            return true;
        }
      }
        if(Player2PosX - 1 >= 0){
          if(Ids[Player2PosX - 1][Player2PosY] == scannedID){
            if(move)
              Player1PosX--;
            return true;
        }
      }
        if(Player2PosX  + 1 < SIZE){
          if(Ids[Player2PosX + 1][Player2PosY] == scannedID){
            if(move)
              Player1PosX++;
            return true;
        }
      }
        //player1button1.writeValue((byte)(0x01));
    return false;
  }
            
}

bool GetPosition(String id, int &xOut, int &yOut) {
  for (int x = 0; x < SIZE; x++)
    for (int y = 0; y < SIZE; y++)
      if (Ids[x][y] == id)
      {
        xOut = x;
        yOut = y;
        return true;
      }
  
  return false;
}

/*Room GetRoom(int x, int y) {
  return matrix[x][y];
}*/

int GetKeypadOutput() {
  int output = 0x00;

  if (Serial1.available()) {
    output = Serial1.read();
  }

  /*if (output != 0x00) {
    Serial.print("Reading keyboard output:");
    switch (output) {
      case 0xE1:
        Serial.print("1");
        break;
      case 0xE2:
        Serial.print("2");
        break;
      case 0xE3:
        Serial.print("3");
        break;
      case 0xE4:
        Serial.print("4");
        break;
      case 0xE5:
        Serial.print("5");
        break;
      case 0xE6:
        Serial.print("6");
        break;
      case 0xE7:
        Serial.print("7");
        break;
      case 0xE8:
        Serial.print("8");
        break;
      case 0xE9:
        Serial.print("9");
        break;
      case 0xEA:
        Serial.print("*");
        break;
      case 0xEB:
        Serial.print("0");
        break;
      case 0xEC:
        Serial.print("#");
        break;
    }
    Serial.println();
  }*/

  return output;
}

String GetKeypadOutputString() {
  String output = "";

  if (Serial1.available()) {
    //Serial.print("Reading keyboard output:");
    switch (Serial1.read()) {
      case 0xE1:
        output = String("1");
        break;
      case 0xE2:
        output = String("2");
        break;
      case 0xE3:
        output = String("3");
        break;
      case 0xE4:
        output = String("4");
        break;
      case 0xE5:
        output = String("5");
        break;
      case 0xE6:
        output = String("6");
        break;
      case 0xE7:
        output = String("7");
        break;
      case 0xE8:
        output = String("8");
        break;
      case 0xE9:
        output = String("9");
        break;
      case 0xEA:
        output = String("*");
        break;
      case 0xEB:
        output = String("0");
        break;
      case 0xEC:
        output = String("#");
        break;
    }
  }

  return output;
}

void UpdateBoard(BLECharacteristic screens) {
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      {
        byte value = (y*4 + x) << 4;
        value += matrix[x][y];
        //Serial.print(x);
        //Serial.print(",");
        //Serial.print(y);
        //Serial.print(" | ");
        //Serial.print(y*x + x);
        //Serial.print(" | ");
        
        //for (int z = 0; z < 8; z++)
        //  Serial.print(bitRead(value,z));
        
        //Serial.println("");
        //Serial.println(value,BIN);
        screens.writeValue(value);
        delay(200);
      }
}

// Tries connecting to peripheral. Returns true if connecting is succesfull, otherwise it's false.
bool ConnectToPeripheral(const char *id,BLEDevice &deviceToConnect) {
  BLE.scanForUuid(id);
  BLEDevice peripheral = BLE.available();

  while (!peripheral) {
    delay(100);
    peripheral = BLE.available();
    //Serial.println("Scanning for pawn 1");
  }

  //Serial.print("Found ");
  //Serial.print(peripheral.address());
  //Serial.print(" '");
  //Serial.print(peripheral.localName());
  //Serial.print("' ");
  //Serial.print(peripheral.advertisedServiceUuid());
  //Serial.println();

  if (peripheral.localName() != "") 
    BLE.stopScan();

  if (!peripheral.connect())
  {
    Serial.println("Can't Connect");
    //pawn1 = NULL;
    return false;
  }

  if (!peripheral.discoverAttributes())
  {
    Serial.println("Can't discover attributes");
    peripheral.disconnect();
    //pawn1 = NULL;
    return false;
  }
  
  Serial.println("Connection established");

  deviceToConnect = peripheral;
  return true;
}