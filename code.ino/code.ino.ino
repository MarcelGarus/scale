// Use the MD_MAX72XX library to display a Pacman animation
// Just for fun!

#include <MD_MAX72xx.h>
#include <HX711.h>
#include <SPI.h>

// First display: MD_MAX72XX
#define DISPLAY_CLK_PIN   13  // or SCK
#define DISPLAY_DATA_PIN  11  // or MOSI
#define DISPLAY_CS_PIN    10  // or SS

// Load cell: HX711
#define LC_DATA_PIN 7
#define LC_CLOCK_PIN 8

// Serial
void setupSerial() {
  Serial.begin(9600);
  Serial.println();
  Serial.println(__FILE__);
}

// Display
MD_MAX72XX display = MD_MAX72XX(MD_MAX72XX::PAROLA_HW, DISPLAY_CS_PIN, 12);
const uint8_t columnMapping[64] = {7,6,5,4,3,2,1,0,           15,14,13,12,11,10,9,8,    23,22,21,20,19,18,17,16,  31,30,29,28,27,26,25,24,
                                   39,38,37,36,35,34,33,32,   47,46,45,44,43,42,41,40,  55,54,53,52,51,50,49,48,  63,62,61,60,59,58,57,56};

void resetDisplay() {
  display.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
  display.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display.clear();
}
void setupDisplay() {
  display.begin();
  resetDisplay();
}

// Scale
HX711 scale;

void setupScale() {
  scale.begin(LC_DATA_PIN, LC_CLOCK_PIN);
  scale.set_offset(87074);
  scale.set_scale(208.319824);
}

// Main program
void setup() {
  setupSerial();
  setupDisplay();
  setupScale();
}

void loop() {
  // Serial.println("Test");
  display.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // clear old graphic
  // for (uint8_t i=0; i<DATA_WIDTH; i++)
  //   display.setColumn(idx-DATA_WIDTH+i, 0);
  for (uint8_t i=0; i < 64; i++)
    display.setColumn(columnMapping[i], i);

  display.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  Serial.println(scale.get_units(1));
}
