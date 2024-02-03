// TODO: Refactor to use snake case

#include <MD_MAX72xx.h>
#include <HX711.h>
#include <SPI.h>

// Cable colors:
// - red: positive
// - brown/blue: ground/negative
// - green: display clock
// - yellow: display control (CS)
// - orange: display data

// First display: MD_MAX72XX
#define DISPLAY_1_CLK_PIN   10
#define DISPLAY_1_DATA_PIN  8
#define DISPLAY_1_CS_PIN    9

// Second display: MD_MAX72XX
#define DISPLAY_2_CLK_PIN   13
#define DISPLAY_2_DATA_PIN  11
#define DISPLAY_2_CS_PIN    12

// Load cell: HX711
#define LC_DATA_PIN 6
#define LC_CLOCK_PIN 7

// Serial
void setupSerial() {
  Serial.begin(9600);
  Serial.println();
  Serial.println(__FILE__);
}

// Display
MD_MAX72XX display1 = MD_MAX72XX(MD_MAX72XX::PAROLA_HW, DISPLAY_1_DATA_PIN, DISPLAY_1_CLK_PIN, DISPLAY_1_CS_PIN, 12);
MD_MAX72XX display2 = MD_MAX72XX(MD_MAX72XX::PAROLA_HW, DISPLAY_2_DATA_PIN, DISPLAY_2_CLK_PIN, DISPLAY_2_CS_PIN, 12);

void setupDisplays() {
  display1.begin();
  display1.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 4);
  display1.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display1.clear();

  display2.begin();
  display2.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 4);
  display2.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display2.clear();
}

// Scale
HX711 scale;

void setupScale() {
  scale.begin(LC_DATA_PIN, LC_CLOCK_PIN);
  scale.set_offset(87074);
  scale.set_scale(208.319824);
}

// Rendering
#define DISPLAY_WIDTH 128
uint8_t pixels[DISPLAY_WIDTH];

void pushPixelsToDisplays() {
  display1.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  display2.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    if (x < DISPLAY_WIDTH / 2) {
      // Print on left display. It consists of 8x8 blocks and the columns are
      // are addressed in a weird way:
      // 7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8 ...
      int mapped_x = x / 8 * 8 + 7 - (x % 8);
      //             ^^^^^^^^^   ^^^^^^^^^^^
      //             in which    offset in
      //             block?      block
      display1.setColumn(mapped_x, pixels[x]);
    } else {
      // Print on right display. This is flipped upside down so that the
      // connector is on the right, so the column bits need to be flipped.
      // It uses the same weird block-based addressing.
      int x_in_right = DISPLAY_WIDTH / 2 - (x % (DISPLAY_WIDTH / 2)) - 1;
      int mapped_x = x_in_right / 8 * 8 + 7 - (x_in_right % 8);
      // TODO: explain
      int flipped = 0;
      for (int j = 0; j < 8; j++) flipped |= (pixels[x] >> j & 1) << (7 - j);
      display2.setColumn(mapped_x, flipped);
    }
  }

  display1.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display2.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
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
  setupDisplays();
  setupScale();
  stableWeight = scale.get_units(1);
}

void loop() {
  unsigned long now = millis();

  Serial.println("Updating " + String(now));
  clearPixels();
  renderSmolText(1, 1, "HALLI HALLO DAS IST EIN TEST");
  pushPixelsToDisplays();

  return;

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
  s += " AND SOME MORE TEXT";
  // renderSmolText(1, 0, s);
  renderSmolText(1, 0, "RUCKSACK, LAPTOP, IPAD, 900ML WASSER");
  // int len = renderSmolText(1, int(fluctuation / 10.), "KAL?");
  // renderTinyText(0, 0, "123456789");
  // renderTinyText(0, 5, String(weight));
  // shiftPixels();
  pushPixelsToDisplays();
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
