class SerialThread {

  byte[] dataLedGrid;
  
  SerialThread() {
    dataLedGrid = new byte[WIDTH*HEIGHT*3];
  }
  
  public void run() {
    if(myPort != null)
      myPort.write(dataLedGrid);
    else 
      System.out.println("no serial.");
  }
  
  void createGridData() {
    int dataPos = 0;
    for(int j=0; j<HEIGHT; j++) {
      for(int i=0; i<WIDTH; i++) {
         byte r = gridState[i][j][0];
         byte g = gridState[i][j][1];
         byte b = gridState[i][j][2];
         
         dataLedGrid[dataPos] = r;
         dataLedGrid[dataPos+1] = g;
         dataLedGrid[dataPos+2] = b;
         dataPos+=3;
      }
    }
    sthread.run();
  }
}
