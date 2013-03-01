boolean[] keys = new boolean[10];
final int W = 0;
final int S = 1;
final int A = 2;
final int D = 3;

final int _UP = 4;
final int _RIGHT = 5;
final int _DOWN = 6;
final int _LEFT = 7;

final int Z = 8;
final int X = 9;

int upPress = 0;
int leftPress = 0;
int rightPress = 0;

void keyPressed() {
   if(key == CODED) {
     switch(keyCode) {
      case UP:
        keys[_UP] = true;
        break;
      case DOWN:
        keys[_DOWN] = true;
        break;
      case RIGHT:
        keys[_RIGHT] = true;
        break;
      case LEFT:
        keys[_LEFT] = true;
        break;
     }
   } else {  
     switch(key) {
      case 'W':
      case 'w':
        keys[W] = true;
        break;
      case 'S':
      case 's':
        keys[S] = true;
        break;
      case 'A':
      case 'a':
        keys[A] = true;
        break;
      case 'D':
      case 'd':
        keys[D] = true;
        break;
      case 'Z':
      case 'z':
        keys[Z] = true;
        break;
      case 'X':
      case 'x':
        keys[X] = true;
        break;  
     }
   }
}

void keyReleased() {
   if(key == CODED) {
     switch(keyCode) {
      case UP:
        keys[_UP] = false;
        ++upPress;
        break;
      case DOWN:
        keys[_DOWN] = false;
        break;
      case RIGHT:
        keys[_RIGHT] = false;
        ++rightPress;
        break;
      case LEFT:
        keys[_LEFT] = false;
        ++leftPress;
        break;
     }
   } else {  
     switch(key) {
      case 'W':
      case 'w':
        keys[W] = false;
        break;
      case 'S':
      case 's':
        keys[S] = false;
        break;
      case 'A':
      case 'a':
        keys[A] = false;
        break;
      case 'D':
      case 'd':
        keys[D] = false;
        break;
      case 'Z':
      case 'z':
        keys[Z] = true;
        break;
      case 'X':
      case 'x':
        keys[X] = true;
        break;  
     }
   }
}
