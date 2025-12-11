typedef struct {
  double pos_x, pos_y, pos_z;
  double eye_x, eye_y;
}camera;

extern camera cam;

extern int width, height;

extern bool door1, door2;

extern double thetax;

extern int collision;

void initCamera(void);
void setCamera(int x, int y);
void moveForward(void);
void moveBack(void);
void moveLeft(void);
void moveRight(void);
void moveUp(void);
void moveDown(void);
void makeWall();