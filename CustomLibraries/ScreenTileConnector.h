#ifndef ScreenTileConnector_h
#define ScreenTileConnector_h
#include "Arduino.h"
#include <vector>

class ScreenPlugBoard
{
  std::vector<ValuePair> Screens;

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