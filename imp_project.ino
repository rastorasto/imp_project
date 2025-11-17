#include <SPI.h>

// GPIO pins for 74HCT154 decoder
#define ADDR_A 25
#define ADDR_B 17
#define ADDR_C 16
#define ADDR_D 27

// GPIO pins for TLC5947 PWM
#define TLC_DATA  23
#define TLC_CLOCK 18
#define TLC_LATCH 13
#define TLC_BLANK 12
#define TLC_EN    14

// LED Shield buttons
#define BUTTON1   22
#define BUTTON2   21
#define BUTTON3   26
#define BUTTON4   4

// LED matrix dimensions
#define COLS 16
#define ROWS 8

// 24 PWM channels 3 colors for each led
#define CHANNELS 24

// Maximum brightness value (12-bit)
#define FULL_BRIGHTNESS 4095

// Time multiplexing
// 16 columns x 1.25ms = 20ms
// f = 1 / T = 1 / 0.02s = 50Hz
#define WAIT 1250

// Stores RGB values for each pixel in the matrix
// 0=Red, 1=Blue, 2=Green as seen in the schematic
uint16_t frame[COLS][ROWS][3];

// Font with letters 5x7 pixels
const uint8_t font[][5] = {
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7F, 0x41, 0x41, 0x41, 0x3E}, // D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
  {0x00, 0x00, 0x00, 0x00, 0x00}  // SPACE
};

// Convert character to font index
int charIdx(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a';
  if (c == ' ') return 26;
  return 26;
}

// Select active column using 4 to 16 decoder after that P channel MOSFET supplies power to the column
void selectColumn(int c) {
  digitalWrite(ADDR_A, (c >> 0) & 1);  // A0 (LSB)
  digitalWrite(ADDR_B, (c >> 1) & 1);  // A1
  digitalWrite(ADDR_C, (c >> 2) & 1);  // A2
  digitalWrite(ADDR_D, (c >> 3) & 1);  // A3 (MSB)
}

// Sends PWM values to TLC5947
void sendTLC(uint16_t *v) {
  // 24 channels x 12 bits each = 288 bits
  // 288 bits / 8 = 36 bytes
  uint8_t buf[36];
  memset(buf, 0, sizeof(buf));
  
  int totalBitPosition = 0;
  // Iterate over channels in reverse order 
  for (int channel = CHANNELS - 1; channel >= 0; channel--) {
    // Uint16_t is 16bits therefore mask the lower 12 bits the 4 bits are not needed 
    uint16_t pwmValue = v[channel] & 0x0FFF;
    
    // Go through each bit of the 12-bit pwmValue from MSB to LSB
    for (int bitNumber = 11; bitNumber >= 0; bitNumber--) {
      // Calculate byte and bit position
      int byteIndex = totalBitPosition / 8;
      // 7 - bit posision because MSB is sent first
      int bitOffset = 7 - (totalBitPosition % 8);
      
      // Extract the bit value by shifting and masking
      uint8_t bitValue = (pwmValue >> bitNumber) & 1;
      // Set the bit in the buffer
      buf[byteIndex] |= (bitValue << bitOffset);
      
      // Advance to next bit position
      totalBitPosition++;
    }
  }

  // Sets latch pin low before data transfer
  digitalWrite(TLC_LATCH, LOW);
  // SPI configuration 8Mhz clock speed
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));  // 8 MHz < 30 MHz max
  // Send data via SPI
  SPI.writeBytes(buf, sizeof(buf));
  // End SPI transaction
  SPI.endTransaction();
  // Latch the data into the PWM outputs
  digitalWrite(TLC_LATCH, HIGH);
  // Wait to ensure that the data is latched
  delayMicroseconds(1);
  // Set latch low again
  digitalWrite(TLC_LATCH, LOW);
}

// Time multiplexing state
int scanCol = 0;              // Currently active column (0-15)
uint32_t lastColumnTime = 0;  // Timing anchor for fixed dwell period

// Refresh display one column at a time
// After 1.25ms switches to the next column
// that means 16*1.25ms = 20ms the display has whole new "image" every 50Hz
void refreshDisplay() {
  uint32_t now = micros();
  // Check if it's time to switch to the next column
  if ((uint32_t)(now - lastColumnTime) >= WAIT) {
    uint16_t data[CHANNELS];

    // For each row in the current column
    // Populate data array with color values mapped to TLC channels
    for (int r = 0; r < ROWS; r++) {
      // The wiring is Red, Blue, Green
      data[r * 3 + 0] = frame[scanCol][r][1]; // Channel offset 0 Blue
      data[r * 3 + 1] = frame[scanCol][r][0]; // Channel offset 1 Red
      data[r * 3 + 2] = frame[scanCol][r][2]; // Channel offset 2 Green
    }

    // Blank display during the switch
    digitalWrite(TLC_BLANK, HIGH);
    // Select the current column
    selectColumn(scanCol);
    // Send data for this column
    sendTLC(data);
    // Enable outputs
    digitalWrite(TLC_BLANK, LOW);

    // Save time of this column update and move to the next column
    lastColumnTime += WAIT;
    scanCol++;
    // Wrap around after column 15
    if (scanCol >= COLS) scanCol = 0;
  }
}

// Fucntion to clear the frame
void clearFrame() {
  memset(frame, 0, sizeof(frame));
}

// Function to set pixel color in frame
void setPixel(int x, int y, uint16_t red, uint16_t green, uint16_t blue) {
  // Bounds check (should not happen but just to be sure)
  if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
  // Colors are in order Red, Blue, Green
  frame[x][y][0] = red;
  frame[x][y][1] = blue;
  frame[x][y][2] = green;
}

// Draws a single 5x7 character at position x with specified color
void drawCharacter(char c, int x, uint16_t red, uint16_t green, uint16_t blue) {
  // Gets index of the number in font array
  int id = charIdx(c);
  // Draw each of the 5 columns
  for (int cx = 0; cx < 5; cx++) {
    // Gets the bits for the current column
    uint8_t colBits = font[id][cx];
    // Go over each bit (row) in the column
    for (int y = 0; y < 8; y++) {
      // If the bit is 1 draw the pixel
      if (colBits & (1 << y)) {
        // Invert y to match coordinate system
        setPixel(x + cx, 7 - y, red, green, blue);
      }
    }
  }
}

// Function to draw text starting at position startX with specified color
void drawText(String txt, int startX, uint16_t red, uint16_t green, uint16_t blue) {
  // Saves the start position because it will be changed
  int x = startX;
  // Go thrue every character in the string
  for (int i = 0; i < txt.length(); i++) {
    // Draw the character
    drawCharacter(txt[i], x, red, green, blue);
    // Increment by character width + 1 pixel for space between characters
    x += 6;
  }
}

// Text messages
String messages[] = {
  "PROJECT DEMO",
  "IMP",
  "FIT",
  "HELLO",
  "MEOW"
};

// Color structure for RGB values
struct Color { uint16_t red, green, blue; };
// Pallete for adjusting colors
const Color PALETTE[] = {
  {4095,    0,    0}, // Red
  {   0, 4095,    0}, // Green
  {   0,    0, 4095}, // Blue
  {4095,    0, 4095}, // Purple
  {   0, 4095, 4095}, // Cyan
  {4095, 4095,    0}, // Yellow
  {4095, 4095, 4095}  // White
};

// Adjustable brightness (from dim to bright)
const uint16_t BRIGHTNESS_LEVELS[] = { 1024, 2048, 3072, 4095 };

// Adjustable speed (from slow to fast)
const uint16_t SPEED_INTERVALS_MS[] = { 200, 150, 100, 75, 50 };

// Currently displayed message
int currentMsg = 0;
// Scroll position
int scrollPos = 0;
// Last scroll update timestamp
uint32_t lastScrollMs = 0;

// Current settings
int colorIdx = 0;
int brightIdx = 0;
int speedIdx = 0;
uint16_t scrollIntervalMs = SPEED_INTERVALS_MS[speedIdx];

// Last button press timestamp
uint32_t lastBtnMs = 0;

// To avoid button mechanical bounce there should be 200ms between presses
bool isButtonPressed(int btnPin) {
  uint32_t now = millis();
  // If button is pressed and debounce time has passed
  if (digitalRead(btnPin) == LOW && (now - lastBtnMs >= 200)) {
    // Update last button press time and return true
    lastBtnMs = now;
    return true;
  }
  return false;
}

// Update scrolling text (move one pixel to the left)
void updateScrollingText() {
  uint32_t nowMs = millis();
  // Check if it's time to scroll
  if (nowMs - lastScrollMs >= scrollIntervalMs) {
    lastScrollMs = nowMs;
    // Move scroll position one pixel to the left
    scrollPos++;
    
    // Get current message
    String msg = messages[currentMsg];

    // Reset scroll after message left the screen
    if (scrollPos > msg.length() * 6 + COLS) {
      scrollPos = 0;
    }
    
    // Clear previous frame
    clearFrame();

    // Get color and brightness
    Color c = PALETTE[colorIdx];
    uint16_t br = BRIGHTNESS_LEVELS[brightIdx];

    // Calculate color based on palette and brightness
    uint16_t red = (uint32_t)c.red * br / FULL_BRIGHTNESS;
    uint16_t green = (uint32_t)c.green * br / FULL_BRIGHTNESS;
    uint16_t blue = (uint32_t)c.blue * br / FULL_BRIGHTNESS;
    
    // Redraw the frame
    drawText(msg, COLS - scrollPos, red, green, blue);
  }
}

// Setup function
void setup() {
  Serial.begin(115200);

  pinMode(ADDR_A, OUTPUT);  
  pinMode(ADDR_B, OUTPUT);
  pinMode(ADDR_C, OUTPUT);
  pinMode(ADDR_D, OUTPUT);

  pinMode(TLC_DATA, OUTPUT);
  pinMode(TLC_CLOCK, OUTPUT);
  pinMode(TLC_LATCH, OUTPUT);
  pinMode(TLC_BLANK, OUTPUT);
  pinMode(TLC_EN, OUTPUT);

  SPI.begin();

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  digitalWrite(TLC_EN, LOW);
  digitalWrite(TLC_BLANK, HIGH);

  lastColumnTime = micros();
  clearFrame();
}

void loop() {
  // Button press handling
  if (isButtonPressed(BUTTON1)) {
    // Cycle through messages
    currentMsg = (currentMsg + 1) % 5;
    scrollPos = 0;
  }
  if (isButtonPressed(BUTTON2)) {
    // Change scrolling speed
    speedIdx = (speedIdx + 1) % 5;
    scrollIntervalMs = SPEED_INTERVALS_MS[speedIdx];
  }
  if (isButtonPressed(BUTTON3)) {
    // Change text color
    colorIdx = (colorIdx + 1) % 7;
  }
  if (isButtonPressed(BUTTON4)) {
    // Change display brightness
    brightIdx = (brightIdx + 1) % 4;
  }

  // Updates scrolling text position
  updateScrollingText();

  // Resreshes the display
  refreshDisplay();
}