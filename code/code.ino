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

// Rendering
#define DISPLAY_WIDTH 64
uint8_t pixels[DISPLAY_WIDTH];

void pushPixelsToDisplay() {
  display.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (int i = 0; i < DISPLAY_WIDTH; i++)
    display.setColumn(columnMapping[i], pixels[i]);
  display.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}
void clearPixels() {
  for (int i = 0; i < DISPLAY_WIDTH; i++) pixels[i] = 0b00000000;
}
void renderPixel(int x, int y, bool pixel) {
  if (x < 0 || x >= DISPLAY_WIDTH) return;
  if (y < 0 || y >= 8) return;
  if (pixel)
    *(pixels + x) |= 1 << (7 - y); // something like 0b00010000
  else
    *(pixels + x) &= (1 << (7 - y)) ^ 0xFF; // something like 0b11101111
}
void shiftPixels() {
  double t = 1.2 * double(millis() % 2000) / 1000.;
  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    double a = 0.2 * (double(x - DISPLAY_WIDTH / 2) - 50. * t);
    double shiftA = 4. * pow(2., -5. * t) * pow(2., -a*a);

    double b = 0.2 * (double(DISPLAY_WIDTH / 2 - x) - 50. * t);
    double shiftB = 4. * pow(2., -5. * t) * pow(2., -b*b);

    pixels[x] = pixels[x] >> int(max(shiftA, shiftB));
  }
}

// Main program

float stableWeight = 0;
float candidate = 0; // candidate for a new stable weight
unsigned long candidateWhen = 0;
#define MAX_THINGS 128
float things[MAX_THINGS];
int numThings = 0;

void setup() {
  setupSerial();
  setupDisplay();
  setupScale();
  stableWeight = scale.get_units(1);
}

void loop() {
  unsigned long now = millis();
  float weight = scale.get_units(1);

  update(now, weight);

  clearPixels();
  // renderText(15, 0, "0123456789");
  // renderText(15, 0, "Gewicht: 2g");
  // renderText(8, 0, "Rucksack");
  String s = "";
  for (int i = 0; i < numThings; i++) {
    s += (i == 0 ? "" : ",\x1") + String(round(things[i]));
  }
  float fluctuation = weight - stableWeight;
  s += (fluctuation >= 0 ? "+" : "\x2") + String(abs(fluctuation)) + "g";
  // int offset = int((sin(float(millis()) / 400.0) + 1) * 30);
  // renderText(1, 0, s);
  renderText(1, 0, "Rucksack");
  // shiftPixels();
  pushPixelsToDisplay();
}

void update(unsigned long now, float weight) {
  if (abs(weight - candidate) > 5) {
    // This is a bad candidate, it didn't last for a second.
    candidate = weight;
    candidateWhen = now;
    return;
  }
  
  if (now - candidateWhen < 1000) {
    // Wait some more time to see if something disproves the candidate.
    return;
  }

  // The candidate was good for one whole second.
  // Let's pick it!
  float delta = candidate - stableWeight;
  stableWeight = candidate;
  candidate = weight;
  candidateWhen = now;

  // We now that the delta was put on (or removed from) the scale.
  if (abs(delta) < 10) {
    // Just fluctuation. Perhaps the load cell slightly shifts over time because of temperature changes.
    return;
  }

  Serial.println(String(delta) + " put on the scale.");

  if (delta > 0) {
    // Something was put on the scale.
    if (numThings > MAX_THINGS) {
      return; // Too bad.
    }
    things[numThings] = delta;
    numThings++;
  } else {
    // Something was taken from the scale.
    float removed = -delta;
    int thing = -1;
    for (int i = 0; i < numThings; i++)
      if (thing == -1 || abs(things[i] - removed) < abs(things[thing] - removed))
        thing = i;
    if (thing == -1) return; // Nothing was on the scale.

    for (int i = thing + 1; i < numThings; i++)
      things[i - 1] = things[i];
    numThings--;
  }
}
