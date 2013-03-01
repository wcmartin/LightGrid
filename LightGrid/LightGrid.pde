/**
 * LightGrid
 *
 * Simulates a grid of interactive lights.
 */

// Constants
static final int GRID_WIDTH = 10;
static final int GRID_HEIGHT = 10;
static final int PADDING = 30;
static final int PANEL_MARGIN = 3;

// Globals
int panelSide = 50;
int winWidth = 2*PADDING + GRID_WIDTH*panelSide + (GRID_WIDTH+1)*PANEL_MARGIN;
int winHeight = 2*PADDING + GRID_HEIGHT*panelSide + (GRID_HEIGHT+1)*PANEL_MARGIN;
int[][][] gridState = new int[GRID_WIDTH][GRID_HEIGHT][3];

void setup() {
  size(winWidth, winHeight);
  background(0);
  smooth();
  colorMode(RGB, 31);
  setupGUI();
}

void draw() {
}

void setupGUI() {
  drawGridOutline();
  drawGridState();
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
      fill(204,102,0);//gridState[i][j][0], gridState[i][j][1], gridState[i][j][2]);
      rect(PADDING+(panelSide+PANEL_MARGIN)*i+PANEL_MARGIN, 
      PADDING+(panelSide+PANEL_MARGIN)*j+PANEL_MARGIN, 
      panelSide, panelSide);
    }
  }
}

void setColor(int x, int y, color c) {
  try {
    gridState[x][y][0] = int(red(c));
    gridState[x][y][1] = int(green(c));
    gridState[x][y][2] = int(blue(c));
  } catch(Exception ArrayIndexOutOfBoundsException) {
    println("Outside grid, x: "+x+" y: "+y);
  }
}

