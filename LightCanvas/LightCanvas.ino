/* LightCanvas
 *
 * Stand alone Arduino program for an interactive light display with several modes.
 *
 */

#include "SPI.h"
#include "Adafruit_WS2801.h"

#define ROWS 8
#define COLS 20
#define BUTTONS 6
#define PROXIMITY_SENSORS 40
#define CONTROL0 5    
#define CONTROL1 4
#define CONTROL2 3
#define CONTROL3 2
#define JOY_UP_PIN 6
#define JOY_DOWN_PIN 7
#define JOY_UP_IDX 0
#define JOY_DOWN_IDX 1
#define DEBOUNCE_DELAY 50
#define PROXIMITY_MODE 1
#define DRAW_MODE 2
#define SAND_MODE 3
#define ATTRACT_MODE 5
#define COLOR_SKIP 10

// System Variables
Adafruit_WS2801 grid = Adafruit_WS2801(ROWS*COLS); // Data / Clock pins = 11 / 13 on Uno
unsigned int system_mode = ATTRACT_MODE;
byte selected_color = 0;

// Grid variables
//uint32_t grid_state[ROWS][COLS];

// Button Variables
int button_pins[] = {JOY_UP_PIN, JOY_DOWN_PIN};
int button_reading[BUTTONS];
int button_state[BUTTONS];
int last_button_state[BUTTONS];
long last_debounce_time[BUTTONS];

// Proximity Sensor Variables
int proximity_readings[PROXIMITY_SENSORS];

void setup() {
  
  // Set initial values
  memset(last_button_state, LOW, sizeof(int)*BUTTONS);
  memset(last_debounce_time, 0, sizeof(long)*BUTTONS);
  memset(proximity_readings, 0, sizeof(int)*PROXIMITY_SENSORS);
  
  // Set up pins
  pinMode(JOY_UP_PIN, INPUT);
  pinMode(JOY_DOWN_PIN, INPUT);
  
  // Set up grid
  grid.begin();
  grid.show();
}


void loop() {
  
  // Read button inputs
  for(int i=0; i<2; ++i) {
    button_reading[i] = digitalRead(button_pins[i]);
    if (button_reading[i] != last_button_state[i]) {
      last_debounce_time[i] = millis();
    } 
    if ((millis() - last_debounce_time[i]) > DEBOUNCE_DELAY) {
      button_state[i] = button_reading[i];
    }
    last_button_state[i] = button_reading[i];
  }
  
  // Change mode on button press
  for(int i=2; i<BUTTONS; ++i) {
    if(button_reading[i] == HIGH) {
      system_mode = i-1;
    }
  }
  
  // Adjust selected color
  if(button_reading[JOY_UP_IDX] == HIGH) {
    selected_color += 1;
    ShowUserColor(Wheel(selected_color));
    return;
  }
  if(button_reading[JOY_DOWN_IDX] == HIGH) {
    selected_color -= 1;
    ShowUserColor(Wheel(selected_color));
    return;
  }
  
  // Read proximity sensors
  for(int i=0; i<16; ++i) {
    digitalWrite(CONTROL0, (i&15)>>3); 
    digitalWrite(CONTROL1, (i&7)>>2);  
    digitalWrite(CONTROL2, (i&3)>>1);  
    digitalWrite(CONTROL3, (i&1));     
    proximity_readings[i] = analogRead(0);
    proximity_readings[16+i] = analogRead(1);
    if(i<=7) proximity_readings[32+i] = analogRead(2);
  }
  
  // Perform current system mode
  switch(system_mode) {
    
    // Pixels animate on hover
    case PROXIMITY_MODE:
      for(int i=0; i<PROXIMITY_SENSORS; ++i) {
        if(proximity_readings[i] < 180) {
          int x = i/(COLS/2);
          int y = i%(COLS/2);
          SetPixel(2*x, 2*y, Wheel(selected_color));
          SetPixel(2*x, 2*y+1, Wheel(selected_color));
          SetPixel(2*x+1, 2*y, Wheel(selected_color));
          SetPixel(2*x+1, 2*y+1, Wheel(selected_color));
        }
      }
    break;
    
    // Pixels turn on when hovered, fade over time.
    case DRAW_MODE:
      for(int i=0; i<PROXIMITY_SENSORS; ++i) {
        if(proximity_readings[i] < 180) {
          int x = i/(COLS/2);
          int y = i%(COLS/2);
          SetPixel(2*x, 2*y, Wheel(selected_color));
          SetPixel(2*x, 2*y+1, Wheel(selected_color));
          SetPixel(2*x+1, 2*y, Wheel(selected_color));
          SetPixel(2*x+1, 2*y+1, Wheel(selected_color));
        }
      }
      for(int i=0; i<ROWS*COLS; ++i) {
        if(grid.getPixelColor(i) > 0) 
          grid.setPixelColor(i, grid.getPixelColor(i)-0x010101);
      }
    break;
    
    // Pixels move like sand around hands
    case SAND_MODE:
    
    break;
    
    // Static animations to attract users
    case ATTRACT_MODE:
      colorWipe(Color(255, 0, 0), 0);
      colorWipe(Color(0, 255, 0), 0);
      colorWipe(Color(0, 0, 255), 0);
      ColorZoom(-1, 3);
      ColorZoom(1, 3);
      rainbowCycle(0);
      rainbow(0);
    break;  
  }
  grid.show();
}

/* Static Display Functions */

// Loops through color wheel from center to edges
// dir : -1 for moving outward, +1 for moving inward
// loops : number of loops to perform
void ColorZoom(int dir, int loops) {
  for(int i=0; i<256*loops; i+=COLOR_SKIP) { // Different colors
      for(int j=0; j<COLS/2; ++j) { // Different rings
        for(int k=0; k<=j; ++k) {
          SetPixel(COLS/2 + j - k, ROWS/2 + j, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 + j, ROWS/2 + j - k, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 - j + k - 1, ROWS/2 - j - 1, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 - j - 1, ROWS/2 - j + k - 1, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 - j + k - 1, ROWS/2 + j, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 - j - 1, ROWS/2 + j - k, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 + j - k, ROWS/2 - j - 1, Wheel((i+dir*COLOR_SKIP*j)%256));
          SetPixel(COLS/2 + j, ROWS/2 - j + k, Wheel((i+dir*COLOR_SKIP*j)%256));
        }
        grid.show();
      }
  }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < grid.numPixels(); i++) {
      grid.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    grid.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < grid.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / grid.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      grid.setPixelColor(i, Wheel( ((i * 256 / grid.numPixels()) + j) % 256) );
    }  
    grid.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < grid.numPixels(); i++) {
      grid.setPixelColor(i, c);
      grid.show();
      delay(wait);
  }
}



/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Set pixel to color at (x,y) coordinate
void SetPixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x>=COLS || y>=ROWS) return;
  if(y%2 == 0)
    grid.setPixelColor(y*COLS + x, color);
  else
    grid.setPixelColor(y*COLS + (COLS-x-1), color);
}

void ShowUserColor(uint32_t color) {
  for(int i=0; i<ROWS*COLS; ++i)
     grid.setPixelColor(i, color);
  grid.show();
}