void renderBox(int x, int y) {
  renderPixel(x, y, true);
  renderPixel(x, y + 1, true);
  renderPixel(x + 1, y, true);
  renderPixel(x + 1, y + 1, true);
}
void renderColumn(int x, int y, uint8_t column) {
  for (int i = 0; i < 8; i++)
    renderPixel(x, y + i, column >> i & 1);
}
int renderChar(int x, int y, char c) {
  #define RENDER_COL(n, col) renderColumn(x + n, y, col);
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
    CHAR_PIXELS_4('R', 0b01111111,
                       0b00011001,
                       0b00101001,
                       0b01000110)
    CHAR_PIXELS_4('a', 0b00111000,
                       0b01000100,
                       0b01000100,
                       0b01111100)
    CHAR_PIXELS_3('c', 0b00111000,
                       0b01000100,
                       0b01000100)
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
    CHAR_PIXELS_3('o', 0b01111100,
                       0b01000100,
                       0b01111100)
    CHAR_PIXELS_3('s', 0b01011100,
                       0b01010100,
                       0b01110100)
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
    CHAR_PIXELS_2(',', 0b10000000,
                       0b01000000)
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

    default:
      RENDER_COL(0, 0b1111111);
      RENDER_COL(1, 0b1111111);
      RENDER_COL(2, 0b1111111);
      RENDER_COL(3, 0b1111111);
      return 4;
  }
}
int renderText(int x, int y, String text) {
  for (int i = 0; i < text.length(); i++) {
    if (i > 0) x += 1;
    x += renderChar(x, y, text[i]);
  }
  return x;
}
