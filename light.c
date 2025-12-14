#ifdef _WIN32
  #include <GL/glew.h>
  #include <GL/freeglut.h>
  #include <opencv4/opencv2/opencv.hpp>
#endif
#include <GL/glut.h>
#include <math.h>
#include "light.h"
#include "camera.h"

void initlight(){
  glEnable(GL_LIGHT4);
}

void setlight(){
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHT3);
  glEnable(GL_LIGHT5);
  glEnable(GL_LIGHT6);
  glEnable(GL_LIGHT7);
}

void rmlight(){
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHT2);
  glDisable(GL_LIGHT3);
  glDisable(GL_LIGHT5);
  glDisable(GL_LIGHT6);
  glDisable(GL_LIGHT7);
}

GLfloat lightPosition[4] ;
GLfloat lightDiffuse[3];
GLfloat lightAmbient[3]  = { 0.1, 0.1, 0.1 };
GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0  };
GLfloat spotDirrection[3];

// fluorescent
GLfloat fl_pos[4];
GLfloat fl_ambient[3]  = {0.2, 0.2, 0.2};
GLfloat fl_specular[3] = {0.6, 0.6, 0.6};
GLfloat fl_diffuse[3]  = {0.8, 0.9, 1.0};

void setLightPos(GLfloat x, GLfloat y, GLfloat z){
  lightPosition[0] = x;
  lightPosition[1] = y;
  lightPosition[2] = z;
  lightPosition[3] = 1.0;
}

void setLightDif(GLfloat x, GLfloat y, GLfloat z){
  lightDiffuse[0] = x;
  lightDiffuse[1] = y;
  lightDiffuse[2] = z;
}

void setLightDir(GLfloat x, GLfloat y, GLfloat z){
  spotDirrection[0] = x;
  spotDirrection[1] = y;
  spotDirrection[2] = z;
}

void regLight(GLenum light){
  glLightfv(light, GL_POSITION, lightPosition);  
  glLightfv(light, GL_DIFFUSE,  lightDiffuse);
  glLightfv(light, GL_AMBIENT,  lightAmbient);
  glLightfv(light, GL_SPECULAR, lightSpecular);
  glLightf( light, GL_LINEAR_ATTENUATION, 0.01);
  glLightfv( light, GL_SPOT_DIRECTION, spotDirrection );
  glLightf( light, GL_SPOT_CUTOFF,  20.0 );
  glLightf( light, GL_SPOT_EXPONENT, 0 );
}

void setFluorescentPos(GLfloat x, GLfloat y, GLfloat z){
    fl_pos[0] = x;
    fl_pos[1] = y;
    fl_pos[2] = z;
    fl_pos[3] = 1.0;   // Point light
}

void regFluorescent(GLenum light){
    glLightfv(light, GL_POSITION, fl_pos);
    glLightfv(light, GL_DIFFUSE,  fl_diffuse);
    glLightfv(light, GL_AMBIENT,  fl_ambient);
    glLightfv(light, GL_SPECULAR, fl_specular);

    glLightf(light, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(light, GL_LINEAR_ATTENUATION,   0.02);
    glLightf(light, GL_QUADRATIC_ATTENUATION,0.01);
    glLightf(light, GL_SPOT_CUTOFF, 180.0);
}

void setSpotlight(){
  setLightPos(-0.5, 5.7, 2);
  setLightDif(0.2, 0.2, 0.2);
  setLightDir(0, -1.0, 0);
  regLight(GL_LIGHT1);
}

void setFluorescentLight(){
    setFluorescentPos(-0.5, 5.7, 2);
    regFluorescent(GL_LIGHT2);
    glEnable(GL_LIGHT2);

    setFluorescentPos(-15.5, 5.7, -8);
    regFluorescent(GL_LIGHT3); 
    glEnable(GL_LIGHT3);

    setFluorescentPos(-15.5, 5.7, -18);
    regFluorescent(GL_LIGHT5);
    glEnable(GL_LIGHT5);

    setFluorescentPos(-25.5, 5.7, -38);
    regFluorescent(GL_LIGHT6);
    glEnable(GL_LIGHT6);

    setFluorescentPos(9.5, 5.7, 22);
    regFluorescent(GL_LIGHT7);
    glEnable(GL_LIGHT7);
}
