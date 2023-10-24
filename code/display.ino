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
  switch (c) {
    case 'A':
      renderColumn(x + 0, y, 0b1111111);
      renderColumn(x + 1, y, 0b0001001);
      renderColumn(x + 2, y, 0b1111111);
      return 3;
    case 'G':
      renderColumn(x + 0, y, 0b111111);
      renderColumn(x + 1, y, 0b100001);
      renderColumn(x + 2, y, 0b111001);
      return 3;
    case 'c':
      renderColumn(x + 0, y, 0b1111100);
      renderColumn(x + 1, y, 0b1000100);
      renderColumn(x + 2, y, 0b1000100);
      return 3;
    case 'e':
      renderColumn(x + 0, y, 0b1111100);
      renderColumn(x + 1, y, 0b1010100);
      renderColumn(x + 2, y, 0b1011100);
      return 3;
    case 'g':
      renderColumn(x + 0, y, 0b10111100);
      renderColumn(x + 1, y, 0b10100100);
      renderColumn(x + 2, y, 0b11111100);
      return 3;
    case 'h':
      renderColumn(x + 0, y, 0b1111111);
      renderColumn(x + 1, y, 0b0000100);
      renderColumn(x + 2, y, 0b1111100);
      return 3;
    case 'i':
      renderColumn(x + 0, y, 0b1111101);
      return 1;
    case 'l':
      renderColumn(x + 0, y, 0b1111111);
      renderColumn(x + 1, y, 0b1000000);
      return 2;
    case 'o':
      renderColumn(x + 0, y, 0b1111100);
      renderColumn(x + 1, y, 0b1000100);
      renderColumn(x + 2, y, 0b1111100);
      return 3;
    case 'w':
      renderColumn(x + 0, y, 0b0011100);
      renderColumn(x + 1, y, 0b1100000);
      renderColumn(x + 2, y, 0b0011100);
      renderColumn(x + 3, y, 0b1100000);
      renderColumn(x + 4, y, 0b0011100);
      return 5;
    case ':':
      renderColumn(x + 0, y, 0b1000100);
      return 1;
    case '.':
      renderColumn(x + 0, y, 0b1000000);
      return 1;
    case '-':
      renderColumn(x + 0, y, 0b0001000);
      renderColumn(x + 1, y, 0b0001000);
      return 2;
    case ' ':
      renderColumn(x + 0, y, 0b0000000);
      return 1;
    case '0':
      renderColumn(x + 0, y, 0b0111110);
      renderColumn(x + 1, y, 0b1000001);
      renderColumn(x + 2, y, 0b1000001);
      renderColumn(x + 3, y, 0b0111110);
      return 4;
    case '1':
      renderColumn(x + 0, y, 0b0000100);
      renderColumn(x + 1, y, 0b1000010);
      renderColumn(x + 2, y, 0b1111111);
      renderColumn(x + 3, y, 0b1000000);
      return 4;
    case '2':
      renderColumn(x + 0, y, 0b1100010);
      renderColumn(x + 1, y, 0b1010001);
      renderColumn(x + 2, y, 0b1010001);
      renderColumn(x + 3, y, 0b1001110);
      return 4;
    case '3':
      renderColumn(x + 0, y, 0b1000001);
      renderColumn(x + 1, y, 0b1001001);
      renderColumn(x + 2, y, 0b1001001);
      renderColumn(x + 3, y, 0b0110110);
      return 4;
    case '4':
      renderColumn(x + 0, y, 0b0011000);
      renderColumn(x + 1, y, 0b0010100);
      renderColumn(x + 2, y, 0b0010010);
      renderColumn(x + 3, y, 0b1111111);
      return 4;
    case '5':
      renderColumn(x + 0, y, 0b0101111);
      renderColumn(x + 1, y, 0b1001001);
      renderColumn(x + 2, y, 0b1001001);
      renderColumn(x + 3, y, 0b0110001);
      return 4;
    case '6':
      renderColumn(x + 0, y, 0b0111110);
      renderColumn(x + 1, y, 0b1001001);
      renderColumn(x + 2, y, 0b1001001);
      renderColumn(x + 3, y, 0b0110001);
      return 4;
    case '7':
      renderColumn(x + 0, y, 0b0000001);
      renderColumn(x + 1, y, 0b1110001);
      renderColumn(x + 2, y, 0b0001101);
      renderColumn(x + 3, y, 0b0000011);
      return 4;
    case '8':
      renderColumn(x + 0, y, 0b0110110);
      renderColumn(x + 1, y, 0b1001001);
      renderColumn(x + 2, y, 0b1001001);
      renderColumn(x + 3, y, 0b0110110);
      return 4;
    case '9':
      renderColumn(x + 0, y, 0b1000110);
      renderColumn(x + 1, y, 0b1001001);
      renderColumn(x + 2, y, 0b1001001);
      renderColumn(x + 3, y, 0b0111110);
      return 4;
    default:
      renderColumn(x + 0, y, 0b1111111);
      renderColumn(x + 1, y, 0b1111111);
      renderColumn(x + 2, y, 0b1111111);
      return 3;
  }
}
int renderText(int x, int y, String text) {
  for (int i = 0; i < text.length(); i++) {
    if (i > 0) x += 1;
    x += renderChar(x, y, text[i]);
  }
  return x;
}
