/* ************************************************************************** *
 *   glview.cpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#include <Utils.hpp>
#include <Settings.hpp>
#include "glview.hpp"

void gl_processNormalKeys(unsigned char key, int x, int y) {
  GLVIEW->processNormalKeys(key, x, y);
}

void gl_processSpecialKeys(int key, int x, int y) {
  GLVIEW->processSpecialKeys(key, x, y);
}

void gl_processReleasedKeys(unsigned char key, int x, int y) {
  GLVIEW->processReleasedKeys(key, x, y);
}

void gl_menu(int key) {
  GLVIEW->menu(key);
}

void gl_changeSize(int w, int h) {
  GLVIEW->changeSize(w, h);
}

void gl_handleIdle() {
  GLVIEW->handleIdle();
}

void gl_processMouse(int button, int state, int x, int y) {
  GLVIEW->processMouse(button, state, x, y);
}

void gl_processMouseActiveMotion(int x, int y) {
  GLVIEW->processMouseActiveMotion(x, y);
}

void gl_processMousePassiveMotion(int x, int y) {
  GLVIEW->processMousePassiveMotion(x, y);
}

void gl_renderScene() {
  GLVIEW->renderScene();
}

void glui_handleRW(int action) {
  GLVIEW->handleRW(action);
}

void glui_handleCloses(int action) {
  GLVIEW->handleCloses(action);
}


void
RenderString(float x, float y, void *font, const char *string, float r, float g,
             float b, float a = 1.0) {
  glColor4f(r, g, b, a);
  glRasterPos2f(x, y);
  int      len = (int) strlen(string);
  for (int i   = 0; i < len; i++)
    glutBitmapCharacter(font, string[i]);
}

void drawCircle(float x, float y, float r) {
  float    n;
  for (int k = 0; k < 17; k++) {
    n = k * (M_PI / 8);
    glVertex3f(x + r * sin(n), y + r * cos(n), 0);
  }
}

void drawQuadrant(float x, float y, float r, float a, float b) {
  glVertex3f(x, y, 0);
  float    n;
  for (int k = 0; k < (int) ((b - a) * 8 / M_PI + 1); k++) {
    n = k * (M_PI / 8) + a;
    glVertex3f(x + r * sin(n), y + r * cos(n), 0);
  }
  glVertex3f(x + r * sin(b), y + r * cos(b), 0);
  glVertex3f(x, y, 0);
}

GLView::GLView(Evolve::World *s) :
        world(world),
        modcounter(0),
        frames(0),
        live_debug(true),
        lastUpdate(0),
        mousedrag(false) {

  xtranslate = 0.0;
  ytranslate = 0.0;
  scalemult  = 0.2;
  downb[0] = 0;
  downb[1] = 0;
  downb[2] = 0;
  mousex = 0;
  mousey = 0;

}

GLView::~GLView() {}

void GLView::changeSize(int w, int h) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, w, h);
  glOrtho(0, w, h, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
}

void GLView::processMouse(int button, int state, int x, int y) {
  if (world->isDebug())
    printf("MOUSE EVENT: button=%i state=%i x=%i y=%i, scale=%f, mousedrag=%i\n",
           button, state, x, y, scalemult, mousedrag);

  if (!mousedrag &&
      state == 1) { //dont let mouse click do anything if drag flag is raised
    //have world deal with it. First translate to world coordinates though
    int wx = (int) ((x - glutGet(GLUT_WINDOW_WIDTH) / 2) / scalemult -
                    xtranslate);
    int wy = (int) ((y - glutGet(GLUT_WINDOW_HEIGHT) / 2) / scalemult -
                    ytranslate);

    if (world->processMouse(button, state, wx, wy, scalemult) &&
        live_selection != Select::RELATIVE)
      live_selection = Select::MANUAL;
  }

  mousex    = x;
  mousey    = y;
  mousedrag = false;
  downb[button] = 1 - state;
}

void GLView::processMouseActiveMotion(int x, int y) {
  if (world->isDebug())
    printf("MOUSE MOTION x=%i y=%i, %i %i %i\n", x, y, downb[0], downb[1],
           downb[2]);
  if (downb[0] == 1) {
    //left mouse button drag: pan around
    xtranslate += (x - mousex) / scalemult;
    ytranslate += (y - mousey) / scalemult;
    if (abs(x - mousex) > 6 || abs(x - mousex) > 6) live_follow = 0;
  }
  if (downb[1] == 1) {
    scalemult -= conf::ZOOM_SPEED * (y - mousey);
    if (scalemult < 0.01) scalemult = 0.01;
  }
  if (downb[2] == 1) {
    std::cout << "Right Click" << std::endl;
  }
  if (abs(mousex - x) > 4 || abs(mousey - y) > 4)
    mousedrag = true; //mouse was clearly dragged, don't select agents after
  mousex      = x;
  mousey      = y;
}

void GLView::processMousePassiveMotion(int x, int y) {
  //for mouse scrolling. [DISABLED]
/*	if(y<=30) ytranslate += 2*(30-y);
	if(y>=glutGet(GLUT_WINDOW_HEIGHT)-30) ytranslate -= 2*(y-(glutGet(GLUT_WINDOW_HEIGHT)-30));
	if(x<=30) xtranslate += 2*(30-x);
	if(x>=glutGet(GLUT_WINDOW_WIDTH)-30) xtranslate -= 2*(x-(glutGet(GLUT_WINDOW_WIDTH)-30));*/
}

void GLView::processNormalKeys(unsigned char key, int x, int y) {}

void GLView::processSpecialKeys(int key, int x, int y) {}

void GLView::processReleasedKeys(unsigned char key, int x, int y) {}

void GLView::menu(int key) {}

void GLView::menuS(int key) {}

void GLView::glCreateMenu() {
  m_id = glutCreateMenu(gl_menu); //right-click context menu
  glutAddMenuEntry("Control Selected (w,a,s,d)",
                   999); //line contains mode-specific text, see menu function above
  glutAddMenuEntry("Heal Agent (/)", '/');
  glutAddMenuEntry("Reproduce Agent (|)", '|');
  glutAddMenuEntry("Mutate Agent (~)", '~');
  glutAddMenuEntry("Delete Agent (del)", 127);
  glutAddMenuEntry("-------------------", -1);
  glutAddMenuEntry("Spawn Agents", 1001);
  glutAddMenuEntry("Spawn Herbivores", 1002);
  glutAddMenuEntry("Spawn Carnivores", 1003);
  glutAddMenuEntry("Spawn Frugivores", 1004);
  glutAddMenuEntry("Toggle Closed (c)", 'c');
  glutAddMenuEntry("Save World", 1008);
  glutAddMenuEntry("-------------------", -1);
  glutAddMenuEntry("Load World", 1009);
  glutAddMenuEntry("Enter Debug Mode", 1005);
  glutAddMenuEntry("Reset Agents", 1007);
  glutAddMenuEntry("Rewrite Config", 1006);
  glutAddMenuEntry("Reload Config", 1010);
  glutAddMenuEntry("Exit (esc)", 27);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void GLView::gluiCreateMenu() {}

void GLView::handleRW(int action) { (void) action; }

void GLView::handleCloses(int action) { (void) action; }

void GLView::trySaveWorld(bool autosave) {}

//void GLView::trySaveUnit(){}
//void GLView::tryLoadUnit() {}

void GLView::countdownEvents() {
  int count = world->events.size();
  if (8 < count) count = 8; //TODO: MAX EVENT
  for (int io = 0; io < count; io++) {
    world->events[io].second--;
  }
}

void GLView::handleIdle() {
//set proper window (we don't want to draw on nothing, now do we?!)
  if (glutGetWindow() != win1) glutSetWindow(win1);

  GLUI_Master.sync_live_all();

  //after syncing all the live vars with GLUI_Master, set the vars they represent to their proper values.
  world->setClosed(live_worldclosed);
  world->setDebug((bool) live_debug);

  modcounter++;
  if (!live_paused) world->update();

  //autosave world periodically, based on world time
  if (live_autosave == 1 &&
      world->modcounter % (world->FRAMES_PER_EPOCH) == 0) {
    trySaveWorld(true);
    world->addEvent("Auto-saved World!");
  }

  //show FPS and other stuff
  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  frames++;
  if ((currentTime - lastUpdate) >= 1000) {
    glutSetWindowTitle(buf);
    frames     = 0;
    lastUpdate = currentTime;
  }

  if (!live_fastmode) {
    if (live_skipdraw > 0) {
      if (modcounter % live_skipdraw == 0) renderScene();
    } else {
      clock_t endwait;
      float   mult = -0.005 * (live_skipdraw - 1);
      endwait = clock() + mult * CLOCKS_PER_SEC;
      while (clock() < endwait) {}
      renderScene();
    }
  } else {
    if (live_selection == Select::RELATIVE) world->setSelection(live_selection);
  }

  world->update();
  renderScene();
}

void GLView::renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 255);
  glPushMatrix();

  glTranslatef(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2,
               0.0f);
  glScalef(scalemult, scalemult, 1.0f);

  glTranslatef(xtranslate, ytranslate, 0.0f);

  world->draw(this, live_layersvis);

  glPopMatrix();
  glutSwapBuffers();
  countdownEvents();
}

void GLView::drawUnit(const Unit &unit, float x, float y, bool ghost) {}


void GLView::drawData() {}

void GLView::drawStatic() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  int ww = glutGet(GLUT_WINDOW_WIDTH);
  int wh = glutGet(GLUT_WINDOW_HEIGHT);
  // set a 2D orthographic projection
  gluOrtho2D(0, ww, 0, wh);
  // invert the y axis, down is positive
  glScalef(1, -1, 1);
  // move the origin from the bottom left corner
  // to the upper left corner
  glTranslatef(0, -wh, 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  //center axis markers
  glBegin(GL_LINES);
  glColor4f(0, 1, 0, 0.3); //green y-axis
  glVertex3f(ww / 2, wh / 2, 0);
  glVertex3f(ww / 2, wh / 2 + 15, 0);

  glColor4f(1, 0, 0, 0.3); //red x-axis
  glVertex3f(ww / 2, wh / 2, 0);
  glVertex3f(ww / 2 + 15, wh / 2, 0);
  glEnd();

  //event log display
  float ss       = 18;
  float movezero = 0;
  int   fadetime = 20;

  int count = world->events.size();

  for (int io = 0; io < count; io++) {
    int counter = world->events[io].second;

    float fade = cap((-abs((float) counter) + 1) / 20);

    float move = 0;


    if (counter > fadetime) move = powf(((float) counter - fadetime) / 7.4, 3);
    else if (counter < -fadetime)
      move = powf(((float) counter + fadetime) / 7.4, 3);

    if (io == 0) {
      movezero = move;
      //move= 0;
    }
    glBegin(GL_QUADS);
    glColor4f(0.8, 0.8, 0.8, 0.5 * fade);
    glVertex3f(ww - 202, 145 + 2.5 * ss + io * ss + movezero + move - 0.5 * ss,
               0);
    glVertex3f(ww - 2, 145 + 2.5 * ss + io * ss + movezero + move - 0.5 * ss,
               0);
    glVertex3f(ww - 2, 145 + 2.5 * ss + io * ss + movezero + move + 0.5 * ss,
               0);
    glVertex3f(ww - 202, 145 + 2.5 * ss + io * ss + movezero + move + 0.5 * ss,
               0);
    glEnd();

    RenderString(ww - 200, 150 + 2.5 * ss + io * ss + movezero + move,
                 GLUT_BITMAP_HELVETICA_12, world->events[io].first, 0.0f, 0.0f,
                 0.0f, fade);
  }

  /*start clean up*/
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  // restore previous projection matrix
  glPopMatrix();

  // get back to modelview mode
  glMatrixMode(GL_MODELVIEW);
  /*end clean up*/
}


void GLView::drawCell(int x, int y, float quantity) {}

void GLView::setWorld(Evolve::World *w) { world = w; }