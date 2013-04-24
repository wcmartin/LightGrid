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
#define NUM_SENSOR_READINGS 3
#define ACTIVE_SENSORS 40
#define ANIMATE_SPEED 40
#define ATTRACT_TIMEOUT 600000
#define FUN_TIMEOUT 4000

#define DSERIAL     if(DEBUG) Serial.print
#define DSERIAL_LN  if(DEBUG) Serial.println
#define DEBUG                 1

// System Variables
Adafruit_WS2801 grid = Adafruit_WS2801(ROWS*COLS); // Data / Clock pins = 11 / 13 on Uno
unsigned int system_mode = PROXIMITY_MODE;
byte selected_color = 0;

// Grid variables
uint32_t grid_state[COLS][ROWS];

long last_animation = 0;
long last_input = 0;
long last_fun = 0;

// Proximity Sensor Variables
uint16_t proximity_readings[PROXIMITY_SENSORS][NUM_SENSOR_READINGS];
uint8_t current_proximity_array = 0; 

void setup() {
  Serial.begin(115200);
  // Set initial values
  memset(proximity_readings, 0, sizeof(int)*PROXIMITY_SENSORS*NUM_SENSOR_READINGS);
  
  // Set up pins
  pinMode(CONTROL0, OUTPUT);
  pinMode(CONTROL1, OUTPUT);
  pinMode(CONTROL2, OUTPUT);
  pinMode(CONTROL3, OUTPUT);
  
  // Set up grid
  grid.begin();
  LoadGrid();
  grid.show();
}


void loop() {
  selected_color = (millis() / 300) % 256;
  
  // Read proximity sensors
  for(int i=0; i<=15; ++i) {
    digitalWrite(CONTROL0, (i&15)>>3); 
    digitalWrite(CONTROL1, (i&7)>>2);  
    digitalWrite(CONTROL2, (i&3)>>1);  
    digitalWrite(CONTROL3, (i&1));     
    proximity_readings[i][current_proximity_array] = uint16_t(analogRead(0));
    proximity_readings[16+i][current_proximity_array] = uint16_t(analogRead(1));
    if(i<=7) proximity_readings[32+i][current_proximity_array] = uint16_t(analogRead(2));
  }
  ToggleProximityArray();
  
          
  // Perform current system mode
  switch(system_mode) {
    
    // Pixels animate on hover
    case PROXIMITY_MODE:
    {

      
      int got_input = 0;
      for(int i=0; i < ACTIVE_SENSORS; ++i) {
        int c_max = max_readings(i);
        int c_min = min_readings(i);
        int diff = abs(c_max - c_min);
//          DSERIAL("index is ");
//          DSERIAL(i);
//          DSERIAL(" and diff is ");
//          DSERIAL(diff);
//          DSERIAL(" and max is ");
//          DSERIAL(c_max);
//          DSERIAL(" and min is ");
//          DSERIAL_LN(c_min);
        
        int y = i / (COLS/2);
        int x = i % (COLS/2);
        
        if(diff >= 18) {
          got_input = 1;
          //grid_state[2*x][2*y] = Wheel(selected_color);
          SetPixel(2*x, 2*y, Wheel(selected_color));
//          SetPixel(2*x, 2*y+1, Wheel(selected_color));
//          SetPixel(2*x+1, 2*y, Wheel(selected_color));
//          SetPixel(2*x+1, 2*y+1, Wheel(selected_color));
        } 
      }
      
      if(got_input) {
        last_input = millis();
        last_fun = last_input;
      }
      if(millis() - last_fun > FUN_TIMEOUT) {
        last_fun = millis();
        int bound = random(3,8);
        for(int i=0; i<=bound; ++i) {
          SetPixel(random(0,COLS), random(0,ROWS), Wheel(selected_color));
        }
      }
      
      if(millis() - last_input > ATTRACT_TIMEOUT) system_mode = ATTRACT_MODE;
      
      for(int y=0; y<ROWS; ++y) {
        for(int x=0; x<COLS; ++x) {
          fade_pixel(x,y); 
        }
      }
      
      // animate
      if(millis() - last_animation > ANIMATE_SPEED) {
        last_animation = millis();
        for(int i=0; i<ACTIVE_SENSORS; ++i)
        {
          int y = i / (COLS/2);
          int x = i % (COLS/2);
          int xx = 0;
          int yy = 0;
          int done = 0;
          for(yy=0; yy<=1; ++yy) {
            for(xx=0; xx<=1; ++xx) {
              uint32_t current_color = GetPixel(2*x+xx, 2*y+yy);
              if(current_color != 0) {
                SetPixel(2*x+xx, 2*y+yy, 0);
                if(xx == 0 && yy == 0) { // top right
                  SetPixel(2*x+1, 2*y, current_color); 
                } else if(xx == 1 && yy == 0) { // top left
                  SetPixel(2*x+1, 2*y+1,  current_color); 
                } else if(xx == 1 && yy == 1) { // bottom left
                  SetPixel(2*x, 2*y+1,  current_color); 
                } else if(xx == 0 && yy == 1) { // xx == 0 && yy == 1 // bottom right
                  SetPixel(2*x, 2*y,  current_color);
                }
                done = 1;
                break;
              }
            }
            if(done) break;
          }
        }
      }
  }
    break;
    
    // Static animations to attract users
    case ATTRACT_MODE: 
    {
      colorWipe(Color(255, 0, 0), 0);
      if(system_mode == PROXIMITY_MODE) break;
      colorWipe(Color(0, 255, 0), 0);
      if(system_mode == PROXIMITY_MODE) break;
      colorWipe(Color(0, 0, 255), 0);
      if(system_mode == PROXIMITY_MODE) break;
      ColorZoom(-1, 3);
      if(system_mode == PROXIMITY_MODE) break;
      //ColorZoom(1, 3);
      rainbowCycle(0);
      //rainbow(0);
      last_input = millis();
      system_mode = PROXIMITY_MODE;
    break;  
    }
  }
  
  LoadGrid();
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
        if(CheckInput()) system_mode = PROXIMITY_MODE;
        LoadGrid();
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
    if(CheckInput()) system_mode = PROXIMITY_MODE;  
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
  if(CheckInput()) system_mode = PROXIMITY_MODE;  
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
      if(CheckInput()) system_mode = PROXIMITY_MODE;
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
  grid_state[x][y] = color;
}

uint32_t GetPixel(uint16_t x, uint16_t y) {
  return grid_state[x][y];
}

void LoadGrid() {
  for(int y=0; y<ROWS; ++y) {
    for(int x=0; x<COLS; ++x) {
    if(y%2 == 0)
      grid.setPixelColor(y*COLS + x, grid_state[x][y]);
    else
      grid.setPixelColor(y*COLS + (COLS-x-1), grid_state[x][y]);
    }
  }
}

void ShowUserColor(uint32_t color) {
  for(int i=0; i<ROWS*COLS; ++i)
     grid.setPixelColor(i, color);
  grid.show();
}

void ToggleProximityArray() 
{
  current_proximity_array = (current_proximity_array + 1) % NUM_SENSOR_READINGS;
  DSERIAL_LN(current_proximity_array);
}

uint16_t max_readings(int idx)
{
  int cur_ind;
  int cur_max = 0;
  for (cur_ind = 0; cur_ind < NUM_SENSOR_READINGS; cur_ind++)
  {
    if(cur_max < proximity_readings[idx][cur_ind])
    {
      cur_max = proximity_readings[idx][cur_ind];
    }
  }
  
  return cur_max;
}

uint16_t min_readings(int idx)
{  
  int cur_ind;
  int cur_min = 1023;
  for (cur_ind = 0; cur_ind < NUM_SENSOR_READINGS; cur_ind++)
  {
    if(cur_min > proximity_readings[idx][cur_ind])
    {
      cur_min = proximity_readings[idx][cur_ind];
    }
  }
  
  return cur_min;
}

void fade_pixel(uint16_t x, uint16_t y) 
{
        uint32_t current_color = grid_state[x][y];
        uint32_t red = (current_color & 0x000000FF);
        uint32_t green =  (current_color & 0x0000FF00) >> 8;
        uint32_t blue = (current_color & 0x00FF0000) >> 16;
        
        if(red > 0) red--;
        if(blue > 0) blue--;
        if(green > 0) green--;
        
        grid_state[x][y] = red | (green << 8) | (blue << 16);
}

int CheckInput() {
      int got_input = 0;
      for(int i=0; i < ACTIVE_SENSORS; ++i) {
        int c_max = max_readings(i);
        int c_min = min_readings(i);
        int diff = abs(c_max - c_min);
        
        if(diff >= 18) {
          return 1;
        } 
      }
      return 0;
}
