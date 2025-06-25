#include <Arduino.h>
#include <U8g2lib.h>
#include <ArduinoBLE.h>
#include <sam_arduino.h>
//#include <AudioTools.h>
#include <vector>
//#include "TCA9548A.h"
//#include <Wire.h>
//#include <SoftwareWire.h>
#define SPEAKER 6

const uint8_t testIcon[] = {
  0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
  0x0F,0x0F,0x0F,0x0F,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x0F,0x0F,0x0F,0x0F,
  0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
  0xF0,0xF0,0xF0,0xF0,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
  0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0xF0,0xF0,0xF0,0xF0,
  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
  0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
  0x0F,0x0F,0x0F,0x0F,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
};

enum Room {
  None = 0,
  Bunks = 1,
  AiCore = 2,
  Kitchen = 3,
  Reactor = 4,
  Medbay = 5,
  CargoHold = 6,
  Armory = 7,
  LifeSupport = 8,
  RecyclingCenter = 9,
  RecreationalCenter = 10,
  DrillControls = 11,
  OreRefinery = 12,
  TrainingCenter = 13,
  Airlock = 14,
  Aquaponics = 15,
  Storage = 16
};

const int SIZE = 4;
int matrix[SIZE][SIZE];
int numbers[SIZE * SIZE];

void shuffleArray(int *array, int n) {
  for (int i = n - 1; i > 0; --i) {
    int j = random(i + 1);
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}


U8G2_SH1107_SEEED_128X128_F_HW_I2C Screen(U8G2_R3, /* reset=*/U8X8_PIN_NONE);

const int buttonPin = 3;
int buttonState = 0;
int state = 0;
bool isPressed = false;

const int buttonPins[] = { 3, 5 };
int buttonStates[2];

//I2SStream output;

const char* text = "Hello, nice to meet you";
String KeypadOutput = "";

//SAM Voice(Serial,true);
//SAM Voice(output);
int BassTab[] = { 1911, 1702, 1516, 1431, 1275, 1136, 1012 };

const char* Player1Id = "10e62b35-1ed8-4149-aeca-4df2e8b24132";
const char* Player2Id = "";
const char* ScreenControllerId = "646c3367-5f9c-4b50-bc95-4701b2d8ba50";

int PlayerTurn = 1;

//BLEDevice board;
//BLEDevice pawn1;
//BLEDevice pawn2;

void assignArray() {
  randomSeed(analogRead(0));

  // Fill array with numbers 1 to 16
  for (int i = 0; i < SIZE * SIZE - 1; ++i) {
    numbers[i] = i + 1;
  }
  numbers[SIZE * SIZE - 1] = 0;

  // Shuffle the numbers
  shuffleArray(numbers, SIZE * SIZE);

  // Fill the 4x4 matrix
  int index = 0;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      matrix[i][j] = numbers[index++];
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

  //pinMode(LEDR, OUTPUT);

  // Button inputs
  pinMode(buttonPins[0], INPUT);
  pinMode(buttonPins[1], INPUT);

  // Button LEDs
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);

  //pinMode(SPEAKER, OUTPUT);
  //digitalWrite(SPEAKER, LOW);

  /*auto cfg = output.defaultConfig();
  cfg.sample_rate = 22050;
  cfg.channels = 1;
  cfg.bits_per_sample = 16;
  cfg.pin_data = SPEAKER;
  output.begin(cfg);*/

  /*Voice.setVoice(SAM::Sam);
  Voice.setOutputChannels(1);
  Voice.say(text);
  delay(500);*/

  //BLE.begin();
  //assignArray();

  Serial.println("Bluetooth® Low Energy Central - Server Board");

  // start scanning for peripherals
  //BLE.scanForUuid(Player1Id);
  //BLE.scanForUuid(ScreenControllerId);
  //BLE.scan();

  //BLE.begin();

  //board = BLE.available();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Screen.clearBuffer();
  //Screen.drawXBMP(0, 0, 32, 32, testIcon);
  //Screen.sendBuffer();
  //delay(100);

  /*BLEDevice pawn = BLE.available();

  if (pawn) {
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
      //digitalWrite(LEDR, HIGH);
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

    BLECharacteristic button1 = pawn.characteristic(Player1Id, 0);
    BLECharacteristic button2 = pawn.characteristic(Player1Id, 1);
    BLECharacteristic keypad = pawn.characteristic(Player1Id, 2);

    while (pawn.connected())
    {
      // Button toggle
      buttonStates[0] = digitalRead(buttonPins[0]);
      buttonStates[1] = digitalRead(buttonPins[1]);
      int keypadOutput = GetKeypadOutput();

      if (buttonStates[0] == LOW && !isPressed)
      {
        Serial.println("Pressing Button 1");
        //digitalWrite(LEDR, (PinStatus)((++state)%2));
        isPressed = true;
        button1.writeValue((byte)(0x01));
        digitalWrite(2,HIGH);
      }
      else if (buttonStates[1] == LOW && !isPressed)
      {
        Serial.println("Pressing Button 2");
        isPressed = true;
        button2.writeValue((byte)(0x01));
        digitalWrite(4,HIGH);
      }
      else if (buttonStates[0] == HIGH && buttonStates[1] == HIGH)
      {
        //Serial.println("Resetting");
        isPressed = false;
        digitalWrite(2,LOW);
        digitalWrite(4,LOW);
      }

      if (keypadOutput != 0x00) 
      {
        if (keypadOutput == 0xEC)
        {
          Serial.print("Sending output:");
          Serial.print(KeypadOutput.toInt());
          Serial.println();
          keypad.writeValue((byte)(KeypadOutput.toInt()));
          KeypadOutput = "";
        } else if (keypadOutput != 0xEA)
        {
          switch (keypadOutput) {
            case 0xE1:
              KeypadOutput += "1";
              break;
            case 0xE2:
              KeypadOutput += "2";
              break;
            case 0xE3:
              KeypadOutput += "3";
              break;
            case 0xE4:
              KeypadOutput += "4";
              break;
            case 0xE5:
              KeypadOutput += "5";
              break;
            case 0xE6:
              KeypadOutput += "6";
              break;
            case 0xE7:
              KeypadOutput += "7";
              break;
            case 0xE8:
              KeypadOutput += "8";
              break;
            case 0xE9:
              KeypadOutput += "9";
              break;
            case 0xEB:
              KeypadOutput += "0";
              break;
          }
        } else {
          KeypadOutput = "";
        }
        
      }
    }

    BLE.scan();
  }*/

  //Serial.println(BLE.peripheralCount());

  BLEDevice board = BLE.available();
  BLEDevice pawn1;
  BLEDevice pawn2;
  
  Serial.println("Scanning for board");
  BLE.scanForUuid(ScreenControllerId);

  while (!board) {
    delay(100);
    board = BLE.available();
    //Serial.println("Scanning for board");
  }

  Serial.print("Found ");
  Serial.print(board.address());
  Serial.print(" '");
  Serial.print(board.localName());
  Serial.print("' ");
  Serial.print(board.advertisedServiceUuid());
  Serial.println();

  if (board.localName() != "") 
    BLE.stopScan();

  if (!board.connect())
  {
    Serial.println("Can't Connect");
    //board = NULL;
    return;
  }

  if (!board.discoverAttributes())
  {
    Serial.println("Can't discover attributes");
    board.disconnect();
    //board = NULL;
    return;
  }
  Serial.println("Connected to board");

  Serial.println("Scanning for pawn 1");
  BLE.scanForUuid(Player1Id);
  pawn1 = BLE.available();

  while (!pawn1) {
    delay(100);
    pawn1 = BLE.available();
    //Serial.println("Scanning for pawn 1");
  }

  Serial.print("Found ");
  Serial.print(pawn1.address());
  Serial.print(" '");
  Serial.print(pawn1.localName());
  Serial.print("' ");
  Serial.print(pawn1.advertisedServiceUuid());
  Serial.println();

  if (pawn1.localName() != "") 
    BLE.stopScan();

  //BLECharacteristic input = board.characteristic(ScreenControllerId,0);

  //setScreensTest(input);

  if (!pawn1.connect())
  {
    Serial.println("Can't Connect");
    //pawn1 = NULL;
    return;
  }

  if (!pawn1.discoverAttributes())
  {
    Serial.println("Can't discover attributes");
    pawn1.disconnect();
    //pawn1 = NULL;
    return;
  }
  
  Serial.println("Connected to Pawn 1");
  
  /*if (!pawn2) {
    BLE.scanForUuid(Player2Id);
    pawn2 = BLE.available();

    while (!pawn12) {
      delay(200);
      pawn2 = BLE.available();
      //Serial.println("Scanning for pawn 1");
    }

    Serial.print("Found ");
    Serial.print(pawn2.address());
    Serial.print(" '");
    Serial.print(pawn2.localName());
    Serial.print("' ");
    Serial.print(pawn2.advertisedServiceUuid());
    Serial.println();

    if (pawn2.localName() != "") 
      BLE.stopScan();

    //BLECharacteristic input = board.characteristic(ScreenControllerId,0);

    //setScreensTest(input);

    if (!pawn2.connect())
    {
      Serial.println("Can't Connect");
      pawn2 = NULL;
      return;
    }

    if (!pawn2.discoverAttributes())
    {
      Serial.println("Can't discover attributes");
      pawn2.disconnect();
      pawn2 = NULL;
      return;
    }
  }*/

  Serial.println("All devices connected");

  // Main game loop
  if (board && pawn1 /*&& pawn2*/)
  {
    Serial.println("Initializing characteristics");
    BLECharacteristic boardInput = board.characteristic(ScreenControllerId,0);
    
    BLECharacteristic player1button1 = pawn1.characteristic(Player1Id, 0);
    //BLECharacteristic player1button2 = pawn1.characteristic(Player1Id, 1);
    BLECharacteristic player1keypad = pawn1.characteristic(Player1Id, 1);
    BLECharacteristic readTag = pawn1.characteristic(Player1Id,2);

    //BLECharacteristic player2button1 = pawn2.characteristic(Player2Id, 0);
    //BLECharacteristic player2button2 = pawn2.characteristic(Player2Id, 1);
    //BLECharacteristic player2keypad = pawn2.characteristic(Player2Id, 2);
    
    //SetScreensTest(input);

    readTag.subscribe();

    InitializeBoard(boardInput);

    while (board.connected() && pawn1.connected() /*&& pawn2.connected()*/)
    {

      // Button toggle
      buttonStates[0] = digitalRead(buttonPins[0]);
      //buttonStates[1] = digitalRead(buttonPins[1]);
      buttonStates[1] = 1;
      int keypadOutput = GetKeypadOutput();

      if (buttonStates[0] == LOW && !isPressed)
      {
        Serial.println("Pressing Button 1");
        //digitalWrite(LEDR, (PinStatus)((++state)%2));
        isPressed = true;
        
        switch (PlayerTurn) {
          case 1:
            player1button1.writeValue((byte)(0x01));
            break;
          case 2:
            //player1button1.writeValue((byte)(0x01));
            break;
        }

        digitalWrite(2, HIGH);
      }
      /*else if (buttonStates[1] == LOW && !isPressed)
      {
        Serial.println("Pressing Button 2");
        isPressed = true;
        
        switch (PlayerTurn) {
          case 1:
            player1button2.writeValue((byte)(0x01));
            break;
          case 2:
            //player1button2.writeValue((byte)(0x01));
            break;
        }

        digitalWrite(4, HIGH);
      }*/

      else if (buttonStates[0] == HIGH && buttonStates[1] == HIGH)
      {
        //Serial.println("Resetting");
        isPressed = false;
        digitalWrite(2, LOW);
        digitalWrite(4, LOW);
      }

      if (keypadOutput != 0x00) {
        if (keypadOutput == 0xEC) {
          Serial.print("Sending output:");
          Serial.print(KeypadOutput.toInt());
          Serial.println();

          switch (PlayerTurn)
          {
            case 1:
              player1keypad.writeValue((byte)(KeypadOutput.toInt()));
              break;
            case 2:
              //player1button1.writeValue((byte)(KeypadOutput.toInt()));
              break;
          }
          
          KeypadOutput = "";
        } else if (keypadOutput != 0xEA) {
          switch (keypadOutput) {
            case 0xE1:
              KeypadOutput += "1";
              break;
            case 0xE2:
              KeypadOutput += "2";
              break;
            case 0xE3:
              KeypadOutput += "3";
              break;
            case 0xE4:
              KeypadOutput += "4";
              break;
            case 0xE5:
              KeypadOutput += "5";
              break;
            case 0xE6:
              KeypadOutput += "6";
              break;
            case 0xE7:
              KeypadOutput += "7";
              break;
            case 0xE8:
              KeypadOutput += "8";
              break;
            case 0xE9:
              KeypadOutput += "9";
              break;
            case 0xEB:
              KeypadOutput += "0";
              break;
          }
        } else {
          KeypadOutput = "";
        }
      }

      if (readTag.valueUpdated())
      {
        Serial.print("Pawn moved to: ");
        //String value = String(readTag.value());
        //readTag.readValue(value);
        String value = reinterpret_cast<const char *>(readTag.value());
        Serial.println(value);
      }
    }
  }

  //BLE.scanForUuid(Player1Id);

  if (board)
    board.disconnect();

  if (pawn1)
    pawn1.disconnect();

  BLE.scanForUuid(ScreenControllerId);

  //digitalWrite(LEDR, LOW);
}

int GetKeypadOutput() {
  int output = 0x00;

  if (Serial1.available()) {
    output = Serial1.read();
  }

  if (output != 0x00) {
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
  }

  return output;
}

/*int* RandomizeRooms()
{
  std::vector<int> availabelRooms = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

  int grid[16];

  for (int x = 1; x <= 16; x++)
  {
    switch(x)
    {
      case 1:
      grid[0] = 1;
      break;
      case 2:
      grid[15] = 2;
      break;
      default:
      break;
    }
  }
  
  return grid;
}*/

/*void callback(size_t len, int16_t *data16)
{
  for (size_t j=0;j<len;j++)
  {
    digitalWrite(SPEAKER,HIGH);
    delayMicroseconds((int)output);
    digitalWrite(SPEAKER,LOW);
    delayMicroseconds((int)output);
  }
}*/

void sound(uint8_t note_index) {
  for (int i = 0; i < 100; i++) {
    digitalWrite(SPEAKER, HIGH);
    delayMicroseconds(BassTab[note_index]);
    digitalWrite(SPEAKER, LOW);
    delayMicroseconds(BassTab[note_index]);
  }
}

void SetScreensTest(BLECharacteristic screens) {
  Serial.println("Writing to screens");

  // Test function
  for (int x = 0; x < 16; x++) {
    byte value = x << 4;
    Serial.print(value, BIN);
    value += x;
    Serial.print(" => ");
    Serial.print(value, BIN);
    
    Serial.print(" | ");
    Serial.print((int)screens.value());

    screens.writeValue(value);
    Serial.print(" => ");
    Serial.println((int)screens.value());
    delay(100);
  }
}

void InitializeBoard(BLECharacteristic screens) {
  assignArray();

  /*for (int x = 0; x < 16; x++)
  {
    byte value = x << 4;
    value += matrix[x%4][y];
    //value += numbers[x];
    Serial.println(value, BIN);
    screens.writeValue(value);
    delay(200);
  }*/

  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      {
        byte value = (y*4 + x) << 4;
        value += matrix[x][y];
        Serial.print(x);
        Serial.print(",");
        Serial.print(y);
        Serial.print(" | ");
        Serial.print(y*x + x);
        Serial.print(" | ");
        
        for (int z = 0; z < 8; z++)
          Serial.print(bitRead(value,z));
        
        Serial.println("");
        //Serial.println(value,BIN);
        screens.writeValue(value);
        delay(200);
      }
}