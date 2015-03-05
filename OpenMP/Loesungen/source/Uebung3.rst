Aufgabe 1
=========

a)
--

  Der Quellcode wurde um die folgenden Regeln ergänzt.
::

  int evolve(unsigned* currentfield, unsigned* newfield, int w, int h) {
  int changes = 0;

    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        int sum = 0;
        sum += currentfield[calcIndex(w,h,x+1,y)];
        sum += currentfield[calcIndex(w,h,x+1,y+1)];
        sum += currentfield[calcIndex(w,h,x+1,y-1)];

        sum += currentfield[calcIndex(w,h,x-1,y)];
        sum += currentfield[calcIndex(w,h,x-1,y+1)];
        sum += currentfield[calcIndex(w,h,x-1,y-1)];

        sum += currentfield[calcIndex(w,h,x,y-1)];
        sum += currentfield[calcIndex(w,h,x,y+1)];
        sum += currentfield[calcIndex(w,h,x,y)];

        if(sum == 2 || sum == 3){
          newfield[calcIndex(w,h,x,y)] = 1;
        } else {
          newfield[calcIndex(w,h,x,y)] = 0;
        }

        if(changes==0 && currentfield[calcIndex(w,h,x,y)] != newfield[calcIndex(w,h,x,y)]){
          changes = 1;
        }
      }
    }
    return changes;
  }

  Die periodischen Ränder wurden in der Methode currentfield ungesetzt. Hierfür wurde die Parameterliste um die angabe der Höhe des Feldes ergänzt.

::

  int calcIndex(int width, int heigth, int x, int y) {
    if(x > width){
      x = 0;
    }else if(x < 0){
      x = width;
    }
    if(y > heigth){
      y = 0;
    }else if(y < 0){
      y = heigth;
    }
    return y * width + x;
  }

  Das Rausschreiben der Daten erfolgte über die schon vorhandene Methode.

Aufgabe 2
=========

!!!!! Muss noch gemacht werden.

Aufgabe 3
=========

!!!!! Muss noch gemacht werden.
