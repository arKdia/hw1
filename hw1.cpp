//Modified by: Erick Toscano
//
//
//
//Author: Gordon Griesel
//cs3350 Spring 2017 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"
#include <math.h>
#include "string"

#define WINDOW_WIDTH  650
#define WINDOW_HEIGHT 360

#define MAX_PARTICLES 4000
#define GRAVITY 0.15
#define rnd() (float)rand() / (float)RAND_MAX

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
  float x, y, z;
};

struct Shape {
  float width, height;
  float radius;
  Vec center;
};

struct Particle {
  Shape s;
  Vec velocity;
};

struct Game {
 // Shape box;//array of boxes and particles will be needed!!
  Shape box[5];
  Particle particle[MAX_PARTICLES];
  int n;
  int bubbler;
  int mouse[2];
  Game() { 
    n=0;
    bubbler =0;

  }
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
  int done=0;
  srand(time(NULL));
  initXWindows();
  init_opengl();
  //declare game object
  Game game;
  game.n=0;

  //declare a box shape
  for(int i=0; i < 5; i ++){
    game.box[i].width = 80;
    game.box[i].height = 12;
    game.box[i].center.x = 110 + i * 70;
    game.box[i].center.y = 280 - i * 45;
  }
  
  //start animation
  while (!done) {
    while (XPending(dpy)) {
      XEvent e;
      XNextEvent(dpy, &e);
      check_mouse(&e, &game);
      done = check_keys(&e, &game);
    }
    movement(&game);
    render(&game);
    glXSwapBuffers(dpy, win);
  }
  cleanupXWindows();
  return 0;
}

void set_title(void)
{
  //Set the window title bar.
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "335 Lab1 - Particles");
}

void cleanupXWindows(void)
{
  //do not change
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
}

void initXWindows(void)
{
  //do not change
  GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
  dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    std::cout << "\n\tcannot connect to X server\n" << std::endl;
    exit(EXIT_FAILURE);
  }
  Window root = DefaultRootWindow(dpy);
  XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
  if (vi == NULL) {
    std::cout << "\n\tno appropriate visual found\n" << std::endl;
    exit(EXIT_FAILURE);
  } 
  Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
  XSetWindowAttributes swa;
  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
    ButtonPress | ButtonReleaseMask | PointerMotionMask |
    StructureNotifyMask | SubstructureNotifyMask;
  win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
      InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
  set_title();
  glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
  glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
  //OpenGL initialization
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  //Initialize matrices
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  glMatrixMode(GL_MODELVIEW); glLoadIdentity();
  //Set 2D mode (no perspective)
  glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
  //Set the screen background color
  glClearColor(0.1, 0.1, 0.2, 1.0);
  glEnable(GL_TEXTURE_2D);
  initialize_fonts();
}

void makeParticle(Game *game, int x, int y)
{
  if (game->n >= MAX_PARTICLES)
    return;

 // std::cout << "makeParticle() " << x << " " << y << std::endl;

  //position of particle
  Particle *p = &game->particle[game->n];
  p->s.center.x = x;
  p->s.center.y = y;
  //p->velocity.y = rnd() * 0.5 - 0.25;
  //p->velocity.x = rnd() * 0.5 - 0.25;
  p->velocity.y = rnd() * 0.5 + 1.5;
  p->velocity.x = rnd() * 0.4 - 0.25; 
  game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
  static int savex = 0;
  static int savey = 0;
  //static int n = 0;

  if (e->type == ButtonRelease) {
    return;
  }
  if (e->type == ButtonPress) {
    if (e->xbutton.button==1) {
      //Left button was pressed
      int y = WINDOW_HEIGHT - e->xbutton.y ;// ORIGINAL

      for (int i=0; i <10; i ++){
        makeParticle(game, e->xbutton.x, y);// ORIGINAL
      }
      return;
    }

    if (e->xbutton.button==3) {
      //Right button was pressed
      return;
    }
  }
  //Did the mouse move?
  if (savex != e->xbutton.x || savey != e->xbutton.y) {
    savex = e->xbutton.x;
    savey = e->xbutton.y;
    int y = WINDOW_HEIGHT - e->xbutton.y ;// ORIGINAL

    if(game->bubbler == 0){
      game->mouse[0] = savex;
      game->mouse[1] = y;
    }

    for (int i=0; i <10; i ++){
      // makeParticle(game, e->xbutton.x, y);// ORIGINAL
      makeParticle(game, game->mouse[0], game->mouse[1]);// ORIGINAL
    }
    //if (++n < 10)
    //	return;
  }
}

int check_keys(XEvent *e, Game *game)
{
  //Was there input from the keyboard?
  if (e->type == KeyPress) {

    int key = XLookupKeysym(&e->xkey, 0);
    if (key == XK_Escape) {
      return 1;
    }

    if (key == XK_b) {
      game->bubbler ^=1;



    }
    //You may check other keys here.



  }
  return 0;
}

void movement(Game *game)
{
  Particle *p;
  int p_dist,
      d0,
      d1;

  if (game->n <= 0)
    return;

  if(game->bubbler != 0){
    //the bubbler is on
    for (int i=0; i < 4; i ++)
      makeParticle(game, game->mouse[0], game->mouse[1]);
  }
  for(int i=0; i<game->n; i++){
    p = &game->particle[i];
    p->velocity.y -= GRAVITY;
    p->velocity.x += 0.003;
    p->s.center.x += p->velocity.x * 4 ;
    p->s.center.y += p->velocity.y - 1.5;



    //check for collision with shapes...
    Shape *s;
    //s = &game->box;
    for (int k =0;k < 5; k++){
      s = &game->box[k];

      d0 = p->s.center.x-(WINDOW_WIDTH-80);
      d1 = p->s.center.y+30;
      p_dist = sqrt(d0*d0+d1*d1);


      if(p->s.center.y <  s->center.y + s->height &&
          p->s.center.x >= s->center.x - s->width &&
          p->s.center.x <= s->center.x + s->width &&
          p->s.center.y > s->center.y - s->height ){

        p->s.center.y = s->center.y + s->height;
        p->velocity.y = -p->velocity.y + rnd() * 0.8 - 0.25;

        //p->velocity.y = rnd() * 0.02;
        //p->velocity.x = rnd() * 0.4; 

      }
      if (p_dist <= 110){
        p->s.center.y = (-30) + 110;
        p->velocity.y = -p->velocity.y;
      }

    }

    //check for off-screen
    if (p->s.center.y < 0.0) {
    //  std::cout << "off screen" << std::endl;
      game->particle[i] = game->particle[--game->n];

    }
  }
}

void render(Game *game)
{
  float w, h;
  int rndB;

  GLfloat x = WINDOW_WIDTH-80,
          y = -30,
          radius = 110,
          twicePI = 2.0f * M_PI;
  int triangles = 100;

//===========+>
//
//
  Rect r;
  glClear(GL_COLOR_BUFFER_BIT);
  r.bot = WINDOW_HEIGHT-30;
  r.left = 20;
  r.center = 0;
  ggprint8b(&r, 16, 0x00ffffff, "Waterfall");
//
//
//===========+>

  //Draw shapes...

  //draw box
  Shape *s;
  char txt[5][20] = {"Requirements", "Design", "Coding", "Testing", "Maintenance"};
  //s = &game->box;
  for(int j=0; j<5; j++){
    glColor3ub(90,140,90);
    s = &game->box[j];
    glPushMatrix();


    glTranslatef(s->center.x, s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();
    r.bot = 272 - j * 45;
    r.left = 110 + j * 70;
    r.center = 1;
    ggprint13(&r, 16, 0x00ffffff, txt[j]);
    glColor3ub(90,140,90);
  }

  //Draw circle
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(x, y); // center

  for(int i=0; i < triangles; i++){
    glVertex2f(x + (radius * cos( i * twicePI / triangles )),
               y + (radius * sin( i * twicePI / triangles )));
  }
  glEnd();

  //draw all particles here
  for(int i=0; i<game->n; i++){
    rndB = rand() % (255 - 130 + 1) + 190;
    glPushMatrix();
    //glColor3ub(150,160,220); //ORIGINAL
    glColor3ub(57,57,rndB); //blue
    Vec *c = &game->particle[i].s.center;
    w = 3;
    h = 3;
    glBegin(GL_QUADS);
    glVertex2i(c->x-w, c->y-h);
    glVertex2i(c->x-w, c->y+h);
    glVertex2i(c->x+w, c->y+h);
    glVertex2i(c->x+w, c->y-h);
    glEnd();
    glPopMatrix();
  }
}



