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

void GLView::drawUnit(const Evolve::Unit &unit, float x, float y, bool ghost) {}

void GLView::drawData() {}

void GLView::drawStatic() {
/*start setup*/
  glMatrixMode(GL_PROJECTION);
  // save previous matrix which contains the
  //settings for the perspective projection
  glPushMatrix();
  glLoadIdentity();

  int ww= glutGet(GLUT_WINDOW_WIDTH);
  int wh= glutGet(GLUT_WINDOW_HEIGHT);
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
  /*end setup*/

  //begin things that we actually want to draw staticly
  if(live_paused) RenderString(10, 20, GLUT_BITMAP_HELVETICA_12, "Paused", 0.5f, 0.5f, 0.5f);
  if(live_follow!=0) {
    if(world->getSelectedUnit()>=0) RenderString(10, 40, GLUT_BITMAP_HELVETICA_12, "Following", 0.5f, 0.5f, 0.5f);
    else RenderString(10, 40, GLUT_BITMAP_HELVETICA_12, "No Follow Target", 1.0f, 0.5f, 0.5f);
  }
  if(live_selection==Select::RELATIVE) RenderString(10, 60, GLUT_BITMAP_HELVETICA_12, "Relative Autoselect Mode", 0.5f, 0.8f, 0.5f);
  if(world->isClosed()) RenderString(10, 80, GLUT_BITMAP_HELVETICA_12, "Closed World", 0.5f, 0.5f, 0.5f);
  if(world->isDebug()) {
    sprintf(buf, "Plant-Haz Supp: %i agents", (int)(world->getFoodSupp()-world->getHazardSupp()));
    RenderString(5, 140, GLUT_BITMAP_HELVETICA_12, buf, 0.5f, 0.5f, 1.0f);
    sprintf(buf, "Fruit-Haz Supp: %i agents", (int)(world->getFruitSupp()-world->getHazardSupp()));
    RenderString(5, 160, GLUT_BITMAP_HELVETICA_12, buf, 0.5f, 0.5f, 1.0f);
    sprintf(buf, "Meat-Haz Supp: %i agents", (int)(world->getMeatSupp()-world->getHazardSupp()));
    RenderString(5, 180, GLUT_BITMAP_HELVETICA_12, buf, 0.5f, 0.5f, 1.0f);
    sprintf(buf, "-Haz 'Supp': %i agents", (int)(-world->getHazardSupp()));
    RenderString(5, 200, GLUT_BITMAP_HELVETICA_12, buf, 0.5f, 0.5f, 1.0f);
    sprintf(buf, "modcounter: %i", world->modcounter);
    RenderString(5, 240, GLUT_BITMAP_HELVETICA_12, buf, 0.5f, 0.5f, 1.0f);
    sprintf(buf, "GL modcounter: %i", modcounter);
    RenderString(5, 260, GLUT_BITMAP_HELVETICA_12, buf, 0.5f, 0.5f, 1.0f);
  }

  //center axis markers
  glBegin(GL_LINES);
  glColor4f(0,1,0,0.3); //green y-axis
  glVertex3f(ww/2,wh/2,0);
  glVertex3f(ww/2,wh/2+15,0);

  glColor4f(1,0,0,0.3); //red x-axis
  glVertex3f(ww/2,wh/2,0);
  glVertex3f(ww/2+15,wh/2,0);
  glEnd();

  //event log display
  float ss= 18;
  float movezero= 0;
  int fadetime= conf::EVENTS_HALFLIFE-20;

  int count= world->events.size();
  if(conf::EVENTS_DISP<count) count= conf::EVENTS_DISP;

  for(int io= 0; io<count; io++){
    int counter= world->events[io].second;

    float fade= cap((-abs((float)counter)+conf::EVENTS_HALFLIFE)/20);

    float move= 0;


    if(counter>fadetime) move= powf(((float)counter-fadetime)/7.4,3);
    else if (counter<-fadetime) move= powf(((float)counter+fadetime)/7.4,3);

    if(io==0){
      movezero= move;
      //move= 0;
    }
    glBegin(GL_QUADS);
    glColor4f(0.8,0.8,0.8,0.5*fade);
    glVertex3f(ww-202, 145+2.5*ss+io*ss+movezero+move-0.5*ss,0);
    glVertex3f(ww-2, 145+2.5*ss+io*ss+movezero+move-0.5*ss,0);
    glVertex3f(ww-2, 145+2.5*ss+io*ss+movezero+move+0.5*ss,0);
    glVertex3f(ww-202, 145+2.5*ss+io*ss+movezero+move+0.5*ss,0);
    glEnd();

    RenderString(ww-200, 150+2.5*ss+io*ss+movezero+move, GLUT_BITMAP_HELVETICA_12, world->events[io].first, 0.0f, 0.0f, 0.0f, fade);
  }

  if(world->getSelectedUnit()>=0){
    Evolve::Unit selected= world->units[world->getSelectedUnit()];
    glBegin(GL_QUADS);
    glColor4f(0,0.4,0.5,0.55);
    glVertex3f(ww-10,10,0);
    glVertex3f(ww-10,150,0);
    glVertex3f(ww-400,150,0);
    glVertex3f(ww-400,10,0);

    //stomach indicators
    glColor4f(0,0,0,0.7);
    glVertex3f(ww-101,15,0);
    glVertex3f(ww-101,26,0);
    glVertex3f(ww-16,26,0);
    glVertex3f(ww-16,15,0);

    glColor3f(0,0.6,0);
    glVertex3f(ww-100,16,0);
    glVertex3f(ww-100,19,0);
    glVertex3f(selected.stomach[Stomach::PLANT]*83+(ww-100),19,0);
    glVertex3f(selected.stomach[Stomach::PLANT]*83+(ww-100),16,0);

    glColor3f(0.6,0.6,0);
    glVertex3f(ww-100,19,0);
    glVertex3f(ww-100,22,0);
    glVertex3f(selected.stomach[Stomach::FRUIT]*83+(ww-100),22,0);
    glVertex3f(selected.stomach[Stomach::FRUIT]*83+(ww-100),19,0);

    glColor3f(0.6,0,0);
    glVertex3f(ww-100,22,0);
    glVertex3f(ww-100,25,0);
    glVertex3f(selected.stomach[Stomach::MEAT]*83+(ww-100),25,0);
    glVertex3f(selected.stomach[Stomach::MEAT]*83+(ww-100),22,0);

    //repcounter indicator
    glColor4f(0,0,0,0.7);
    glVertex3f(ww-201,15,0);
    glVertex3f(ww-201,26,0);
    glVertex3f(ww-106,26,0);
    glVertex3f(ww-106,15,0);
    glColor3f(0,0.7,0.7);
    glVertex3f(ww-200,16,0);
    glVertex3f(ww-200,25,0);
    glVertex3f(cap(selected.repcounter/world->REPRATE)*-93+(ww-107),25,0);
    glVertex3f(cap(selected.repcounter/world->REPRATE)*-93+(ww-107),16,0);

    //health indicator
    glColor4f(0,0,0,0.7);
    glVertex3f(ww-301,15,0);
    glVertex3f(ww-301,26,0);
    glVertex3f(ww-206,26,0);
    glVertex3f(ww-206,15,0);
    glColor3f(0,0.8,0);
    glVertex3f(ww-300,16,0);
    glVertex3f(ww-300,25,0);
    glVertex3f(selected.health/2.0*93+(ww-300),25,0);
    glVertex3f(selected.health/2.0*93+(ww-300),16,0);
    glEnd();

    //draw Ghost Agent
//    drawAgent(selected, ww-350, 80, true);

    //write text and values
    //Agent ID
    sprintf(buf, "ID: %d", selected.id);
    RenderString(ww-380,
                 25, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Health
    sprintf(buf, "Health: %.2f/2", selected.health);
    RenderString(ww-300,
                 25, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Repcounter
    sprintf(buf, "Child: %.2f/%.0f", selected.repcounter, world->REPRATE);
    RenderString(ww-200,
                 25, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Stomach
//		sprintf(buf, "H%.1f F%.1f C%.1f", selected.stomach[Stomach::PLANT], selected.stomach[Stomach::FRUIT],
//			selected.stomach[Stomach::MEAT]);
//		RenderString(ww-100,
//					 25, GLUT_BITMAP_HELVETICA_12,
//					 buf, 0.8f, 1.0f, 1.0f);

    if(selected.isHerbivore()) sprintf(buf, "\"Herbivore\"");
    else if(selected.isFrugivore()) sprintf(buf, "\"Frugivore\"");
    else if(selected.isCarnivore()) sprintf(buf, "\"Carnivore\"");
    else sprintf(buf, "\"Dead\"...");
    RenderString(ww-100,
                 40, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //age
    sprintf(buf, "Age: %d", selected.age);
    RenderString(ww-300,
                 40, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Generation
    sprintf(buf, "Gen: %d", selected.gencount);
    RenderString(ww-200,
                 40, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Temperature Preference
    if(selected.temperature_preference<0.3) sprintf(buf, "Heat-loving(%.3f)", selected.temperature_preference);
    else if (selected.temperature_preference>0.7) sprintf(buf, "Cold-loving(%.3f)", selected.temperature_preference);
    else sprintf(buf, "Temperate(%.3f)", selected.temperature_preference);
    RenderString(ww-300,
                 55, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Lung-type
    if(selected.lungs<0.3) sprintf(buf, "Aquatic(%.3f)", selected.lungs);
    else if (selected.lungs>0.7) sprintf(buf, "Terrestrial(%.3f)", selected.lungs);
    else sprintf(buf, "Amphibious(%.3f)", selected.lungs);
    RenderString(ww-200,
                 55, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Radius
    sprintf(buf, "Radius: %.2f", selected.radius);
    RenderString(ww-100,
                 55, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);


    //Mutrates
    sprintf(buf, "Mutrate1: %.3f", selected.MUTRATE1);
    RenderString(ww-300,
                 70, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    sprintf(buf, "Mutrate2: %.3f", selected.MUTRATE2);
    RenderString(ww-200,
                 70, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Metabolism
    sprintf(buf, "Metab: %.2f", selected.metabolism);
    RenderString(ww-100,
                 70, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Hybrid
    if(selected.hybrid) sprintf(buf, "Hybrid");
    else sprintf(buf, "Budded");
    RenderString(ww-300,
                 85, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Giving
    if(selected.give>0.5) sprintf(buf, "Generous");
    else if(selected.give>0.01) sprintf(buf, "Autocentric");
    else sprintf(buf, "Selfish");
    RenderString(ww-200,
                 85, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Spike
    if(selected.isSpikey(world->SPIKELENGTH)){
      float mw= selected.w1>selected.w2 ? selected.w1 : selected.w2;
      if(mw<0) mw= 0;
      float val= world->SPIKEMULT*selected.spikeLength*mw;
      sprintf(buf, "Spike: %.2f h", val);
    } else sprintf(buf, "Not Spikey");
    RenderString(ww-100,
                 85, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Stats: Children
    sprintf(buf, "Children: %d", selected.children);
    RenderString(ww-300,
                 100, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Trait: Strength
    if(selected.strength>0.7) sprintf(buf, "Strong!");
    else if(selected.strength>0.3) sprintf(buf, "Not Weak");
    else sprintf(buf, "Weak!");
    RenderString(ww-200,
                 100, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Stats: Killed
    sprintf(buf, "Killed: %d", selected.killed);
    RenderString(ww-100,
                 100, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Trait: Num Babies
    sprintf(buf, "Num Babies: %d", selected.numbabies);
    RenderString(ww-300,
                 115, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Output: Sexual projection
    if(selected.sexproject) sprintf(buf, "Sexting");
    else sprintf(buf, "Not Sexting");
    RenderString(ww-200,
                 115, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Species ID (Genome)
    sprintf(buf, "Gene: %d", selected.species);
    RenderString(ww-100,
                 115, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Jumping status
    if(selected.jump<=0) sprintf(buf, "Grounded");
    else sprintf(buf, "Airborne!");
    RenderString(ww-300,
                 130, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Grab status
    if(selected.grabbing>0.5){
      if(selected.grabID==-1) sprintf(buf, "Seeking");
      else sprintf(buf, "Hold: %d", selected.grabID);
    } else sprintf(buf, "Isolated");
    RenderString(ww-200,
                 130, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

    //Stats: Biting
    if(selected.jawrend!=0) sprintf(buf, "Bite: %.2f h", abs(selected.jawPosition*world->HEALTHLOSS_JAWSNAP));
    else sprintf(buf, "Not Biting");
    RenderString(ww-100,
                 130, GLUT_BITMAP_HELVETICA_12,
                 buf, 0.8f, 1.0f, 1.0f);

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