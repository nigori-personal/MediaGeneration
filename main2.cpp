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

static const GLfloat lightpos[] = { 35.0, 35.0, 35.0, 1.0 };
static const GLfloat lightcol[] = { -0.1, -0.1, -0.1, 1.0 };
static const GLfloat lightdim[] = { -0.5, -0.5, -0.5, 1.0 };
static const GLfloat lightblk[] = { 0.0, 0.0, 0.0, 1.0 };
static const GLfloat lightamb[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };


#define TEXWIDTH  360 // Texture Width                                
#define TEXHEIGHT 480 // Texture Height
GLuint  texid_2; 
static const char texture2[] = "dora-360x480.raw";                        
int width = TEXWIDTH, height = TEXHEIGHT;

extern int collision;

#define USEALPHA 1  

static void init(void)
{
  // glewInit(); // if windows

  // Load Textures
  GLubyte texture_buf2[TEXHEIGHT][TEXWIDTH][4];
  FILE *fp;

  if ((fp = fopen(texture2, "rb")) != NULL) {
    fread(texture_buf2, sizeof texture_buf2, 1, fp);
    fclose(fp);
  }
  else {
    perror(texture2);
  }

  glGenTextures(1, &texid_2);           // Generate TextureID
  glBindTexture(GL_TEXTURE_2D, texid_2);

  // 安全のため行揃えを1に（RAWデータがRGBなどの場合に重要）
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // テクスチャパラメータは bind 後に設定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 端処理を安全に
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // RGBA データを mipmap としてアップロード（360x480 NPOT の場合でも gluBuild2DMipmaps で対応）
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TEXWIDTH, TEXHEIGHT,
                    GL_RGBA, GL_UNSIGNED_BYTE, texture_buf2);

  // アルファを使うならブレンド／アルファテストの設定（必要に応じて）
#if USEALPHA
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
#else
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
#endif

  // ここで色用テクスチャは完成。別に深度テクスチャを作るなら別IDを使うこと。
  glBindTexture(GL_TEXTURE_2D, 0);

  /* --- 以下はレンダリング初期設定 --- */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
  initlight();
  initCamera();
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
  //glMatrixMode(GL_TEXTURE);
  //glLoadIdentity();
  
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

  case 't':
      collision = !collision; // ←当たり判定ON/OFF切り替え
      printf("Collision: %s\n", collision ? "ON" : "OFF");
      break;
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
  glutCreateWindow("exit n");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutPassiveMotionFunc(passive_motion); // �ɥ�å���θƤӽФ�
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
  return 0;
}
