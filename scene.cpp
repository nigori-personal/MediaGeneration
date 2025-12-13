#ifdef _WIN32
  #include <GL/glew.h>
  #include <GL/freeglut.h>
  #include <opencv4/opencv2/opencv.hpp>
#endif
#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "scene.h"
#include "light.h"
#include "camera.h"

#define PI 3.1415926535897932384626433832795 

GLuint m_iTexture;

static void tile(double w, double d, int nw, int nd)
{
  static const GLfloat color[][4] = {
    { 0.0, 0.0, 0.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0 }
  };
  
  int i, j;

  glNormal3d(0.0, 1.0, 0.0);
  glBegin(GL_QUADS);
  for (j = 0; j < nd; ++j) {
    GLdouble dj = d * j, djd = dj + d;

    for (i = 0; i < nw; ++i) {
      GLdouble wi = w * i, wiw = wi + w;

      // Draw black lines
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[0]);
      glVertex3d(wi,  0.0, dj);
      glVertex3d(wi,  0.0, djd);
      glVertex3d(wiw, 0.0, djd);
      glVertex3d(wiw, 0.0, dj);

      double marginW = w * 0.01;
      double marginD = d * 0.01;

      // Draw white squares
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[1]);
      glVertex3d(wi + marginW,  0.01, dj + marginD);
      glVertex3d(wi + marginW,  0.01, djd - marginD);
      glVertex3d(wiw - marginW, 0.01, djd - marginD);
      glVertex3d(wiw - marginW, 0.01, dj + marginD);
    }
  }
  glEnd();
}

static void ceiling(double w, double d, int nw, int nd)
{
    static const GLfloat color[][4] = {
        {0.0, 0.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0}
    };

    glNormal3d(0.0, -1.0, 0.0);

    glBegin(GL_QUADS);

    for (int j = 0; j < nd; ++j) {
        GLdouble dj = d * j, djd = dj + d;

        for (int i = 0; i < nw; ++i) {
            GLdouble wi = w * i, wiw = wi + w;

            // Draw black lines
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[0]);
            glVertex3d(wi,  6.0, djd);
            glVertex3d(wi,  6.0, dj);
            glVertex3d(wiw, 6.0, dj);
            glVertex3d(wiw, 6.0, djd);

            double marginW = w * 0.01;
            double marginD = d * 0.01;

            // Draw white squares
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[1]);
            glVertex3d(wi + marginW,  5.99, djd - marginD);
            glVertex3d(wi + marginW,  5.99, dj + marginD);
            glVertex3d(wiw - marginW, 5.99, dj + marginD);
            glVertex3d(wiw - marginW, 5.99, djd - marginD);
        }
    }
    glEnd();
}

static void box(double x, double y, double z, double width, double height, double depth, int numblack)
{
  const GLdouble vertex[][3] = {
    { 0.0, 0.0, 0.0 },
    {   width, 0.0, 0.0 },
    {   width,   height, 0.0 },
    { 0.0,   height, 0.0 },
    { 0.0, 0.0,   depth },
    {   width, 0.0,   depth },
    {   width,   height,   depth },
    { 0.0,   height,   depth },
  };
  
  static const GLdouble *face[][4] = {
    { vertex[0], vertex[1], vertex[2], vertex[3] },
    { vertex[1], vertex[5], vertex[6], vertex[2] },
    { vertex[5], vertex[4], vertex[7], vertex[6] },
    { vertex[4], vertex[0], vertex[3], vertex[7] },
    { vertex[4], vertex[5], vertex[1], vertex[0] },
    { vertex[3], vertex[2], vertex[6], vertex[7] },
  };
  
  static const GLdouble normal[][3] = {
    { 0.0, 0.0,-1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 },
  };
  
  static const GLfloat black[] = { 0.2, 0.0, 0.0, 1.0 };
  static const GLfloat color[] = { 0.8, 1.0, 1.0, 1.0 };
  
  int i, j;
  glPushMatrix();
  glTranslated(x,y,z);
  
  glBegin(GL_QUADS);
  for (j = 0; j < 6; j++) {
    glNormal3dv(normal[j]);
    if(j==numblack){glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);}
    else{glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);}
    for (i = 4; --i >= 0;) {
      glVertex3dv(face[j][i]);
    }
  }
  glEnd();
  glPopMatrix();
}

static void drawTileQuad(GLdouble v0[3], GLdouble v1[3], GLdouble v2[3], GLdouble v3[3],
                         const GLdouble normal[3])
{
    static const GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
    static const GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

    double margin = 0.05;  // タイル枠の太さ
    double offset = 0.01;  // 白タイルを外側に押し出す距離

    /* ---- 黒タイル（外側） ---- */
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
    glBegin(GL_QUADS);
    glVertex3dv(v0);
    glVertex3dv(v1);
    glVertex3dv(v2);
    glVertex3dv(v3);
    glEnd();

    /* ---- 白タイル（内側） ---- */
    GLdouble w0[3], w1[3], w2[3], w3[3];

    for (int i = 0; i < 3; i++) {
        w0[i] = v0[i] + (v3[i] - v0[i]) * margin + (v1[i] - v0[i]) * margin;
        w1[i] = v1[i] + (v0[i] - v1[i]) * margin + (v2[i] - v1[i]) * margin;
        w2[i] = v2[i] + (v1[i] - v2[i]) * margin + (v3[i] - v2[i]) * margin;
        w3[i] = v3[i] + (v2[i] - v3[i]) * margin + (v0[i] - v3[i]) * margin;
    }

    /* ---- 白タイルを法線方向に押し出す ---- */
    for (int i = 0; i < 3; i++) {
        w0[i] += normal[i] * offset;
        w1[i] += normal[i] * offset;
        w2[i] += normal[i] * offset;
        w3[i] += normal[i] * offset;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    glBegin(GL_QUADS);
    glVertex3dv(w0);
    glVertex3dv(w1);
    glVertex3dv(w2);
    glVertex3dv(w3);
    glEnd();
}

static void wall(double x, double y, double z,
                double width, double height, double depth,
                int numblack)
{
    glPushMatrix();
    glTranslated(x, y, z);

    double tile = 0.5;  // タイル 0.5 × 0.5

    /* 面の法線 */
    const GLdouble normals[6][3] = {
        {  0,  0, -1 }, // Z=0（前）
        {  0,  0,  1 }, // Z=depth（後）
        {  1,  0,  0 }, // X=width（右）
        { -1,  0,  0 }, // X=0（左）
        {  0, -1,  0 }, // Y=0（底）
        {  0,  1,  0 }, // Y=height（天井）
    };
    /*===========================
        Z = 0 面（前面）
    ===========================*/
    glNormal3dv(normals[0]);
    for(double iy = 0; iy < height; iy += tile){
        for(double ix = 0; ix < width; ix += tile){
            GLdouble v0[3]={ix,      iy,      0};
            GLdouble v1[3]={ix,      iy+tile, 0};
            GLdouble v2[3]={ix+tile, iy+tile, 0};
            GLdouble v3[3]={ix+tile, iy,      0};
            drawTileQuad(v0, v1, v2, v3, normals[0]);
        }
    }

    /*===========================
        Z = depth 面（背面）
    ===========================*/
    glNormal3dv(normals[1]); // {0, 0, 1}

    for(double iy = 0; iy < height; iy += tile){
        for(double ix = 0; ix < width; ix += tile){

            GLdouble v0[3]={ix,      iy,      depth};
            GLdouble v1[3]={ix+tile, iy,      depth};
            GLdouble v2[3]={ix+tile, iy+tile, depth};
            GLdouble v3[3]={ix,      iy+tile, depth};

            drawTileQuad(v0, v1, v2, v3, normals[1]);
        }
    }

    /*===========================
        X = 0 面（左）
    ===========================*/
    glNormal3dv(normals[3]);
    for(double iy = 0; iy < height; iy += tile){
        for(double iz = 0; iz < depth; iz += tile){
            GLdouble v0[3]={0, iy,      iz};
            GLdouble v1[3]={0, iy,      iz+tile};
            GLdouble v2[3]={0, iy+tile, iz+tile};
            GLdouble v3[3]={0, iy+tile, iz};
            drawTileQuad(v0, v1, v2, v3, normals[3]);
        }
    }

    /*===========================
        X = width 面（右）
    ===========================*/
    glNormal3dv(normals[2]); // {1,0,0}
    for(double iy = 0; iy < height; iy += tile){
        for(double iz = 0; iz < depth; iz += tile){
            GLdouble v0[3]={width, iy,      iz};
            GLdouble v1[3]={width, iy+tile, iz};
            GLdouble v2[3]={width, iy+tile, iz+tile};
            GLdouble v3[3]={width, iy,      iz+tile};
            drawTileQuad(v0, v1, v2, v3, normals[2]);
        }
    }

    /*===========================
        Y = 0 面（底）
    ===========================*/
    glNormal3dv(normals[4]);
    for(double ix = 0; ix < width; ix += tile){
        for(double iz = 0; iz < depth; iz += tile){
            GLdouble v0[3]={ix,      0, iz};
            GLdouble v1[3]={ix+tile, 0, iz};
            GLdouble v2[3]={ix+tile, 0, iz+tile};
            GLdouble v3[3]={ix,      0, iz+tile};
            drawTileQuad(v0, v1, v2, v3, normals[4]);
        }
    }

    /*===========================
        Y = height 面（天井）
    ===========================*/
    glNormal3dv(normals[5]);
    for(double ix = 0; ix < width; ix += tile){
        for(double iz = 0; iz < depth; iz += tile){
            GLdouble v0[3]={ix,      height, iz};
            GLdouble v1[3]={ix+tile, height, iz};
            GLdouble v2[3]={ix+tile, height, iz+tile};
            GLdouble v3[3]={ix,      height, iz+tile};
            drawTileQuad(v0, v1, v2, v3, normals[5]);
        }
    }

    glPopMatrix();
}

static void door(double x, double y, double z,
                 double width, double height, double depth, int numblack)
{
    const GLdouble vertex[][3] = {
        { 0.0, 0.0, 0.0 },
        { width, 0.0, 0.0 },
        { width, height, 0.0 },
        { 0.0, height, 0.0 },
        { 0.0, 0.0, depth },
        { width, 0.0, depth },
        { width, height, depth },
        { 0.0, height, depth },
    };
    
    static const GLdouble *face[][4] = {
        { vertex[0], vertex[1], vertex[2], vertex[3] },
        { vertex[1], vertex[5], vertex[6], vertex[2] },
        { vertex[5], vertex[4], vertex[7], vertex[6] },
        { vertex[4], vertex[0], vertex[3], vertex[7] },
        { vertex[4], vertex[5], vertex[1], vertex[0] },
        { vertex[3], vertex[2], vertex[6], vertex[7] },
    };
    
    static const GLdouble normal[][3] = {
        { 0.0, 0.0,-1.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        {-1.0, 0.0, 0.0 },
        { 0.0,-1.0, 0.0 },
        { 0.0, 1.0, 0.0 },
    };

    static const GLfloat metalAmbient[]  = { 0.30f, 0.30f, 0.35f, 1.0f };
    static const GLfloat metalDiffuse[]  = { 0.55f, 0.55f, 0.60f, 1.0f };
    static const GLfloat metalSpecular[] = { 0.80f, 0.80f, 0.85f, 1.0f };
    static const GLfloat metalShine      = 80.0f;

    static const GLfloat blackAmbient[]  = { 0.05f, 0.05f, 0.05f, 1.0f };
    static const GLfloat blackDiffuse[]  = { 0.10f, 0.10f, 0.10f, 1.0f };
    static const GLfloat blackSpecular[] = { 0.20f, 0.20f, 0.20f, 1.0f };
    static const GLfloat blackShine      = 20.0f;

    static const GLfloat knobAmbient[]  = { 0.05f, 0.05f, 0.05f, 1.0f };
    static const GLfloat knobDiffuse[]  = { 0.10f, 0.10f, 0.10f, 1.0f };
    static const GLfloat knobSpecular[] = { 0.20f, 0.20f, 0.20f, 1.0f };
    static const GLfloat knobShine      = 20.0f;


    int i, j;
    glPushMatrix();
      glTranslated(x, y, z);

      glBegin(GL_QUADS);
      for (j = 0; j < 6; j++) {

          glNormal3dv(normal[j]);

          if (j == numblack) {
              // black aspect
              glMaterialfv(GL_FRONT, GL_AMBIENT, blackAmbient);
              glMaterialfv(GL_FRONT, GL_DIFFUSE, blackDiffuse);
              glMaterialfv(GL_FRONT, GL_SPECULAR, blackSpecular);
              glMaterialf(GL_FRONT, GL_SHININESS, blackShine);
          } else {
              // metal aspect
              glMaterialfv(GL_FRONT, GL_AMBIENT, metalAmbient);
              glMaterialfv(GL_FRONT, GL_DIFFUSE, metalDiffuse);
              glMaterialfv(GL_FRONT, GL_SPECULAR, metalSpecular);
              glMaterialf(GL_FRONT, GL_SHININESS, metalShine);
          }

          for (i = 4; --i >= 0;) {
              glVertex3dv(face[j][i]);
          }
      }
      glEnd();

      GLUquadric *q = gluNewQuadric();
      gluQuadricNormals(q, GLU_SMOOTH);

      /* knob texture */
      glMaterialfv(GL_FRONT, GL_AMBIENT,  knobAmbient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE,  knobDiffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, knobSpecular);
      glMaterialf (GL_FRONT, GL_SHININESS, knobShine);

      glPushMatrix();

      /* knob location */
      glTranslated(
          width * 0.1, 
          height * 0.5, 
          depth * 0.9 
      );

      glRotated(-90.0, 0.0, 1.0, 0.0);  

      GLdouble radius = width * 0.25;
      GLdouble length = width * 0.45;

      gluCylinder(q,
                  radius/1.5,   // 底面半径
                  radius,   // 上面半径
                  length,   // 長さ
                  24,
                  4);

      glPushMatrix();
      glTranslated(0.0, 0.0, length);
      gluDisk(q,
              0.0,
              radius,
              24,
              1);
      glPopMatrix();

      glPopMatrix();
      gluDeleteQuadric(q);


    glPopMatrix();
}


void texwall_zplus(double x, double y, double z,
                   double w, double h,
                   GLuint tex)
{
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

    /* テクスチャ行列リセット */
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    /* 自動テクスチャ生成OFF */
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);

    /* テクスチャ & 透過 */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    /* +Z 方向を向く */
    glNormal3d(0.0, 0.0, 1.0);

    glPushMatrix();
    glTranslated(x, y, z);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 1.0);glVertex3d(0.0, -h/2, 0.0);
        glTexCoord2d(1.0, 1.0);glVertex3d(w, -h/2, 0.0);
        glTexCoord2d(1.0, 0.0);glVertex3d(w, h/2, 0.0);
        glTexCoord2d(0.0, 0.0);glVertex3d(0.0, h/2, 0.0);
    glEnd();

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}


void texwall_xplus(double x, double y, double z,
                   double w, double h,
                   GLuint tex)
{
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

    /* テクスチャ行列リセット */
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    /* 自動生成は完全にOFF */
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);

    /* テクスチャ & 透過 */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    /* +X 方向を向く */
    glNormal3d(-1.0, 0.0, 0.0);

    glPushMatrix();
    glTranslated(x, y, z);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);glVertex3d(0.0, h/2, 0.0);
        glTexCoord2d(1.0, 0.0);glVertex3d(0.0, h/2, w);
        glTexCoord2d(1.0, 1.0);glVertex3d(0.0, -h/2, w);
        glTexCoord2d(0.0, 1.0);glVertex3d(0.0, -h/2, 0.0);
    glEnd();

    glPopMatrix();

    /* 後始末 */
    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}





bool door1 = false;
int door1theta = 0;

bool door2 = false;
int door2theta = 0;

void scene(double t)
{
  static const GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
  static const double r = 1.5;
  double wt = 2.0 * PI * t;

  // Display ground
  glPushMatrix();
  glTranslated(-50.0, 0.0, -50.0);
  tile(1.0, 1.0, 100, 100);
  glPopMatrix();

  // Display ceiling
  glPushMatrix();
  glTranslated(-50.0, 0.0, -50.0);
  ceiling(1.0, 1.0, 100, 100);
  glPopMatrix();

  // Display walls
  wall(7, 0, 15, 0.2, 6.0, 10.0, 0);
  wall(12, 0, 10, 0.2, 6.0, 10.0, 0);
  wall(-3, 0, 15, 10.0, 6.0, 0.2, 0);
  wall(2, 0, 10, 10.0, 6.0, 0.2, 0);
  wall(-3, 0, 0, 0.2, 6.0, 15.0, 0);
  wall(2, 0, -5, 0.2, 6.0, 15.0, 0);
  wall(-18, 0, 0, 15.0, 6.0, 0.2, 0);
  wall(-13, 0, -5, 15.0, 6.0, 0.2, 0);
  wall(-18, 0, -25, 0.2, 6.0, 25.0, 0);
  wall(-13, 0, -30, 0.2, 6.0, 25.0, 0);
  wall(-28, 0, -25, 10.0, 6.0, 0.2, 0);
  wall(-23, 0, -30, 10.0, 6.0, 0.2, 0);
  wall(-28, 0, -40, 0.2, 6.0, 15.0, 0);
  wall(-23, 0, -45, 0.2, 6.0, 15.0, 0);
  wall(-38, 0, -40, 10.0, 6.0, 0.2, 0);
  wall(-33, 0, -45, 10.0, 6.0, 0.2, 0);

  // Display lights
  box(-2, 5.9, 2, 3.0, 0.1, 0.1, 0);
  box(-2, 5.9, 1.8, 3.0, 0.1, 0.1, 0);
  box(-17, 5.9, -8, 3.0, 0.1, 0.1, 0);
  box(-17, 5.9, -8.2, 3.0, 0.1, 0.1, 0);
  box(-17, 5.9, -18, 3.0, 0.1, 0.1, 0);
  box(-17, 5.9, -18.2, 3.0, 0.1, 0.1, 0);
  box(-27, 5.9, -38, 3.0, 0.1, 0.1, 0);
  box(-27, 5.9, -38.2, 3.0, 0.1, 0.1, 0);

  // Display textured wall
  double iw, ih, h, w;
  iw = 1000.0;
  ih = 1000.0;
  h = 2.0;             
  w = h * (iw / ih); 
  texwall_xplus(-2.7, 2, 1, w, h, exitN_Texture);
  texwall_xplus(-27.7, 2, -39, w, h, exitN_Texture);

  iw = 1200.0;
  ih = 1200.0;
  h = 2.0;             
  w = h * (iw / ih); 
  texwall_zplus(-1, 2, -3, w, h, exit8_Texture);

  iw = 500.0;
  ih = 700.0;
  h = 1.5;             
  w = h * (iw / ih);
  texwall_xplus(-17.7, 2, -8, w, h, dental_Texture);

  // Display doors
  door(-13.05, 0, -13, 0.3, 3.0, 2.0, 1);
  door(-13.05, 0, -19, 0.3, 3.0, 2.0, 1);
}
