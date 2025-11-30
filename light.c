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
GLfloat lightAmbient[3]  = { 0.1, 0.1, 0.1 }; //環境光
GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0  }; //鏡面光
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
  glLightfv( light, GL_SPOT_DIRECTION, spotDirrection ); //スポットライトの向ける方向（デフォルト (0,0,-1.0)）
  glLightf( light, GL_SPOT_CUTOFF,  20.0 );// スポットライトの絞り（デフォルト 180.0）
  glLightf( light, GL_SPOT_EXPONENT, 0 );// スポットライトの中心からの減衰の度合い（デフォルト 0）
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
  /*setLightPos(-5, 4.0, -5.0);//光源の位置
  setLightDif(0.6,  0.4, 0.1); //拡散光
  setLightDir(0, -1.0, 0.3);//スポットライトを向ける方向
  regLight(GL_LIGHT1);*/

  /*setLightPos(-10, 4.0, -1.0);//光源の位置
  setLightDif(0.6,  0.4, 0.1); //拡散光
  setLightDir(0, -1.0, -0.3);//スポットライトを向ける方向
  regLight(GL_LIGHT2);

  setLightPos(-19, 4.0, -10.0);//光源の位置
  setLightDif(0.6,  0.4, 0.1); //拡散光
  setLightDir(0.3, -1.0, 0);//スポットライトを向ける方向
  regLight(GL_LIGHT3);

  setLightPos(-8.0, 30.0, -10.0);//光源の位置
  setLightDif(0.0,  0.0, 0.2); //拡散光
  setLightDir(0, -1.0, 0);//スポットライトを向ける方向
  glLightf( GL_LIGHT4, GL_SPOT_CUTOFF,  30.0 );// スポットライトの絞り（デフォルト 180.0
  regLight(GL_LIGHT4);

  setLightPos(-19, 4.0, -2.0);//光源の位置
  setLightDif(0.6,  0.4, 0.1); //拡散光
  setLightDir(0.3, -1.0, 0);//スポットライトを向ける方向
  regLight(GL_LIGHT5);

  setLightPos(-19, 4.0, -19.0);//光源の位置
  setLightDif(0.6,  0.4, 0.1); //拡散光
  setLightDir(0.3, -1.0, 0);//スポットライトを向ける方向
  regLight(GL_LIGHT6);

  setLightPos(4, 4.0, -1.0);//光源の位置
  setLightDif(0.6,  0.4, 0.1); //拡散光
  setLightDir(0, -1.0, -0.3);//スポットライトを向ける方向
  regLight(GL_LIGHT7);*/

  setLightPos(-0.5, 5.7, 2);
  setLightDif(0.2, 0.2, 0.2);
  setLightDir(0, -1.0, 0);
  regLight(GL_LIGHT1);
}

void setFluorescentLight(){
    setFluorescentPos(-0.5, 5.7, 2); // スポットライトと同じ位置
    regFluorescent(GL_LIGHT2);
    glEnable(GL_LIGHT2);
}
