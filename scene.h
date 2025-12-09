extern void scene(double t);
void textureInit(void);
void doorChange();
static void tile(double w, double d, int nw, int nd);
static void wall(double x, double y, double z, double width, double height, double depth, int numblack);
static void box(double x, double y, double z, double width, double height, double depth, int numblack);
extern GLuint texid_2;