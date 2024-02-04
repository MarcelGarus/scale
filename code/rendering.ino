void render_box(int x, int y) {
  render_pixel(x, y, true);
  render_pixel(x, y + 1, true);
  render_pixel(x + 1, y, true);
  render_pixel(x + 1, y + 1, true);
}
void render_column(int x, int y, uint8_t column) {
  for (int i = 0; i < 8; i++)
    render_pixel(x, y + i, column >> i & 1);
}
int render_char(int x, int y, char c) {
  #define RENDER_COL(n, col) render_column(x + n, y, col);
  #define CHAR_PIXELS_1(ch, col0) case ch: RENDER_COL(0, col0); return 1;
  #define CHAR_PIXELS_2(ch, col0, col1) case ch: RENDER_COL(0, col0); RENDER_COL(1, col1); return 2;
  #define CHAR_PIXELS_3(ch, col0, col1, col2) case ch: RENDER_COL(0, col0); RENDER_COL(1, col1); RENDER_COL(2, col2); return 3;
  #define CHAR_PIXELS_4(ch, col0, col1, col2, col3) case ch: RENDER_COL(0, col0); RENDER_COL(1, col1); RENDER_COL(2, col2); RENDER_COL(3, col3); return 4;
  #define CHAR_PIXELS_5(ch, col0, col1, col2, col3, col4) case ch: RENDER_COL(0, col0); RENDER_COL(1, col1); RENDER_COL(2, col2); RENDER_COL(3, col3); RENDER_COL(4, col4); return 5;

  switch (c) {
    CHAR_PIXELS_3('A', 0b01111111,
                       0b00001001,
                       0b01111111)
    CHAR_PIXELS_3('G', 0b00111111,
                       0b00100001,
                       0b00111001)
    CHAR_PIXELS_3('L', 0b01111111,
                       0b01000000,
                       0b01000000)
    CHAR_PIXELS_4('P', 0b01111111,
                       0b00001001,
                       0b00001001,
                       0b00000110)
    CHAR_PIXELS_4('R', 0b01111111,
                       0b00011001,
                       0b00101001,
                       0b01000110)
    CHAR_PIXELS_4('a', 0b00111000,
                       0b01000100,
                       0b01000100,
                       0b01111100)
    CHAR_PIXELS_4('b', 0b01111111,
                       0b01000100,
                       0b01000100,
                       0b00111000)
    CHAR_PIXELS_3('c', 0b00111000,
                       0b01000100,
                       0b01000100)
    CHAR_PIXELS_4('d', 0b00111000,
                       0b01000100,
                       0b01000100,
                       0b01111111)
    CHAR_PIXELS_3('e', 0b01111100,
                       0b01010100,
                       0b01011100)
    CHAR_PIXELS_3('g', 0b10111100,
                       0b10100100,
                       0b11111100)
    CHAR_PIXELS_3('h', 0b01111111,
                       0b00000100,
                       0b01111100)
    CHAR_PIXELS_1('i', 0b01111101)
    CHAR_PIXELS_3('k', 0b01111111,
                       0b00010000,
                       0b01101100)
    CHAR_PIXELS_2('l', 0b01111111,
                       0b01000000)
    CHAR_PIXELS_5('m', 0b01111100,
                       0b00000100,
                       0b01111000,
                       0b00000100,
                       0b01111000)
    CHAR_PIXELS_4('o', 0b00111000,
                       0b01000100,
                       0b01000100,
                       0b00111000)
    CHAR_PIXELS_4('p', 0b11111100,
                       0b00100100,
                       0b00100100,
                       0b00011000)
    CHAR_PIXELS_3('s', 0b01011100,
                       0b01010100,
                       0b01110100)
    CHAR_PIXELS_3('t', 0b00000100,
                       0b00111111,
                       0b01000100)
    CHAR_PIXELS_4('u', 0b00111100,
                       0b01000000,
                       0b01000000,
                       0b00111100)
    CHAR_PIXELS_5('w', 0b00011100,
                       0b01100000,
                       0b00011100,
                       0b01100000,
                       0b00011100)
    CHAR_PIXELS_1(':', 0b01000100)
    CHAR_PIXELS_1('.', 0b01000000)
    CHAR_PIXELS_1(',', 0b11000000)
    CHAR_PIXELS_5('+', 0b00001000,
                       0b00001000,
                       0b00111110,
                       0b00001000,
                       0b00001000)
    CHAR_PIXELS_2('-', 0b00001000,
                       0b00001000)
    CHAR_PIXELS_1(' ', 0b00000000)
    CHAR_PIXELS_4('0', 0b00111110,
                       0b01000001,
                       0b01000001,
                       0b00111110)
    CHAR_PIXELS_4('1', 0b00000100,
                       0b01000010,
                       0b01111111,
                       0b01000000)
    CHAR_PIXELS_4('2', 0b01110010,
                       0b01001001,
                       0b01001001,
                       0b01000110)
    CHAR_PIXELS_4('3', 0b01000001,
                       0b01001001,
                       0b01001001,
                       0b00110110)
    CHAR_PIXELS_4('4', 0b00011000,
                       0b00010100,
                       0b00010010,
                       0b01111111)
    CHAR_PIXELS_4('5', 0b00101111,
                       0b01001001,
                       0b01001001,
                       0b00110001)
    CHAR_PIXELS_4('6', 0b00111110,
                       0b01001001,
                       0b01001001,
                       0b00110001)
    CHAR_PIXELS_4('7', 0b00000001,
                       0b01110001,
                       0b00001101,
                       0b00000011)
    CHAR_PIXELS_4('8', 0b00110110,
                       0b01001001,
                       0b01001001,
                       0b00110110)
    CHAR_PIXELS_4('9', 0b01000110,
                       0b01001001,
                       0b01001001,
                       0b00111110)

    // We want some special characters that are not part of ASCII.
    // Here, we (mis)use some of the ASCII code points for our needs.

    // thin space
    case 0x01:
      return 0;
    // fat minus (same width as plus)
    CHAR_PIXELS_5(0x02,
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001000)
    // charger icon
    CHAR_PIXELS_5(0x03,
      0b10011000,
      0b10111110,
      0b01111000,
      0b00111110,
      0b00011000)


    default:
      RENDER_COL(0, 0b1111111);
      RENDER_COL(1, 0b1111111);
      RENDER_COL(2, 0b1111111);
      RENDER_COL(3, 0b1111111);
      return 4;
  }
}
int render_text(int x, int y, String text) {
  for (int i = 0; i < text.length(); i++) {
    if (i > 0) x += 1;
    x += render_char(x, y, text[i]);
  }
  return x;
}

void render_smol_column(int x, int y, uint8_t column) {
  for (int i = 0; i < 5; i++)
    render_pixel(x, y + i, column >> i & 1);
}
int render_smol_char(int x, int y, char c) {
  #define RENDER_SMOL_COL(n, col) render_smol_column(x + n, y, col);
  #define SMOL_CHAR_PIXELS_1(ch, col0) case ch: RENDER_SMOL_COL(0, col0); return 1;
  #define SMOL_CHAR_PIXELS_2(ch, col0, col1) case ch: RENDER_SMOL_COL(0, col0); RENDER_SMOL_COL(1, col1); return 2;
  #define SMOL_CHAR_PIXELS_3(ch, col0, col1, col2) case ch: RENDER_SMOL_COL(0, col0); RENDER_SMOL_COL(1, col1); RENDER_SMOL_COL(2, col2); return 3;
  #define SMOL_CHAR_PIXELS_4(ch, col0, col1, col2, col3) case ch: RENDER_SMOL_COL(0, col0); RENDER_SMOL_COL(1, col1); RENDER_SMOL_COL(2, col2); RENDER_SMOL_COL(3, col3); return 4;
  #define SMOL_CHAR_PIXELS_5(ch, col0, col1, col2, col3, col4) case ch: RENDER_SMOL_COL(0, col0); RENDER_SMOL_COL(1, col1); RENDER_SMOL_COL(2, col2); RENDER_SMOL_COL(3, col3); RENDER_SMOL_COL(4, col4); return 5;

  switch (c) {
    SMOL_CHAR_PIXELS_3('A', 0b11111,
                            0b00101,
                            0b11111)
    SMOL_CHAR_PIXELS_3('B', 0b11111,
                            0b10101,
                            0b01110)
    SMOL_CHAR_PIXELS_3('C', 0b11111,
                            0b10001,
                            0b10001)
    SMOL_CHAR_PIXELS_3('D', 0b11111,
                            0b10001,
                            0b01110)
    SMOL_CHAR_PIXELS_3('E', 0b11111,
                            0b10101,
                            0b10101)
    SMOL_CHAR_PIXELS_2('F', 0b11111,
                            0b00101)
    SMOL_CHAR_PIXELS_3('G', 0b11111,
                            0b10001,
                            0b11101)
    SMOL_CHAR_PIXELS_3('H', 0b11111,
                            0b00100,
                            0b11111)
    SMOL_CHAR_PIXELS_1('I', 0b11111)
    SMOL_CHAR_PIXELS_3('J', 0b01000,
                            0b10000,
                            0b01111)
    SMOL_CHAR_PIXELS_3('K', 0b11111,
                            0b00100,
                            0b11011)
    SMOL_CHAR_PIXELS_2('L', 0b11111,
                            0b10000)
    SMOL_CHAR_PIXELS_5('M', 0b11111,
                            0b00010,
                            0b00100,
                            0b00010,
                            0b11111)
    SMOL_CHAR_PIXELS_4('N', 0b11111,
                            0b00010,
                            0b00100,
                            0b11111)
    SMOL_CHAR_PIXELS_3('O', 0b11111,
                            0b10001,
                            0b11111)
    SMOL_CHAR_PIXELS_3('P', 0b11111,
                            0b00101,
                            0b00111)
    SMOL_CHAR_PIXELS_4('Q', 0b11111,
                            0b10001,
                            0b11111,
                            0b10000)
    SMOL_CHAR_PIXELS_3('R', 0b11111,
                            0b01101,
                            0b10111)
    SMOL_CHAR_PIXELS_3('S', 0b10111,
                            0b10101,
                            0b11101)
    SMOL_CHAR_PIXELS_3('T', 0b00001,
                            0b11111,
                            0b00001)
    SMOL_CHAR_PIXELS_3('U', 0b11111,
                            0b10000,
                            0b11111)
    SMOL_CHAR_PIXELS_3('V', 0b00111,
                            0b11000,
                            0b00111)
    SMOL_CHAR_PIXELS_5('W', 0b00111,
                            0b11000,
                            0b00111,
                            0b11000,
                            0b00111)
    SMOL_CHAR_PIXELS_3('X', 0b11011,
                            0b00100,
                            0b11011)
    SMOL_CHAR_PIXELS_3('Y', 0b00011,
                            0b11100,
                            0b00011)
    SMOL_CHAR_PIXELS_3('Z', 0b11001,
                            0b10101,
                            0b10011)
    SMOL_CHAR_PIXELS_3('ö', 0b11101,
                            0b10100,
                            0b11101)
    SMOL_CHAR_PIXELS_3('Ü', 0b11101,
                            0b10000,
                            0b11101)
    SMOL_CHAR_PIXELS_1(':', 0b01010)
    SMOL_CHAR_PIXELS_1('.', 0b10000)
    SMOL_CHAR_PIXELS_3('+', 0b00100,
                            0b01110,
                            0b00100)
    SMOL_CHAR_PIXELS_2('-', 0b00100,
                            0b00100)
    SMOL_CHAR_PIXELS_3('?', 0b00001,
                            0b10101,
                            0b00011)
    SMOL_CHAR_PIXELS_1(' ', 0b00000)
    SMOL_CHAR_PIXELS_3('0', 0b11111,
                            0b10001,
                            0b11111)
    SMOL_CHAR_PIXELS_3('1', 0b00000,
                            0b00000,
                            0b11111)
    SMOL_CHAR_PIXELS_3('2', 0b11101,
                            0b10101,
                            0b10111)
    SMOL_CHAR_PIXELS_3('3', 0b10101,
                            0b10101,
                            0b11111)
    SMOL_CHAR_PIXELS_3('4', 0b00111,
                            0b00100,
                            0b11111)
    SMOL_CHAR_PIXELS_3('5', 0b10111,
                            0b10101,
                            0b11101)
    SMOL_CHAR_PIXELS_3('6', 0b11111,
                            0b10101,
                            0b11101)
    SMOL_CHAR_PIXELS_3('7', 0b00001,
                            0b11001,
                            0b00111)
    SMOL_CHAR_PIXELS_3('8', 0b11111,
                            0b10101,
                            0b11111)
    SMOL_CHAR_PIXELS_3('9', 0b10111,
                            0b10101,
                            0b11111)
    // fat minus (same width as plus)
    SMOL_CHAR_PIXELS_3(0x02, 0b00100,
                             0b00100,
                             0b00100)

    default:
      RENDER_SMOL_COL(0, 0b11111);
      RENDER_SMOL_COL(1, 0b11111);
      return 2;
  }
}
int render_smol_text(int x, int y, String text) {
  for (int i = 0; i < text.length(); i++) {
    if (i > 0) x += 1;
    x += render_smol_char(x, y, text[i]);
  }
  return x;
}

void render_tiny_column(int x, int y, uint8_t column) {
  for (int i = 0; i < 3; i++)
    render_pixel(x, y + i, column >> i & 1);
}

int render_tiny_char(int x, int y, char c) {
  #define RENDER_TINY_COL(n, col) render_tiny_column(x + n, y, col);
  #define TINY_CHAR_PIXELS_1(ch, col0) case ch: RENDER_TINY_COL(0, col0); return 1;
  #define TINY_CHAR_PIXELS_3(ch, col0, col1, col2) case ch: RENDER_TINY_COL(0, col0); RENDER_TINY_COL(1, col1); RENDER_TINY_COL(2, col2); return 3;

  switch (c) {
    TINY_CHAR_PIXELS_1('.', 0b010)
    TINY_CHAR_PIXELS_3('0', 0b000, 0b000, 0b000)
    TINY_CHAR_PIXELS_3('1', 0b100, 0b000, 0b000)
    TINY_CHAR_PIXELS_3('2', 0b100, 0b100, 0b000)
    TINY_CHAR_PIXELS_3('3', 0b100, 0b100, 0b100)
    TINY_CHAR_PIXELS_3('4', 0b110, 0b100, 0b100)
    TINY_CHAR_PIXELS_3('5', 0b110, 0b110, 0b100)
    TINY_CHAR_PIXELS_3('6', 0b110, 0b110, 0b110)
    TINY_CHAR_PIXELS_3('7', 0b111, 0b110, 0b110)
    TINY_CHAR_PIXELS_3('8', 0b111, 0b111, 0b110)
    TINY_CHAR_PIXELS_3('9', 0b111, 0b111, 0b111)

    default:
      RENDER_TINY_COL(0, 0b11111);
      RENDER_TINY_COL(1, 0b11111);
      return 2;
  }
}
int render_tiny_text(int x, int y, String text) {
  for (int i = 0; i < text.length(); i++) {
    if (i > 0) x += 1;
    x += render_tiny_char(x, y, text[i]);
  }
  return x;
}
