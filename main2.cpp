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

#define WINWIDTH 500
#define WINHEIGHT 500

#define TEXWIDTH  1000 // Texture Width                                
#define TEXHEIGHT 1000 // Texture Height
/*GLuint  texid_2; 
static const char texture2[] = "dora-360x480.raw";*/                        
int width = TEXWIDTH, height = TEXHEIGHT;

extern int collision;
GLuint exitN_Texture;
GLuint exit8_Texture;
GLuint dental_Texture;

#define USEALPHA 1  

static void init(void)
{
  glewInit(); // if windows

  // Load Textures
  /*GLubyte texture_buf2[TEXHEIGHT][TEXWIDTH][4];
  FILE *fp;

  if ((fp = fopen(texture2, "rb")) != NULL) {
    fread(texture_buf2, sizeof texture_buf2, 1, fp);
    fclose(fp);
  }
  else {
    perror(texture2);
  }*/

  // add new image - exitN
  cv::Mat picture = cv::imread("./exitN.png", -1);

  glGenTextures(1, &exitN_Texture);           // Generate TextureID
  glBindTexture(GL_TEXTURE_2D, exitN_Texture);

  // 安全のため行揃えを1に（RAWデータがRGBなどの場合に重要）
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, 1000, 1000, 0, GL_BGRA, GL_UNSIGNED_BYTE, picture.data);
  // テクスチャパラメータは bind 後に設定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // 端処理を安全に
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // RGBA データを mipmap としてアップロード（360x480 NPOT の場合でも gluBuild2DMipmaps で対応）
  /*gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TEXWIDTH, TEXHEIGHT,
                    GL_RGBA, GL_UNSIGNED_BYTE, texture_buf2);*/
  glBindTexture(GL_TEXTURE_2D, 0);

  // add new image - exit8
  picture = cv::imread("./exit8.png", -1);

  glGenTextures(1, &exit8_Texture);           // Generate TextureID
  glBindTexture(GL_TEXTURE_2D, exit8_Texture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, 1200, 1200, 0, GL_BGRA, GL_UNSIGNED_BYTE, picture.data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  // add new image - dental
  picture = cv::imread("./dental3.png", -1);

  glGenTextures(1, &dental_Texture);           // Generate TextureID
  glBindTexture(GL_TEXTURE_2D, dental_Texture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, 500, 700, 0, GL_BGRA, GL_UNSIGNED_BYTE, picture.data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);  

  // settings
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
  initlight();
  initCamera();
}

#define FRAMES 600
bool person = false;

static void display(void)
{
    static int frame = 0;
    double t = (double)frame / (double)FRAMES;
    if (++frame >= FRAMES) frame = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1.0, 1.0, 100.0);

    /* ModelView */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glRotated(cam.eye_y, 1, 0, 0);
    glRotated(cam.eye_x, 0, 1, 0);

    if (!person) {
        glTranslated(cam.pos_x, cam.pos_y, cam.pos_z);
    } else {
        glTranslated(cam.pos_x + sin(thetax) * 4,
                     cam.pos_y - 2,
                     cam.pos_z - cos(thetax) * 4);
    }

    setSpotlight();
    setFluorescentLight();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);

    scene(t);

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}


static void resize(int w, int h)
{
  if (w < WINWIDTH || h < WINHEIGHT) {
    if (w < WINWIDTH) w = WINWIDTH;
    if (h < WINHEIGHT) h = WINHEIGHT;
    glutReshapeWindow(w, h);
  }
  
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  gluPerspective(40.0, (double)w / (double)h, 1.0, 100.0);
}

void passive_motion(int x, int y){
  setCamera(x,y);
}

static void idle(void)
{
  glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
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
      collision = !collision;
      printf("Collision: %s\n", collision ? "ON" : "OFF");
      break;
  default:
    break;
  }
  makeWall();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(WINWIDTH, WINHEIGHT);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("exit n");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutPassiveMotionFunc(passive_motion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
  return 0;
}
