#ifndef ScreenTileConnector_h
#define ScreenTileConnector_h
#include "Arduino.h"

class ScreenPlugBoard
{
  public:


}

class ValuePair
{
  public:
  int Key;
  string Value;

  ValuePair(int key, string value)
  {
    Key = key;
    Value = value;
  }
}

#endif