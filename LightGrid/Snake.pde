import java.util.Queue;
import java.util.LinkedList;

class SnakeGame {

  int xFood, yFood, time;
  Snake s;
  
  SnakeGame() {
    xFood = int(random(GRID_WIDTH));
    yFood = int(random(GRID_HEIGHT));
  }
  
  void startGame() {
    s = new Snake();
    s.start();
    updateBoard(RIGHT);
    time = millis();
  }
  
  void updateBoard(int k) {
    if(s != null) {
      if(keyPressed && key == CODED)
        s.setDir(k);
      if(s.xHead == xFood && s.yHead == yFood) {
        s.S_length += 1;
        if(s.speed > 30)
          s.speed -= 2;
        xFood = int(random(GRID_WIDTH));
        yFood = int(random(GRID_HEIGHT));
      }
      if(millis()-time > 150) {
        setColorRandom(xFood, yFood);
        time = millis();
      }
    }
  }
  
  void stopGame() {
    if(s != null) s.kill();
    s = null;
  }
  

}

class Snake extends Thread {
  
  int S_length, xHead, yHead, dir, speed, badDir, lastDir;
  boolean alive;
  Queue<Pair> body;
  
  Snake() {
    S_length = 3;
    xHead = yHead = 0;
    dir = RIGHT;
    badDir = LEFT;
    speed = 200;
    alive = true;
    body = new LinkedList<Pair>();
    body.add(new Pair(xHead, yHead));
    lastDir = dir;
  }
  
  void run() {
    while(alive) {
      if(dir == badDir) {
        dir = lastDir;
      }
      
      try {
        this.sleep(speed);
      } 
      catch(InterruptedException e) {}
      
      if(dir == badDir) {
        dir = lastDir;
        continue;
      }
      
      int dx = 0, dy = 0;
      switch(dir) {
        case UP:
          dy = -1;
          break;
        case RIGHT:
          dx = 1;
          break;
        case DOWN:
          dy = 1;
          break;
        case LEFT:
          dx = -1;
          break;
      }
      
      
      xHead += dx;
      yHead += dy;
      Pair h = new Pair(xHead,yHead);
      if(xHead >= GRID_WIDTH || yHead >= GRID_HEIGHT || xHead < 0 || yHead < 0 || body.contains(h)) {
        alive = false;
        break;
      }
      
      if(body.size() >= S_length) {
        Pair remove = body.poll();
        setColor((int)remove.x, (int)remove.y, backgroundColor);
      }
      // add to body
      body.add(h);
      if(drawColor != backgroundColor)
        setColor(xHead, yHead, drawColor);
      else
        setColorRandom(xHead,yHead);
      // 40 down 39 right 38 up 37 left
      lastDir = dir;
      badDir = (dir == DOWN || dir == RIGHT) ? dir-2 : dir+2;
    }
    
    // you lose, delete snake, display message
    setBackgroundColor(1);
    clearGrid();
    String msg = "GAMEOVER";
    /*for(int i=0; i<msg.length(); ++i) {
      if(i*5<20)
        drawChar(i*5,4,msg.charAt(i), Color.HSBtoRGB(45.0*i/360.0,1.0,1.0));
      else
        drawChar(i*5-20,12,msg.charAt(i),Color.HSBtoRGB(45.0*i/360.0,1.0,1.0));
    }
    if(mode != 'z') clearGrid();*/
  }
  
  void setDir(int _dir) {
    if(dir-2 != _dir && dir+2 != _dir) {
      dir = _dir;
    }
  }
  
  void kill() {
    alive = false;
  }
}

class Pair {
  int x, y;
  Pair(int _x, int _y) {
    x = _x;
    y = _y;
  }
  
  public boolean equals(Object other) {
    if(other == null) return false;
    if(other == this) return true;
    if(this.getClass() != other.getClass()) return false;
    return (x == ((Pair)other).x && y == ((Pair)other).y);
  }
}
