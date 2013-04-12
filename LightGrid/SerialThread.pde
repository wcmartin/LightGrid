class SerialThread {

  byte[] dataLedGrid;
  
  SerialThread() {
    dataLedGrid = new byte[GRID_WIDTH*GRID_HEIGHT*3];
  }
  
  public void run() {
    if(myPort != null) {
      myPort.write(dataLedGrid);
    }
    else 
      System.out.println("no serial.");
  }
  
  void createGridData() {
    int dataPos = 0;
    for(int j=0; j<GRID_HEIGHT; j++) {
      for(int i=0; i<GRID_WIDTH; i++) {
         byte r = gridState[i][j][0];
         byte g = gridState[i][j][1];
         byte b = gridState[i][j][2];
         
         dataLedGrid[dataPos] = r;
         dataLedGrid[dataPos+1] = g;
         dataLedGrid[dataPos+2] = b;
         dataPos+=3;
      }
    }
    myPort.write(dataLedGrid);
    while(myPort.available() == -1) {}
    //System.out.println(myPort.read());
    //System.out.println(myPort.read());
    //System.out.println(myPort.read());
    System.out.println();
  }
}
