// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <SPI.h>
#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <U8g2lib.h>
#include <ArduinoBLE.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        15 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel HealthBar(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
U8G2_SSD1306_128X64_NONAME_F_SW_I2C Screen(U8G2_R0,/*clock=*/22,/*data=*/21,U8X8_PIN_NONE);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

MFRC522DriverPinSimple ss_pin(5);
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
MFRC522 mfrc522{driver};

int Health = 0;
int MaxHealth = 6;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  HealthBar.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  Screen.begin();
  Screen.setFont(u8g2_font_7x14B_tr);
  Screen.clearBuffer();
  Screen.drawBox(2, 2, 100, 50);
  Screen.sendBuffer();

  mfrc522.PCD_Init();
}

void loop() {
  //pixels.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  /*for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }*/

  if (Health == 0)
    Health = MaxHealth;

  else
    Health--;

  UpdateHealthBar();

  delay(DELAYVAL);
}

void UpdateHealthBar()
{
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