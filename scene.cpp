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

      // Draw brack lines
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

/*static void box(double x, double y, double z, double width, double height, double depth, int numblack)
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
  static const GLfloat color[] = { 0.7, 0.3, 0.2, 1.0 };
  
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
}*/

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

static void box(double x, double y, double z,
                double width, double height, double depth,
                int numblack)
{
    glPushMatrix();
    glTranslated(x, y, z);

    double tile = 0.5;  // タイル 0.5 × 0.5

    /* 面の法線 */
    const GLdouble normals[6][3] = {
        {  0,  0, -1 }, // 前
        {  1,  0,  1 }, // 後 ★ 下で正しく指定（typo無し）
        {  1,  0,  0 }, // 右
        { -1,  0,  0 }, // 左
        {  0, -1,  0 }, // 底
        {  0,  1,  0 }, // 天井
    };

    /*===========================
        Z = 0 面（前面）
    ===========================*/
    glNormal3dv(normals[0]);
    for(double iy = 0; iy < height; iy += tile){
        for(double ix = 0; ix < width; ix += tile){
            GLdouble v0[3]={ix,      iy,      0};
            GLdouble v1[3]={ix+tile, iy,      0};
            GLdouble v2[3]={ix+tile, iy+tile, 0};
            GLdouble v3[3]={ix,      iy+tile, 0};
            drawTileQuad(v0, v1, v2, v3, normals[0]);
        }
    }

    /*===========================
        Z = depth 面（背面）
    ===========================*/
    glNormal3dv(normals[1]);
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
    glNormal3dv(normals[2]);
    for(double iy = 0; iy < height; iy += tile){
        for(double iz = 0; iz < depth; iz += tile){
            GLdouble v0[3]={width, iy,      iz};
            GLdouble v1[3]={width, iy,      iz+tile};
            GLdouble v2[3]={width, iy+tile, iz+tile};
            GLdouble v3[3]={width, iy+tile, iz};
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

static void doorbox(double x, double y, double z, double width, double height, double depth, int numblack)
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
  
  static const GLfloat black[] = { 0.1, 0.0, 0.0, 1.0 };
  static const GLfloat color[] = { 0.3, 0.3, 0.2, 1.0 };
  
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

void trap(double x,double y,double z){
  glPushMatrix();
  glTranslated(x+1,y,z+1);
  for(int i=0;i<=4;i+=2){
    for(int j=0;j<=4;j+=2){
      glPushMatrix();
        glTranslated(i,0,j);
        glRotated(-90,1,0,0);
        glutSolidCone(1,4,15,15);
      glPopMatrix();
    }
  }
  glPopMatrix();
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
  
  /*if(cam.pos_z>=8 && cam.pos_z<=20 && cam.pos_x <= 20 &&  cam.pos_x >= 14 )
  trap(-20,0,-20);*/

  // Display ground
  glPushMatrix();
  glTranslated(-50.0, 0.0, -50.0);
  tile(1.0, 1.0, 100, 100);
  glPopMatrix();


  // Display walls
  box(-18, 0, 0, 15.0, 4.0, 0.2, 0);
  box(-3, 0, 0, 0.2, 4.0, 10.0, 0);
  box(2, 0, -5, 0.2, 4.0, 15.0, 0);
  box(-13, 0, -5, 15.0, 4.0, 0.2, 0);
  box(-18, 0, -25, 0.2, 4.0, 25.0, 0);
  box(-13, 0, -30, 0.2, 4.0, 25.0, 0);
  box(-28, 0, -25, 10.0, 4.0, 0.2, 0);
  box(-23, 0, -30, 10.0, 4.0, 0.2, 0);
  box(-28, 0, -40, 0.2, 4.0, 15.0, 0);
  box(-23, 0, -45, 0.2, 4.0, 15.0, 0);
  box(-38, 0, -40, 10.0, 4.0, 0.2, 0);
  box(-33, 0, -45, 10.0, 4.0, 0.2, 0);


  /*box(-17, 3, 0,2.0, 1.0, 0.2,0);
  box(-17, 1.9, 0,2.0, 0.2, 0.2,0);
  box(-16, 1, 0,0.2, 2.0, 0.2,0);
  box(-17, 0, 0,2.0, 1.0, 0.2,0);

  box(-15, 0, 0,3.0, 4.0, 0.2,0);

  box(-12, 3, 0,2.0, 1.0, 0.2,0);
  box(-12, 1.9, 0,2.0, 0.2, 0.2,0);
  box(-11, 1, 0,0.2, 2.0, 0.2,0);
  box(-12, 0, 0,2.0, 1.0, 0.2,0);

  box(-10, 0, 0,3.0, 4.0, 0.2,0);

  box(-7, 3, 0,2.0, 1.0, 0.2,0);
  box(-7, 1.9, 0,2.0, 0.2, 0.2,0);
  box(-6, 1, 0,0.2, 2.0, 0.2,0);
  box(-7, 0, 0,2.0, 1.0, 0.2,0);

  box(-5, 0, 0,4.0, 4.0, 0.2,0);

  box(-1, 3, 0,2.0, 1.0, 0.2,0);*/

  /*glPushMatrix();
  glTranslated(-1,0,0.2);
  if(door1 == false){
    door1theta++;
  }else{
    door1theta--;
  }
  if(door1theta > 0){
    door1theta=0;
  }else if(door1theta < -90){
    door1theta = -90;
  }
  if(door1theta == 0 && cam.pos_z > 0){
    setlight();
  }else{
    rmlight();
  }
  glRotated(door1theta,0,1,0);
  glTranslated(0,0,-0.2);
  doorbox(0, 0, 0,2.0, 3.0, 0.2,0);
  glPopMatrix();*/

  /*box(1, 0, 0,4.0, 4.0, 0.2,0);

  // �Ȥκ�¦��
  box(-20, 0, -20,0.2, 4.0, 20.0,-1);

  //�Ȥ�ŷ��
  box(-20, 4.0, -20,25,0.2,20.2,4);

  // �Ȥα�¦��
  box(4.8, 0, -20,0.2, 4.0, 1.0,3);

  box(4.8, 3, -19,0.2, 1.0, 2.0,3);
  box(4.8, 1.9, -19,0.2, 0.2, 2.0,3);
  box(4.8, 1, -18,0.2, 2.0, 0.2,3);
  box(4.8, 0, -19,0.2, 1.0, 2.0,3);

  box(4.8, 0, -17,0.2, 4.0, 3.0,3);

  box(4.8, 3, -14,0.2, 1.0, 2.0,3);
  box(4.8, 1.9, -14,0.2, 0.2, 2.0,3);
  box(4.8, 1, -13,0.2, 2.0, 0.2,3);
  box(4.8, 0, -14,0.2, 1.0, 2.0,3);

  box(4.8, 0, -12,0.2, 4.0, 4.0,3);

  box(4.8, 3, -8,0.2, 1.0, 2.0,3);
  box(4.8, 1.9, -8,0.2, 0.2, 2.0,3);
  box(4.8, 1, -7,0.2, 2.0, 0.2,3);
  box(4.8, 0, -8,0.2, 1.0, 2.0,3);

  box(4.8, 0, -6,0.2, 4.0, 1.0,3);

  box(4.8, 3, -5,0.2, 1.0, 2.0,3);
  box(4.8, 0, -5,0.2, 1.0, 2.0,3);
  box(4.8, 1.9, -5,0.2, 0.2, 2.0,3);
  box(4.8, 1, -4,0.2, 2.0, 0.2,3);
  box(4.8, 0, -3,0.2, 4.0, 3.0,3);

  // �Ȥα�¦
  box(-20, 0.0, -20,25,4.0,0.2,-1);*/

  /*glPushMatrix();
  glTranslated(0,0,-6);
  box(-13.8, 0, 0,3.8, 4.0, 0.2,0);
  box(-10, 3, 0,2.0, 1.0, 0.2,0);
  glPushMatrix();
  glTranslated(-10,0,0.2);
  if(door2 == false){
    door2theta++;
  }else{
    door2theta--;
  }
  if(door2theta > 0){
    door2theta=0;
  }else if(door2theta < -90){
    door2theta = -90;
  }
  glRotated(door2theta,0,1,0);
  glTranslated(0,0,-0.2);
  box(0, 0, 0,2.0, 3.0, 0.2,0);
  glPopMatrix();
  box(-8, 0, 0,13.0, 4.0, 0.2,0);
  glPopMatrix();*/

  // �Ȥ���-��¦��
  /*box(-14, 0, -20,0.2, 4.0, 14.2,3);*/
  
  /* ������� */
  // glPushMatrix();
  // glTranslated(-10,1,4);
  // glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  // glutSolidSphere(0.9, 32, 16);
  // glPopMatrix();

  // ��ʬ�α���box
  glPushMatrix();
  glTranslated(-cam.pos_x, -cam.pos_y, -cam.pos_z);
  glRotated(-cam.eye_x,0,1,0);
  glRotated(-cam.eye_y,1,0,0);
  glTranslated(0,-2,0);
  // ƹ��
  /*box(-0.3,-6,-0.3,0.6,7.5,0.6,-1);
    // Ƭ
    glPushMatrix();
    glTranslated(0,2.5,0);
    glutSolidSphere(0.5, 32, 16);
    glPopMatrix();

    // ����
    glPushMatrix();
    glTranslated(0.6,0.2,-0.3);
    glRotated(15,0,0,1);
    box(0,0,0,0.3,1.2,0.3,-1);
    glPopMatrix();

    // ����
    glPushMatrix();
    glTranslated(-0.85,0.3,-0.3);
    glRotated(-15,0,0,1);
    box(0,0,0,0.3,1.2,0.3,-1);
    glPopMatrix();*/

  glPopMatrix();
}

void textureInit(void) {
    /* �������ɤ߹����Mat�˳�Ǽ */
    cv::Mat picture = cv::imread("./dora.jpg");    // (". / test.png");

    /* �ɤ߹���������Υ���������� */
    int TEXWIDTH = picture.size().width;
    int TEXHEIGHT = picture.size().height;

    // Texture�γ������
    glGenTextures(1, &m_iTexture);   // ID����
    glBindTexture(GL_TEXTURE_2D, m_iTexture);  // ����ID�Υƥ��������activate

    /* �ƥ�����������ϥХ���ñ�̤˵ͤ���ޤ�Ƥ��� */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* �ƥ�������γ������ */
    glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXWIDTH, TEXHEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, picture.data); // mipmap�Ȥ�ʤ��ʤ���2������0,�㤦�ʤ�����٤��͡���3�����Ͽ��η�����4,5�ϥƥ�������νĲ����Ǹ�ϥǡ���

    /* �ƥ����������硦�̾�������ˡ�λ��� */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   // �ѥ�᡼����Ϣ������
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ���������礵�줿�Ȥ����䴰��ǽ
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // �������̾����줿�Ȥ����䴰��ǽ
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glBindTexture(GL_TEXTURE_2D, 0);  // texture��bind�򳰤��Ƥ����ʰ����Τ����
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

}


// glActiveTexture(GL_TEXTURE1);
  // glEnable(GL_TEXTURE_2D);
  // glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  // glBindTexture(GL_TEXTURE_2D, m_iTexture);
  // glBegin(GL_QUADS);
  //   glTexCoord2d(0.0, 0.0);
  //   glVertex3f(-100, 0, 100);
  //   glTexCoord2d(1.0, 0.0); 
  //   glVertex3f(100, 0, 100);
  //   glTexCoord2d(1.0, 1.0); 
  //   glVertex3f(100, 0, -100);
  //   glTexCoord2d(0.0, 1.0); 
  //   glVertex3f(-100, 0, -100);
  // glEnd();
  // glDisable(GL_TEXTURE_2D);
  // glActiveTexture(GL_TEXTURE0);

  // glPushMatrix();
  // glTranslated(-9.0, -2.0, -9.0);
  // textureTile(1.0, 1.0, 18, 18);
  // glPopMatrix();
  // ground();