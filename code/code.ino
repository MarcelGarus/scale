// TODO: Refactor to use snake case

#include <MD_MAX72xx.h>
#include <HX711.h>
#include <SPI.h>

// Displays: MD_MAX72XX
#define DISPLAY_LEFT_CLK_PIN   A2
#define DISPLAY_LEFT_DATA_PIN  A0
#define DISPLAY_LEFT_CS_PIN    A1
#define DISPLAY_RIGHT_CLK_PIN  A5
#define DISPLAY_RIGHT_DATA_PIN A3
#define DISPLAY_RIGHT_CS_PIN   A4

// Load cells: HX711
#define LC_FRONT_LEFT_DATA_PIN  13
#define LC_FRONT_LEFT_SCK_PIN   12
#define LC_FRONT_RIGHT_DATA_PIN 3
#define LC_FRONT_RIGHT_SCK_PIN  4
#define LC_BACK_LEFT_DATA_PIN   10
#define LC_BACK_LEFT_SCK_PIN    9
#define LC_BACK_RIGHT_DATA_PIN  6
#define LC_BACK_RIGHT_SCK_PIN   7

void setup_pins() {
  // Because of pin shortage, we use some analog pins as digital pins.
  // https://forum.arduino.cc/t/arduino-uno-using-analog-input-as-digital-in-out/143837
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
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
  Serial.println("Setting up displays.");

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
  Serial.println("Setting up load cells.");
  
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

// Sinus curve from 0 to 1 (useful for animation).
#define PI 3.14159265358979323846
float ease_out(float t) {
  if (t <= 0.0) return 0.0;
  if (t >= 1.0) return 1.0;
  return sin(t * PI / 2);
}
float ease_in_out(float t) {
  if (t <= 0.0) return 0.0;
  if (t >= 1.0) return 1.0;
  return (1.0 - cos(t * PI)) / 2.0;
}

// Main program.

unsigned long now;
Weight current;
Weight wood_plate;

enum Mode {
  mode_standby,
  mode_measure,
  mode_backpack,
  mode_tea,
  mode_cooking,
  mode_calibrate
};

void setup() {
  setup_pins();
  setup_serial();
  setup_displays();
  setup_load_cells();

  Serial.print("Front left...  ");
  lc_front_left.wait_ready(1);
  Serial.println("done");

  Serial.print("Front right... ");
  lc_front_right.wait_ready(1);
  Serial.println("done");

  Serial.print("Back left...   ");
  lc_back_left.wait_ready(1);
  Serial.println("done");

  Serial.print("Back right...  ");
  // lc_back_right.wait_ready(1); // TODO
  Serial.println("skipped");
  
  update_current();
  delay(500);
  update_current();
  wood_plate = current;
  Serial.println("Done with setup.");

  init_state();
}
void update_current() {
  if (lc_front_left.is_ready())  current.front_left  = lc_front_left.get_units(1);
  if (lc_front_right.is_ready()) current.front_right = lc_front_right.get_units(1);
  if (lc_back_left.is_ready())   current.back_left   = lc_back_left.get_units(1);
  if (lc_back_right.is_ready())  current.back_right  = lc_back_right.get_units(1);
}

// Detects taps on the right side. A tap is a rapid adding and removing of
// weight.
struct Button {
  Weight previous; // TODO: allow increase over multiple ticks
  unsigned long tap_down = 0; // 0 = not occurred, otherwise timestamp
  Weight tapped_down;
  bool tapped = false;

  Button() {
    previous = current;
  }

  void tick() {
    Weight prev = previous;
    previous = current;

    if (tapped) return;

    if (tap_down > 0) {
      if (now - tap_down > 1000) {
        // not released for a second, abort tap
        Serial.println("tap not released for a second, abort");
        tap_down = 0;
        return;
      }
      if (current.front_right < tapped_down.front_right - 10) {
        // tap up occurred
        tapped = true;
        return;
      }
      // tapped down, but not up yet
      Serial.println("tap is still holding");
      return;
    }

    if (current.front_right > prev.front_right + 10
      && abs(current.total() - prev.total()) < 15
    ) {
      Serial.println("tap started");
      tap_down = now;
      tapped_down = current;
    }
  }
};

// Standby mode.
struct Standby {
  // DoubleTapDetector calibrate_gesture;

  void tick() {
    if (abs(wood_plate.diff_to(current).total()) > 15) return switch_to(mode_measure);

    clear_pixels();
    show_pixels();
  }
};

// Measure mode.
struct Measure {
  Button tare_button;

  void tick() {
    float weight_on_wood_plate = wood_plate.diff_to(current).total();
    if (abs(weight_on_wood_plate) < 10) return switch_to(mode_standby);
    if (abs(weight_on_wood_plate) > 1220) return switch_to(mode_tea); // TODO: only when in measure mode for short amount of time

    tare_button.tick();

    clear_pixels();
    render_smol_text(0, 2, String(round(weight_on_wood_plate)));
    render_smol_text(113, 2, "TARE");
    if (tare_button.tapped) render_smol_text(80, 2, "Hi");
    show_pixels();
  }
};

// Tea mode.
struct Tea {
  unsigned long brew_start = 0; // 0 = hasn't started, otherwise timestamp

  void tick() {
    Weight weight_on_wood_plate = wood_plate.diff_to(current);

    if (weight_on_wood_plate.total() < 20) return switch_to(mode_measure);

    float brew_where = weight_on_wood_plate.center_of_mass_x();
    if (brew_start == 0 && weight_on_wood_plate.total() > 1500) {
      brew_start = now;
    }

    clear_pixels();
    render_smol_text(0, 2, "TEE");

    if (brew_start > 0) {
      unsigned long diff_in_seconds = (now - brew_start) / 1000;
      unsigned long display_minutes = diff_in_seconds / 60;
      unsigned long display_seconds = diff_in_seconds % 60;
      render_smol_text(
        59, // clamp(DISPLAY_WIDTH * brew_where, 0, DISPLAY_WIDTH - 1),
        round((1.0 - ease_out(float(now - brew_start) / 400.0)) * (-7.0) + 2.0),
        String(display_minutes) + ":" + (display_seconds < 10 ? "0" : "") + String(display_seconds)
      );
    }

    show_pixels();
  }
};

union State {
  Standby standby;
  Measure measure;
  Tea tea;
};
Mode mode = mode_standby;
State state = {};

void init_state() { switch_to(mode_standby); }
void switch_to(int m) {
  Serial.println("Switching to mode " + String(m));
  mode = m;
  if (mode == mode_standby) state.standby = Standby();
  if (mode == mode_measure) state.measure = Measure();
  if (mode == mode_tea)     state.tea = Tea();
}

void loop() {
  now = millis();
  // Serial.println("Tick " + String(now));

  update_current();

  if (mode == mode_standby) state.standby.tick();
  if (mode == mode_measure) state.measure.tick();
  if (mode == mode_tea)     state.tea.tick();
  return;
}

struct DoubleTapDetector {
  unsigned long first_tap_down = 0; // 0 = not occurred, otherwise timestamp
  unsigned long first_tap_up   = 0; // 0 = not occurred, otherwise timestamp
  unsigned long second_tap     = 0; // 0 = not occurred, otherwise timestamp
};
void tick_double_tap_detector(struct DoubleTapDetector* state) {}
