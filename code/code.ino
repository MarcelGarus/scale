// TODO: Refactor to use snake case

#include <MD_MAX72xx.h>
#include <HX711.h>
#include <SPI.h>

// Displays: MD_MAX72XX
#define DISPLAY_LEFT_CLK_PIN   10
#define DISPLAY_LEFT_DATA_PIN  8
#define DISPLAY_LEFT_CS_PIN    9
#define DISPLAY_RIGHT_CLK_PIN  13
#define DISPLAY_RIGHT_DATA_PIN 11
#define DISPLAY_RIGHT_CS_PIN   12

// Load cells: HX711
#define LC_FRONT_LEFT_DATA_PIN  2
#define LC_FRONT_LEFT_SCK_PIN   3
#define LC_FRONT_RIGHT_DATA_PIN 4
#define LC_FRONT_RIGHT_SCK_PIN  5
#define LC_BACK_LEFT_DATA_PIN   A0
#define LC_BACK_LEFT_SCK_PIN    A1
#define LC_BACK_RIGHT_DATA_PIN  A2
#define LC_BACK_RIGHT_SCK_PIN   A3

void setup_pins() {
  // Because of pin shortage, we use some analog pins as digital pins.
  // https://forum.arduino.cc/t/arduino-uno-using-analog-input-as-digital-in-out/143837
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
}

// Serial
void setup_serial() {
  Serial.begin(9600);
  Serial.println();
  Serial.println(__FILE__);
}

// Displays
MD_MAX72XX display_left  = MD_MAX72XX(MD_MAX72XX::PAROLA_HW,
  DISPLAY_LEFT_DATA_PIN, DISPLAY_LEFT_CLK_PIN, DISPLAY_LEFT_CS_PIN, 12);
MD_MAX72XX display_right = MD_MAX72XX(MD_MAX72XX::PAROLA_HW,
  DISPLAY_RIGHT_DATA_PIN, DISPLAY_RIGHT_CLK_PIN, DISPLAY_RIGHT_CS_PIN, 12);

void setup_displays() {
  display_left.begin();
  display_left.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 4);
  display_left.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display_left.clear();

  display_right.begin();
  display_right.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 4);
  display_right.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display_right.clear();
}

// Load cells
HX711 lc_front_left;
HX711 lc_front_right;
HX711 lc_back_left;
HX711 lc_back_right;

void setup_load_cells() {
  lc_front_left.begin(LC_FRONT_LEFT_DATA_PIN, LC_FRONT_LEFT_SCK_PIN);
  lc_front_left.set_offset(-19500);
  lc_front_left.set_scale(200.8);
  lc_front_right.begin(LC_FRONT_RIGHT_DATA_PIN, LC_FRONT_RIGHT_SCK_PIN);
  lc_front_right.set_offset(90200);
  lc_front_right.set_scale(200.8);
  lc_back_left.begin(LC_BACK_LEFT_DATA_PIN, LC_BACK_LEFT_SCK_PIN);
  lc_back_left.set_offset(-162800);
  lc_back_left.set_scale(187.4);
  lc_back_right.begin(LC_BACK_RIGHT_DATA_PIN, LC_BACK_RIGHT_SCK_PIN);
  lc_back_right.set_offset(-245700);
  lc_back_right.set_scale(200.8);
}

// Rendering
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 8
uint8_t pixels[DISPLAY_WIDTH]; // Each entry is 8 bits for an entire column

void show_pixels() {
  display_left.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  display_right.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    if (x < DISPLAY_WIDTH / 2) {
      // Print on left display. It consists of 8x8 blocks and the columns are
      // are addressed in a weird way:
      // 7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8 ...
      int mapped_x = x / 8 * 8 + 7 - (x % 8);
      //             ^^^^^^^^^   ^^^^^^^^^^^
      //             in which    offset in
      //             block?      block
      display_left.setColumn(mapped_x, pixels[x]);
    } else {
      // Print on right display. This is flipped upside down so that the
      // connector is on the right, so the column bits need to be flipped.
      // It uses the same weird block-based addressing.
      int x_in_right = DISPLAY_WIDTH / 2 - (x % (DISPLAY_WIDTH / 2)) - 1;
      int mapped_x = x_in_right / 8 * 8 + 7 - (x_in_right % 8);
      //             ^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      //             in which     offset in block
      //             block?
      int flipped = 0;
      for (int j = 0; j < 8; j++) flipped |= (pixels[x] >> j & 1) << (7 - j);
      display_right.setColumn(mapped_x, flipped);
    }
  }

  display_left.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  display_right.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}
void clear_pixels() {
  for (int i = 0; i < DISPLAY_WIDTH; i++) pixels[i] = 0b00000000;
}
void render_pixel(int x, int y, bool pixel) {
  if (x < 0 || x >= DISPLAY_WIDTH) return;
  if (y < 0 || y >= 8) return;
  if (pixel)
    *(pixels + x) |= 1 << (7 - y); // something like 0b00010000
  else
    *(pixels + x) &= (1 << (7 - y)) ^ 0xFF; // something like 0b11101111
}
void shift_pixels() {
  double t = 1.2 * double(millis() % 2000) / 1000.;
  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    double a = 0.2 * (double(x - DISPLAY_WIDTH / 2) - 50. * t);
    double shiftA = 4. * pow(2., -5. * t) * pow(2., -a*a);

    double b = 0.2 * (double(DISPLAY_WIDTH / 2 - x) - 50. * t);
    double shiftB = 4. * pow(2., -5. * t) * pow(2., -b*b);

    pixels[x] = pixels[x] >> int(max(shiftA, shiftB));
  }
}

float clamp(float v, float low, float high) { return min(high, max(low, v)); }

// Weight measuring.
struct Weight {
  float front_left;
  float front_right;
  float back_left;
  float back_right;

  float total() { return front_left + front_right + back_left + back_right; }
  Weight diff_to(Weight goal) {
    Weight res;
    res.front_left = goal.front_left - front_left;
    res.front_right = goal.front_right - front_right;
    res.back_left = goal.back_left - back_left;
    res.back_right = goal.back_right - back_right;
    return res;
  }
  // 0 = left, 1 = right
  float center_of_mass_x() { return clamp((front_right + back_right) / total(), 0, 1); }
  // 0 = front, 1 = back
  float center_of_mass_y() { return clamp((back_left + back_right) / total(), 0, 1); }
};
typedef struct Weight Weight;

// Main program.

Weight current;
Weight stable;
Weight candidate;
unsigned long candidate_when = 0;

#define MAX_THINGS 128
float things[MAX_THINGS];
int num_things = 0;

void setup() {
  setup_pins();
  setup_serial();
  setup_displays();
  setup_load_cells();

  lc_front_left.wait_ready(1);
  lc_front_right.wait_ready(1);
  lc_back_left.wait_ready(1);
  lc_back_right.wait_ready(1);
  update_current();
  stable = current;
}

void update_current() {
  // unsigned long time_a = millis();

  // Serial.print("Measuring front left...  ");
  if (lc_front_left.is_ready())
    current.front_left = lc_front_left.get_units(1);
  // unsigned long time_b = millis();
  // Serial.println("took " + String(time_b - time_a) + " ms.");

  // Serial.print("Measuring front right... ");
  if (lc_front_right.is_ready())
    current.front_right = lc_front_right.get_units(1);
  // unsigned long time_c = millis();
  // Serial.println("took " + String(time_c - time_b) + " ms.");

  // Serial.print("Measuring back left...   ");
  if (lc_back_left.is_ready())
    current.back_left = lc_back_left.get_units(1);
  // unsigned long time_d = millis();
  // Serial.println("took " + String(time_d - time_c) + " ms.");

  // Serial.print("Measuring back right...  ");
  if (lc_back_right.is_ready())
    current.back_right = lc_back_right.get_units(1);
  // unsigned long time_e = millis();
  // Serial.println("took " + String(time_e - time_d) + " ms.");
}

void loop() {
  unsigned long now = millis();

  Serial.println("Updating " + String(now));
  // clearPixels();
  // renderSmolText(1, 1, "HALLI HALLO DAS IST EIN TEST");
  // pushPixelsToDisplays();

  // return;

  update_current();
  update(now);

  clear_pixels();
  // render_text(15, 0, "0123456789");
  // render_text(15, 0, "Gewicht: 2g");
  // render_text(8, 0, "Rucksack");
  String s = "";
  for (int i = 0; i < num_things; i++) {
    s += (i == 0 ? "" : "+") + String(round(things[i]));
  }
  float fluctuation = stable.diff_to(current).total();
  s += (fluctuation >= 0 ? "+" : "\x2") + String(abs(fluctuation));
  // int offset = int((sin(float(millis()) / 400.0) + 1) * 30);
  render_smol_text(1, 2, s);
  // render_smol_text(1, 0, "RUCKSACK, LAPTOP, IPAD, 900ML WASSER");
  // int len = render_smol_text(1, int(fluctuation / 10.), "KAL?");
  // render_tiny_text(0, 0, "123456789");
  // render_smol_text(1, 2, String(current.total()));

  if (fluctuation > 5) {
    int x = int(stable.diff_to(current).center_of_mass_x() * DISPLAY_WIDTH);
    x = clamp(x, 0, DISPLAY_WIDTH - 1);
    Serial.println("x is " + String(x));
    int y = int(stable.diff_to(current).center_of_mass_y() * DISPLAY_HEIGHT);
    y = clamp(y, 0, DISPLAY_HEIGHT - 1);
    render_pixel(x, 0, true);
  // } else {
    // Serial.println("fluctuation is " + String(fluctuation));
  }
  // shift_pixels();
  show_pixels();
}

void update(unsigned long now) {
  Weight delta = stable.diff_to(current);

  if (abs(candidate.diff_to(current).total()) > 5) {
    // This is a bad candidate, it didn't last for a second.
    candidate = current;
    candidate_when = now;
    return;
  }

  if (now - candidate_when < 1000) {
    // Wait some more time to see if something disproves the candidate.
    return;
  }

  // The candidate was good for one whole second.
  // Let's pick it!
  stable = candidate;
  candidate = current;
  candidate_when = now;

  // We know that the delta was put on (or removed from) the scale.
  if (abs(delta.total()) < 10) {
    // Just fluctuation. Perhaps the load cell slightly shifts over time because of temperature changes.
    return;
  }

  Serial.println(String(delta.total()) + " put on the scale.");

  if (delta.total() > 0) {
    // Something was put on the scale.
    if (num_things > MAX_THINGS) {
      return; // Too bad.
    }
    things[num_things] = delta.total();
    num_things++;
  } else {
    // Something was taken from the scale.
    float removed = -delta.total();
    int thing = -1;
    for (int i = 0; i < num_things; i++)
      if (thing == -1 || abs(things[i] - removed) < abs(things[thing] - removed))
        thing = i;
    if (thing == -1) return; // Nothing was on the scale.

    for (int i = thing + 1; i < num_things; i++)
      things[i - 1] = things[i];
    num_things--;
  }
}
