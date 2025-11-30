#ifdef _WIN32
  #include <GL/glew.h>
  #include <GL/freeglut.h>
  #include <opencv4/opencv2/opencv.hpp>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #  define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include "camera.h"
#include "scene.h"
#include "light.h"

static const GLfloat lightpos[] = { 35.0, 35.0, 35.0, 1.0 }; /* ���֡������������� */
static const GLfloat lightcol[] = { -0.1, -0.1, -0.1, 1.0 }; /* ľ�ܸ����١������� */
static const GLfloat lightdim[] = { -0.5, -0.5, -0.5, 1.0 }; /* ����γȻ�ȿ�Ͷ��� */
static const GLfloat lightblk[] = { 0.0, 0.0, 0.0, 1.0 }; /* ����ζ���ȿ�Ͷ��� */
static const GLfloat lightamb[] = { 0.1, 0.1, 0.1, 1.0 }; /* �Ķ������١������� */
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };


#define TEXWIDTH  512                                     /* �ƥ�������������� */
#define TEXHEIGHT 512                                     /* �ƥ�������ι⤵�� */
int width = TEXWIDTH, height = TEXHEIGHT;

#define USEALPHA 1                                        /* ����ե��ƥ��Ȼ��� */

static void init(void)
{
  glewInit(); //if windows
  /* �ƥ�������γ������ */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TEXWIDTH, TEXHEIGHT, 0,
    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  
  /* �ƥ����������硦�̾�������ˡ�λ��� */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  /* �ƥ�������η����֤���ˡ�λ��� */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  
  /* �񤭹���ݥꥴ��Υƥ��������ɸ�ͤΣҤȥƥ�������Ȥ���Ӥ�Ԥ��褦�ˤ��� */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  
  /* �⤷�Ҥ��ͤ��ƥ���������Ͱʲ��ʤ鿿�ʤĤޤ������� */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  
// #if USEALPHA
//   /* ��Ӥη�̤򥢥�ե��ͤȤ������� */
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);
  
  /* ����ե��ƥ��Ȥ���Ӵؿ��ʤ������͡� */
  glAlphaFunc(GL_GEQUAL, 0.5f);
// #else
  /* ��Ӥη�̤����ͤȤ������� */
  // glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
// #endif
  
  /* �ƥ��������ɸ�˻�����ɸ�Ϥˤ�����ʪ�Τκ�ɸ�ͤ��Ѥ��� */
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

  /* ���������ƥ��������ɸ�򤽤Τޤ� (S, T, R, Q) �˻Ȥ� */
  static const GLdouble genfunc[][4] = {
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 },
  };
  glTexGendv(GL_S, GL_EYE_PLANE, genfunc[0]);
  glTexGendv(GL_T, GL_EYE_PLANE, genfunc[1]);
  glTexGendv(GL_R, GL_EYE_PLANE, genfunc[2]);
  glTexGendv(GL_Q, GL_EYE_PLANE, genfunc[3]);

  /* ������� */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  
  /* �����ν������ */
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
  initlight(); // �饤�Ȥν����
  initCamera();

  // glActiveTexture(GL_TEXTURE1);
  // textureInit();
  // glActiveTexture(GL_TEXTURE0);
}


/****************************
** GLUT �Υ�����Хå��ؿ� **
****************************/

/* ������������ؿ������ */

/* ���˥᡼�����Υ������� */
#define FRAMES 600
bool person = false;

static void display(void)
{
  // printf("display\n");
  GLint viewport[4];       /* �ӥ塼�ݡ��Ȥ���¸�ѡ������� */
  GLdouble modelview[16];  /* ��ǥ�ӥ塼�Ѵ��������¸�� */
  GLdouble projection[16]; /* Ʃ���Ѵ��������¸�ѡ������� */
  static int frame = 0;    /* �ե졼����Υ�����ȡ������� */
  double t = (double)frame / (double)FRAMES; /* �в���֡� */

  if (++frame >= FRAMES) frame = 0;

  /*
  ** �裱���ƥåס��ǥץ��ƥ�������κ���
  */
  
  /* �ǥץ��Хåե��򥯥ꥢ���� */
  glClear(GL_DEPTH_BUFFER_BIT);
  
  /* ���ߤΥӥ塼�ݡ��Ȥ���¸���Ƥ��� */
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  /* �ӥ塼�ݡ��Ȥ�ƥ�������Υ����������ꤹ�� */
  glViewport(0, 0, TEXWIDTH, TEXHEIGHT);
  
  /* ���ߤ�Ʃ���Ѵ��������¸���Ƥ��� */
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  
  /* Ʃ���Ѵ������ñ�̹�������ꤹ�� */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  /* �������֤�����Ȥ������󤬻���˼��ޤ�褦��ǥ�ӥ塼�Ѵ���������ꤹ�� */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluPerspective(40.0, (GLdouble)TEXWIDTH / (GLdouble)TEXHEIGHT, 1.0, 100.0);
  gluLookAt(lightpos[0], lightpos[1], lightpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  /* ���ꤷ����ǥ�ӥ塼�Ѵ��������¸���Ƥ��� */
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

  /* �ǥץ��Хåե������Ƥ������������Τǥե졼��Хåե��ˤϽ񤭹��ޤʤ� */
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  /* �������äƱ����դ������פʤΤǥ饤�ƥ��󥰤򥪥դˤ��� */
  glDisable(GL_LIGHTING);

  /* �ǥץ��Хåե��ˤ����̤Υݥꥴ��α��Ԥ���Ͽ����褦�ˤ��� */
  glCullFace(GL_FRONT);

  /* ����������褹�� */
  scene(t);

  /* �ǥץ��Хåե������Ƥ�ƥ�����������ž������ */
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, TEXWIDTH, TEXHEIGHT);

  /* �̾�������������᤹ */
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projection);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_LIGHTING);
  glCullFace(GL_BACK);
  
  /*
  ** �裲���ƥåס����Τ�����
  */
  
  /* �ե졼��Хåե��ȥǥץ��Хåե��򥯥ꥢ���� */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* ��ǥ�ӥ塼�Ѵ���������� */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /* �����ΰ��֤����ꤹ���ʪ�Τ�������˰�ư�����*/
  glRotated(cam.eye_y,1,0,0);
  glRotated(cam.eye_x,0,1,0);
  if(person == false){
    glTranslated(cam.pos_x, cam.pos_y, cam.pos_z);
  }else{
    glTranslated(cam.pos_x+sin(thetax)*4, cam.pos_y-2, cam.pos_z-cos(thetax)*4);
  }
  
  /* �����ΰ��֤����ꤹ�� */
  //glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

  setSpotlight();
  setFluorescentLight();


#if USEALPHA
  /* ���������뤵��Ƥ���ʬ�Ǥ����뤵������ */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdim);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightblk);
  
  /* ����������褹�� */
  scene(t);
#endif

  /*
  ** �裳���ƥåס���������ʬ�����褹��
  */

  /* �ƥ��������Ѵ���������ꤹ�� */
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  
  /* �ƥ��������ɸ�� [-1,1] ���ϰϤ� [0,1] ���ϰϤ˼���� */
  glTranslated(0.5, 0.5, 0.5);
  glScaled(0.5, 0.5, 0.5);
  
  /* �ƥ�������Υ�ǥ�ӥ塼�Ѵ������Ʃ���Ѵ�������Ѥ򤫤��� */
  glMultMatrixd(modelview);
  
  if(person == false){
    glTranslated(-cam.pos_x, -cam.pos_y, -cam.pos_z);
  }else{
    glTranslated(-cam.pos_x-sin(thetax)*4, -cam.pos_y+2, -cam.pos_z+cos(thetax)*4);
  }
  glRotated(-cam.eye_x,0,1,0);
  glRotated(-cam.eye_y,1,0,0);
  // gluLookAt(0, 0, -10, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  
  /* ��ǥ�ӥ塼�Ѵ�������᤹ */
  glMatrixMode(GL_MODELVIEW);
  
  /* �ƥ�������ޥåԥ󥰤ȥƥ��������ɸ�μ�ư������ͭ���ˤ��� */
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_Q);
  
#if USEALPHA
  /* ����ե��ƥ��Ȥ�ͭ���ˤ��ƱƤ���ʬ���������褹�� */
  glEnable(GL_ALPHA_TEST);
  
  /* ��������ʬ����Ȥο޷��˽Ťͤ��������褦�˱��Ԥ�����Ӵؿ����ѹ����� */
  glDepthFunc(GL_LEQUAL);
#endif
  
  /* ���������뤵����������ʬ�Ǥ����뤵������ */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);
  
  /* ����������褹�� */
  scene(t);
    glEnable(GL_TEXTURE_2D);
  
#if USEALPHA
  /* ���Ԥ�����Ӵؿ��򸵤��᤹ */
  glDepthFunc(GL_LESS);
  
  /* ����ե��ƥ��Ȥ�̵���ˤ��� */
  glDisable(GL_ALPHA_TEST);
#endif
  
  /* �ƥ�������ޥåԥ󥰤ȥƥ��������ɸ�μ�ư������̵���ˤ��� */
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_Q);
  glDisable(GL_TEXTURE_2D);
  
  // glBegin(GL_QUADS);
  //   glNormal3d(0,0,8);
  //   glNormal3d(1,0,8);
  //   glNormal3d(1,1,8);
  //   glNormal3d(0,1,8);
  // glEnd();
  
  /* ���֥�Хåե���� */
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  /* ������ɥ��������ν̾������¤��� */
  if (w < TEXWIDTH || h < TEXHEIGHT) {
    if (w < TEXWIDTH) w = TEXWIDTH;
    if (h < TEXHEIGHT) h = TEXHEIGHT;
    glutReshapeWindow(w, h);
  }
  
  /* ������ɥ����Τ�ӥ塼�ݡ��Ȥˤ��� */
  glViewport(0, 0, w, h);
  
  /* Ʃ���Ѵ�����λ��� */
  glMatrixMode(GL_PROJECTION);
  
  /* Ʃ���Ѵ�����ν���� */
  glLoadIdentity();
  gluPerspective(40.0, (double)w / (double)h, 1.0, 100.0);
}

void passive_motion(int x, int y){
  // printf("passive_motion\n");
  setCamera(x,y);
}

static void idle(void)
{
  /* ���̤������ؤ� */
  glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    /* ESC �� q �� Q �򥿥��פ����齪λ */
    exit(0);
  
  case 'w': moveForward(); break;
  case 's': moveBack(); break;
  case 'a': moveLeft(); break;
  case 'd': moveRight(); break;
  case 'r': moveUp(); break;
  case 'f': moveDown(); break;
  case 'g': glutIdleFunc(idle); break;
  case 'b': glutIdleFunc(0); break;
  case 'e': initCamera(); break;
  case 'c': person = !person; break;
  default:
    break;
  }
  makeWall();
}

/*
** �ᥤ��ץ������
*/
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(TEXWIDTH, TEXHEIGHT);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("night house");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutPassiveMotionFunc(passive_motion); // �ɥ�å���θƤӽФ�
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
  return 0;
}
