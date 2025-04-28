#include <Arduino.h>
#include <sam_arduino.h>
#include <AudioTools.h>
//#include <WiFiNINA.h>
#define SPEAKER 6

I2SStream output;
//PWM output;
const char* text = "Hello, nice to meet you";

//SAM Voice(Serial,true);
SAM Voice(output);

int BassTab[]={1911,1702,1516,1431,1275,1136,1012};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  pinMode(SPEAKER, OUTPUT);
  //digitalWrite(SPEAKER, LOW);

  auto cfg = output.defaultConfig();
  cfg.sample_rate = 22050;
  cfg.channels = 1;
  cfg.bits_per_sample = 16;
  cfg.pin_data = SPEAKER;
  output.begin(cfg);

  Voice.setVoice(SAM::Sam);
  Voice.setOutputChannels(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Speaking");
  Voice.say(text);
  Serial.println("Done Speaking");

  /*for(int note_index=0;note_index<7;note_index++)
    {
      sound(note_index);
      delay(500);
    }*/

  delay(1000);
}

/*void sound(uint8_t note_index)
{
    for(int i=0;i<100;i++)
    {
        digitalWrite(SPEAKER,HIGH);
        delayMicroseconds(BassTab[note_index]);
        digitalWrite(SPEAKER,LOW);
        delayMicroseconds(BassTab[note_index]);
    }
}*/