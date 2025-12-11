#ifdef _WIN32
  #include <GL/glew.h>
  #include <GL/freeglut.h>
  #include <opencv4/opencv2/opencv.hpp>
#endif
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "camera.h"
#include "scene.h"

camera cam;

double oneStep = 0.4;

double thetax;
int collision = 1;

void initCamera(void){
  cam.pos_x = 0.0;
  cam.pos_y = -2.0;
  cam.pos_z = -10.0;
  cam.eye_x = 0;
  cam.eye_y = 0;
}

void setCamera(int x, int y){
  cam.eye_x = 360.0*x/width-180;
  cam.eye_y = 180.0*y/height-90;
  thetax = M_PI*cam.eye_x/180;
}

void moveForward(void){
  cam.pos_z+=cos(thetax)*oneStep;
  cam.pos_x-=sin(thetax)*oneStep;
}

void moveBack(void){
  cam.pos_z-=cos(thetax)*oneStep;
  cam.pos_x+=sin(thetax)*oneStep;
}

void moveLeft(void){
  cam.pos_z-=cos(thetax+M_PI/2)*oneStep;
  cam.pos_x+=sin(thetax+M_PI/2)*oneStep;
}

void moveRight(void){
  cam.pos_z-=cos(thetax-M_PI/2)*oneStep;
  cam.pos_x+=sin(thetax-M_PI/2)*oneStep;
}

void moveUp(void){
  cam.pos_y-=oneStep;
}

void moveDown(void){
  cam.pos_y+=oneStep;
}

void makeWall(){
  if (!collision) return;
  /*if( cam.pos_z >= -8.0 && cam.pos_z<=4 && cam.pos_x >= -2 && cam.pos_x <= 2){
    door1 = true;
  }else{
    door1 = false;
  }
  if(cam.pos_z >= 0.5 && cam.pos_z <= 10 && cam.pos_x >= 5 && cam.pos_x <= 10){
    door2 = true;
  }else{
    door2 = false;
  }
  if(door1 == false){
    if(cam.pos_z>-1.5 && cam.pos_z <0){
      if(cam.pos_x <21.5 && cam.pos_x >-6.5){
        cam.pos_z=-1.5;
      }
    }else if(cam.pos_z>=0 && cam.pos_z <1.5){
      if(cam.pos_x <21.5 && cam.pos_x >-6.5){
        cam.pos_z = 1.5;
      }
    }
  }

  if(door2 == false){
    if(cam.pos_z>4.5 && cam.pos_z <6.0){
      if(cam.pos_x <15.5 && cam.pos_x >-6.5){
        cam.pos_z=4.5;
      }
    }else if(cam.pos_z>=6.0 && cam.pos_z <7.5){
      if(cam.pos_x <15.5 && cam.pos_x >-6.5){
        cam.pos_z = 7.5;
      }
    }
  }*/

  // collision detection
  if(cam.pos_z <= -11 && cam.pos_z > -15){
    if(cam.pos_x > 1.5){
      cam.pos_x = 1.5;
    }
    if(cam.pos_z < -13.5 && cam.pos_x < 8) {
      cam.pos_z = -13.5;
    }
  }

  if(cam.pos_z < 0 && cam.pos_z > -11){
    if(cam.pos_x < -0.8){
      cam.pos_x = -0.8;
    }
    if(cam.pos_x > 1.5){
      cam.pos_x = 1.5;
    }
  }

  if(cam.pos_z < 5 && cam.pos_z >= 0){
    if(cam.pos_x < -0.8){
      cam.pos_x = -0.8;
    }
    if(cam.pos_x > 16.5){
      cam.pos_x = 16.5;
    }
    if(cam.pos_z > 3 && cam.pos_x < 14){
      cam.pos_z = 3;
    }
    if(cam.pos_z < 1.5 && cam.pos_x < 17 && cam.pos_x > 1.5){
      cam.pos_z = 1.5;
    }
  }

  if(cam.pos_z < 25 && cam.pos_z >= 5){
    if(cam.pos_x > 16.5){
      cam.pos_x = 16.5;
    }
    if(cam.pos_x < 14.5){
      cam.pos_x = 14.5;
    }
  }

  if(cam.pos_z < 30 && cam.pos_z >= 25){
    if(cam.pos_x < 14.5){
      cam.pos_x = 14.5;
    }
    if(cam.pos_x > 26.5){
      cam.pos_x = 26.5;
    }
    if(cam.pos_z > 28 && cam.pos_x < 23){
      cam.pos_z = 28;
    }
    if(cam.pos_z < 26.5 && cam.pos_x < 27 && cam.pos_x > 16.5){
      cam.pos_z = 26.5;
    }    
  }

  // teleport
  if(cam.pos_x < 28 && cam.pos_x > 23){
    if(cam.pos_z > 29){
      cam.pos_x = 0.0;
      cam.pos_z = -10.0;
    }
  }

  if(cam.pos_x < -1 && cam.pos_z <= -11 && cam.pos_z > -15){
    cam.pos_x = 23;
    cam.pos_z = 27;

  }
  
  // printf("%f %f %f\n",cam.pos_x,cam.pos_y,cam.pos_z);
}