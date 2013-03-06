#include <FastSPI_LED.h>

#define NUM_LEDS 48
#define WIDTH 8
#define HEIGHT 6
#define BUTTON_PIN 2
#define PROXIMITY1_PIN A0
#define PROXIMITY2_PIN A1
#define DEBOUNCE_DELAY 50
#define PROXIMITY_TIME_ON 500

int hue = 1;
int systemState = 4;
int lastButtonState = LOW;
int lastProxState1 = 0;
boolean buttonOpen = true;
long lastDebounceTime = 0;
long lastDebounceTime1 = 0;
long previousMillis = 0;

#define PIN 4

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_LPD6803);
  FastSPI_LED.setPin(PIN);
  FastSPI_LED.init();
  FastSPI_LED.start();
  
  unsigned char *pData = FastSPI_LED.getRGBData();
  for(short i=0; i < NUM_LEDS; ++i) {
      *pData++ = 0;
      *pData++ = 0;
      *pData++ = 0;
  }
  FastSPI_LED.show();
}

void loop() {
  unsigned char *pData = FastSPI_LED.getRGBData();
  int reading = digitalRead(BUTTON_PIN);
  unsigned long currentMillis = millis();
  
  /* Debounce the button properly */
  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis;
  } 
  if ((currentMillis - lastDebounceTime) > DEBOUNCE_DELAY) {
    if(reading == LOW && buttonOpen) {    
      buttonOpen = false;
      systemState++;
      if(systemState>4) systemState = 1;
    }
    if(reading == HIGH) {
      buttonOpen = true;
    }
  }
  lastButtonState = reading;
  
  /* Perform system operation */
  switch(systemState) {
    case 1:  // Falling Colors
    {
      int skip = 1;
      for(int j = 0; j < HEIGHT; j++) {
        for(int i = 0; i < WIDTH; i++) {
          int pos = j*WIDTH + i;
          int h = j*skip+hue;
          if(h>359) h-=359;
          LedHSV(h, 1, 1, pos);
        }
      }
      hue+=skip;
      if(hue > 359) hue = 1;
    }
    break;
    
    case 2:
    {
      for(int j = 0; j < HEIGHT; j++) {
        for(int i = 0; i < WIDTH; i++) {
          int pos = j*WIDTH + i;
          LedHSV(180, 1, 1, pos);
        }
      }
    }
    break;
    
    case 3: // Proximity
    {
      int proximity1 = analogRead(PROXIMITY1_PIN); 
      Serial.println(proximity1);
      for(int j = 0; j < HEIGHT; j++) {
        for(int i = 0; i < WIDTH; i++) {
          int value =  map(proximity1-120, 0, 180, 0, 255);
          if(proximity1 < 140) value = 0;
          *(pData+0+(j*WIDTH + i)*3) = 0;
          *(pData+1+(j*WIDTH + i)*3) = 0;
          *(pData+2+(j*WIDTH + i)*3) = value;
        }
      } 
    }
    break;
    
    case 4: // Serial command
    {
      unsigned int c1, c2, c3, pos;
      if (Serial.available() > 0) {
        for(byte j=0; j<HEIGHT; ++j) {
            for(byte i=0; i<WIDTH; ++i) {
              c1 = Serial.read();
              c2 = Serial.read();
              c3 = Serial.read();
              
              pos = i+WIDTH*j;
              if(j % 2 != 0)
                pos = WIDTH*(j+1)-i-1;

              *(pData+pos*3) = c1 * 8;
              *(pData+1+pos*3) = c2 * 8;
              *(pData+2+pos*3) = c3 * 8;
            }
        }
      }
    }
    break;
  }
  
  FastSPI_LED.show();
}


//Convert a given HSV (Hue Saturation Value) to RGB(Red Green Blue) and set the led to the color
//  h is hue value, integer between 0 and 360
//  s is saturation value, double between 0 and 1
//  v is value, double between 0 and 1
void LedHSV(int h, double s, double v, int pos) {
  //this is the algorithm to convert from RGB to HSV
  double r=0; 
  double g=0; 
  double b=0;

  double hf=h/60.0;

  int i=(int)floor(h/60.0);
  double f = h/60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s*f);
  double tv = v * (1 - s * (1 - f));

  switch (i)
  {
  case 0: //rojo dominante
    r = v;
    g = tv;
    b = pv;
    break;
  case 1: //verde
    r = qv;
    g = v;
    b = pv;
    break;
  case 2: 
    r = pv;
    g = v;
    b = tv;
    break;
  case 3: //azul
    r = pv;
    g = qv;
    b = v;
    break;
  case 4:
    r = tv;
    g = pv;
    b = v;
    break;
  case 5: //rojo
    r = v;
    g = pv;
    b = qv;
    break;
  }

  //set each component to a integer value between 0 and 255
  int red=constrain((int)255*r,0,255);
  int green=constrain((int)255*g,0,255);
  int blue=constrain((int)255*b,0,255);
  unsigned char *pData = FastSPI_LED.getRGBData();
  *(pData+pos*3) = red;
  *(pData+1+pos*3) = green;
  *(pData+2+pos*3) = blue;
}
