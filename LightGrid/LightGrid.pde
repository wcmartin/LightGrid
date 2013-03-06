/**
 * LightGrid
 *
 * Simulates a grid of interactive lights.
 */

import java.awt.Color;
import processing.serial.*;

// Constants
static final int GRID_WIDTH = 8;
static final int GRID_HEIGHT = 6;
static final int PADDING = 30;
static final int PANEL_MARGIN = 3;

// Globals
int panelSide = 50;
int winWidth = 2*PADDING + GRID_WIDTH*panelSide + (GRID_WIDTH+1)*PANEL_MARGIN;
int winHeight = 2*PADDING + GRID_HEIGHT*panelSide + (GRID_HEIGHT+1)*PANEL_MARGIN;
byte[][][] gridState = new byte[GRID_WIDTH][GRID_HEIGHT][3];
color drawColor = #00FF00;
color backgroundColor = #000000;
SnakeGame z;
Serial myPort; 
SerialThread sthread = new SerialThread();

void setup() {
  size(winWidth, winHeight);
  background(0);
  frameRate(30);
  smooth();
  colorMode(RGB, 31);
  setupGUI();
  initializeGridModes();
  setupSerial();
  
  z.startGame();
}

void draw() {
  z.updateBoard(keyCode);
  drawGridState();
  
  // Not actually threaded yet
  sthread.createGridData();
}

void setupGUI() {
  drawGridOutline();
  drawGridState();
}

void setupSerial() {
  println(Serial.list());
  String portName = Serial.list()[4];
  myPort = new Serial(this, portName, 9600);
}

void initializeGridModes() {
  z = new SnakeGame();
}

void drawGridOutline() {
  stroke(31);
  strokeWeight(PANEL_MARGIN);
  strokeCap(SQUARE);
  for (int i=0; i<=GRID_WIDTH; i++) { // Vertical lines
    line(PADDING+i*(panelSide+PANEL_MARGIN)+PANEL_MARGIN/2, PADDING, PADDING+i*(panelSide+PANEL_MARGIN)+PANEL_MARGIN/2, PADDING+GRID_HEIGHT*(panelSide+PANEL_MARGIN)+PANEL_MARGIN/2);
  }
  for (int j=0; j<=GRID_HEIGHT; j++) { // Horizontal lines
    line(PADDING, PADDING+j*(panelSide+PANEL_MARGIN)+PANEL_MARGIN/2, PADDING+GRID_WIDTH*(panelSide+PANEL_MARGIN)+PANEL_MARGIN/2, PADDING+j*(panelSide+PANEL_MARGIN)+PANEL_MARGIN/2);
  }
}

void drawGridState() {
  noStroke();
  for (int j=0; j<GRID_HEIGHT; j++) {
    for (int i=0; i<GRID_WIDTH; i++) {
      fill(gridState[i][j][0], gridState[i][j][1], gridState[i][j][2]);
      rect(PADDING+(panelSide+PANEL_MARGIN)*i+PANEL_MARGIN, 
      PADDING+(panelSide+PANEL_MARGIN)*j+PANEL_MARGIN, 
      panelSide, panelSide);
    }
  }
}

void setColor(int x, int y, color c) {
  try {
    gridState[x][y][0] = byte(red(c));
    gridState[x][y][1] = byte(green(c));
    gridState[x][y][2] = byte(blue(c));
  } 
  catch(Exception ArrayIndexOutOfBoundsException) {
    println("Outside grid, x: "+x+" y: "+y);
  }
}

void setColorRandom(int x, int y) {
  gridState[x][y][0] = byte(random(32));
  gridState[x][y][1] = byte(random(32));
  gridState[x][y][2] = byte(random(32));
}

void setBackgroundColor(int h) {
  if(h==1) backgroundColor = color(0,0,0);
  else if(h==0) backgroundColor = color(31,31,31);
  else backgroundColor = Color.HSBtoRGB(h/360.0,1.0,1.0);
}

void clearGrid() {
  for(int j=0; j<GRID_HEIGHT; j++) {
    for(int i=0; i<GRID_WIDTH; i++) {
      setColor(i,j,backgroundColor);
    }
  }
}
