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

GLView::GLView(Evolve::World *w) :
        _world(w),
        _modcounter(0),
        _frames(0),
        _lastUpdate(0),
        _mousedrag(false) {

  _translate = Vector2f();
  _scalemult = 0.02;
  _downb[0] = 0;
  _downb[1] = 0;
  _downb[2] = 0;
  _downb[3] = 0;
  _downb[4] = 0;
  _downb[5] = 0;
  _downb[6] = 0;
  _mouse = Vector2d();
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
  if (_world->isDebug())
    printf("MOUSE EVENT: button=%i state=%i x=%i y=%i, scale=%f, _mousedrag=%i\n",
           button, state, x, y, _scalemult, _mousedrag);

  if (!_mousedrag && state == 1) {
    int wx = (int) ((x - glutGet(GLUT_WINDOW_WIDTH) / 2) / _scalemult -
                    _translate.x);
    int wy = (int) ((y - glutGet(GLUT_WINDOW_HEIGHT) / 2) / _scalemult -
                    _translate.y);

    if (_world->processMouse(button, state, wx, wy, _scalemult) &&
        _live.selection != Select::RELATIVE)
      _live.selection = Select::MANUAL;
  }

  if (button == 3 || button == 4) {
    if (button == 3)
      _scalemult += 10 * conf::ZOOM_SPEED / 10;
    else
      _scalemult -= 10 * conf::ZOOM_SPEED / 10;
    if (_scalemult < 0.01) _scalemult = 0.01;

  }

  _mouse.x = x;
  _mouse.y = y;
  _mousedrag = false;
  _downb[button] = 1 - state;
}

void GLView::processMouseActiveMotion(int x, int y) {
  if (_world->isDebug())
    printf("MOUSE MOTION x=%i y=%i, %i %i %i\n", x, y, _downb[0], _downb[1],
           _downb[2]);
  if (_downb[0]) {
    _translate.x += (x - _mouse.x) / _scalemult;
    _translate.y += (y - _mouse.y) / _scalemult;
    if (abs(x - _mouse.x) > 6 || abs(x - _mouse.x) > 6)
      _live.follow = 0;
  }
  if (_downb[1]) {
    _scalemult -= conf::ZOOM_SPEED * (y - _mouse.y);
    if (_scalemult < 0.01) _scalemult = 0.01;
  }
  if (_downb[2]) {
    std::cout << "Right Click" << std::endl;
  }
  if (abs(_mouse.x - x) > 4 || abs(_mouse.y - y) > 4)
    _mousedrag = true;
  _mouse.x = x;
  _mouse.y = y;
}

void GLView::processMousePassiveMotion(int x, int y) {
  //for mouse scrolling. [DISABLED]
/*	if(y<=30) _translate.y += 2*(30-y);
	if(y>=glutGet(GLUT_WINDOW_HEIGHT)-30) _translate.y -= 2*(y-(glutGet(GLUT_WINDOW_HEIGHT)-30));
	if(x<=30) _translate.x += 2*(30-x);
	if(x>=glutGet(GLUT_WINDOW_WIDTH)-30) _translate.x -= 2*(x-(glutGet(GLUT_WINDOW_WIDTH)-30));*/
}

void GLView::processNormalKeys(unsigned char key, int x, int y) {
  menu(key);
}

void GLView::processSpecialKeys(int key, int x, int y) {
  menuS(key);
}

void GLView::processReleasedKeys(unsigned char key, int x, int y) {
  if (key == 32) {//spacebar input [released]
    _world->pinput1 = 0;
  }
}

void GLView::menu(int key) {
//  ReadWrite* savehelper= new ReadWrite(); //for loading/saving
  if (!key)
    return;
  if (key == 27) //[esc] quit
    exit(0);
  else if (key == 'p') { //Pause
    _live.paused = !_live.paused;
  } else if (key == 'l') { //Drawing
    _live.fastmode = !_live.fastmode;
  } else if (key == 43) { //+
    _live.skipdraw++;
  } else if (key == 45) { //-
    _live.skipdraw--;
  } else if (key == 'k' || key == 'm') { //layer switch
    if (key == 'm') _live.layersvis++;
    else _live.layersvis--;
    if (_live.layersvis > Evolve::Layer::LAYERS)
      _live.layersvis = 0;
    if (_live.layersvis < 0)
      _live.layersvis = Evolve::Layer::LAYERS;
  } else if (key == 'a' || key == 'e') { //cell switch
    if (key == 'e')
      _live.unitsvis++;
    else
      _live.unitsvis--;
    if (_live.unitsvis > Visual::VISUALS - 1) _live.unitsvis = Visual::NONE;
    if (_live.unitsvis < Visual::NONE)
      _live.unitsvis =
              Visual::VISUALS - 1;
  } else if (key == 1001) { //add units
    _world->addUnits(5);
  } else if (key == 1002) { //add Herbivore units
    _world->addUnits(5, Stomach::PLANT);
  } else if (key == 1003) { //add Carnivore units
    _world->addUnits(5, Stomach::MEAT);
  } else if (key == 1004) { //add Frugivore units
    _world->addUnits(5, Stomach::FRUIT);
  } else if (key == 'c') {
    _world->setClosed(!_world->isClosed());
    _live.worldclosed = (int) _world->isClosed();
    glutGet(GLUT_MENU_NUM_ITEMS);
    if (_world->isClosed()) glutChangeToMenuEntry(4, "Open World", 'c');
    else glutChangeToMenuEntry(4, "Close World", 'c');
    glutSetMenu(m_id);
  } else if (key == 'f') {
    _live.follow = !_live.follow; //toggle follow selected unit
  } else if (key == 'o') {
    if (_live.selection != Select::OLDEST)
      _live.selection = Select::OLDEST; //select oldest unit
    else _live.selection = Select::NONE;
  } else if (key == 'q') {
    //zoom and translocate to instantly see the whole world
    float scaleA = (float) glutGet(GLUT_WINDOW_WIDTH) / (conf::WIDTH + 2200);
    float scaleB = (float) glutGet(GLUT_WINDOW_HEIGHT) / (conf::HEIGHT + 150);
    if (scaleA > scaleB) _scalemult = scaleB;
    else _scalemult = scaleA;
    _translate.x = -(conf::WIDTH - 2020) / 2;
    _translate.y = -(conf::HEIGHT - 80) / 2;
    _live.follow = 0;
  } else if (key == 'g') {
    if (_live.selection != Select::BEST_GEN)
      _live.selection = Select::BEST_GEN; //select most advanced generation unit
    else _live.selection = Select::NONE;
  } else if (key == 'h') {
    if (_live.selection != Select::HEALTHY)
      _live.selection = Select::HEALTHY; //select healthiest
    else _live.selection = Select::NONE;
  } else if (key == 'R') {
    _world->setSelectedUnit(randi(0, _world->units.size() - 1));
    _live.selection = Select::MANUAL;
  } else if (key == 'r') {
    while (true) {
      int idx = randi(0, _world->units.size() - 1);
      if (_world->units[idx].health > 0.1) {
        _world->setSelectedUnit(idx);
        _live.selection = Select::MANUAL;
        break;
      }
    }
  } else if (key == 127) { //delete
    _world->selectedKill();
  } else if (key == 62) { //zoom+ >
    _scalemult += 10 * conf::ZOOM_SPEED;
  } else if (key == 60) { //zoom- <
    _scalemult -= 10 * conf::ZOOM_SPEED;
    if (_scalemult < 0.01) _scalemult = 0.01;
  } else if (key == 32) { //spacebar input [pressed]
    _world->pinput1 = 1;
  } else if (key == '/') { // / heal selected
    _world->selectedHeal();
  } else if (key == '|') { // | reproduce selected
    _world->selectedBabys();
  } else if (key == '~') { // ~ mutate selected
    _world->selectedMutate();
  } else if (key == 119) { //w (move faster)
    _world->pcontrol = true;
    _world->pleft    = capm(_world->pleft + 0.08, -1, 1);
    _world->pright   = capm(_world->pright + 0.08, -1, 1);
  } else if (key == 97) { //a (turn left)
    _world->pcontrol = true;
    _world->pleft    = capm(
            _world->pleft - 0.05 + (_world->pright - _world->pleft) * 0.05, -1,
            1); //this extra code helps with turning out of tight circles
    _world->pright   = capm(
            _world->pright + 0.05 + (_world->pleft - _world->pright) * 0.05, -1,
            1);
  } else if (key == 115) { //s (move slower)
    _world->pcontrol = true;
    _world->pleft    = capm(_world->pleft - 0.08, -1, 1);
    _world->pright   = capm(_world->pright - 0.08, -1, 1);
  } else if (key == 100) { //d (turn right)
    _world->pcontrol = true;
    _world->pleft    = capm(
            _world->pleft + 0.05 + (_world->pright - _world->pleft) * 0.05, -1,
            1);
    _world->pright   = capm(
            _world->pright - 0.05 + (_world->pleft - _world->pright) * 0.05, -1,
            1);
  } else if (key == 999) { //player control
    _world->setControl(!_world->pcontrol);
    glutGet(GLUT_MENU_NUM_ITEMS);
    if (_world->pcontrol) glutChangeToMenuEntry(1, "Release Unit", 999);
    else glutChangeToMenuEntry(1, "Control Selected (w,a,s,d)", 999);
    glutSetMenu(m_id);
  } else if (key == 1005) { //menu only, debug mode
    _world->setDebug(!_world->isDebug());
    _live.debug = (int) _world->isDebug();
    glutGet(GLUT_MENU_NUM_ITEMS);
    if (_world->isDebug()) {
      glutChangeToMenuEntry(18, "Exit Debug Mode", 1005);
      printf("Entered Debug Mode\n");
    } else glutChangeToMenuEntry(18, "Enter Debug Mode", 1005);
    glutSetMenu(m_id);
  } else if (key == 1006) { //force-reset config
    _world->writeConfig();
  } else if (key == 1007) { // reset
    _world->reset();
    _world->spawn();
    printf("WORLD RESET!\n");
  } else if (key == 1008) { //save world
    handleRW(2);
  } else if (key == 1009) { //load world
    handleRW(1);
  } else if (key == 1010) { //reload config
    _world->readConfig();
  } else if (_world->isDebug()) {
    printf("Unknown key pressed: %i\n", key);
  }
}

void GLView::menuS(int key) {}

void GLView::glCreateMenu() {
  m_id = glutCreateMenu(gl_menu);
  glutAddMenuEntry("Control Selected (w,a,s,d)", 999);
  glutAddMenuEntry("Heal Unit (/)", '/');
  glutAddMenuEntry("Reproduce Unit (|)", '|');
  glutAddMenuEntry("Mutate Unit (~)", '~');
  glutAddMenuEntry("Delete Unit (del)", 127);
  glutAddMenuEntry("-------------------", -1);
  glutAddMenuEntry("Spawn Units", 1001);
  glutAddMenuEntry("Spawn Herbivores", 1002);
  glutAddMenuEntry("Spawn Carnivores", 1003);
  glutAddMenuEntry("Spawn Frugivores", 1004);
  glutAddMenuEntry("Toggle Closed (c)", 'c');
  glutAddMenuEntry("Save World", 1008);
  glutAddMenuEntry("-------------------", -1);
  glutAddMenuEntry("Load World", 1009);
  glutAddMenuEntry("Enter Debug Mode", 1005);
  glutAddMenuEntry("Reset Units", 1007);
  glutAddMenuEntry("Rewrite Config", 1006);
  glutAddMenuEntry("Reload Config", 1010);
  glutAddMenuEntry("Exit (esc)", 27);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void GLView::gluiCreateMenu() {
  _live.worldclosed = 0;
  _live.paused      = 0;
  _live.fastmode    = 0;
  _live.skipdraw    = 1;
  _live.unitsvis    = Visual::RGB;
  _live.layersvis   = Evolve::Layer::LAND + 1;
  _live.profilevis  = Profile::INOUT;
  _live.selection   = Select::MANUAL;
  _live.follow      = 0;
  _live.autosave    = 0;
  _live.debug       = 0;
  _live.grid        = 0;

  _menu = GLUI_Master.create_glui("Menu", 0, 1, 1);
  _menu->add_checkbox("Closed world", &_live.worldclosed);
  _menu->add_checkbox("Pause", &_live.paused);
  _menu->add_checkbox("Allow Autosaves", &_live.autosave);

  new GLUI_Button(_menu, "Load World", 1, glui_handleRW);
  new GLUI_Button(_menu, "Save World", 2, glui_handleRW);
  new GLUI_Button(_menu, "New World", 3, glui_handleRW);

  GLUI_Panel *panel_speed = new GLUI_Panel(_menu, "Speed Control");
  _menu->add_checkbox_to_panel(panel_speed, "Fast Mode", &_live.fastmode);
  _menu->add_spinner_to_panel(panel_speed, "Speed", GLUI_SPINNER_INT,
                              &_live.skipdraw);

  GLUI_Rollout *rollout_vis = new GLUI_Rollout(_menu, "Visuals");

  GLUI_RadioGroup *group_layers = new GLUI_RadioGroup(rollout_vis,
                                                      &_live.layersvis);
  new GLUI_StaticText(rollout_vis, "Layer");
  new GLUI_RadioButton(group_layers, "off");
  for (int i = 0; i < Evolve::Layer::LAYERS; i++) {
    char text[16] = "";
    if (i == Evolve::Layer::PLANTS) strcpy(text, "Plant");
    else if (i == Evolve::Layer::MEATS) strcpy(text, "Meat");
    else if (i == Evolve::Layer::HAZARDS) strcpy(text, "Hazard");
    else if (i == Evolve::Layer::FRUITS) strcpy(text, "Fruit");
    else if (i == Evolve::Layer::LAND) strcpy(text, "Map");
    else if (i == Evolve::Layer::LIGHT) strcpy(text, "Light");
    else if (i == Evolve::Layer::TEMP) strcpy(text, "Temp");

    new GLUI_RadioButton(group_layers, text);
  }

  _menu->add_column_to_panel(rollout_vis, true);
  GLUI_RadioGroup *group_units = new GLUI_RadioGroup(rollout_vis,
                                                     &_live.unitsvis);
  new GLUI_StaticText(rollout_vis, "Agents");
  for (int i = 0; i < Visual::VISUALS; i++) {
    char text[16] = "";
    if (i == Visual::NONE) strcpy(text, "off");
    else if (i == Visual::RGB) strcpy(text, "RGB");
    else if (i == Visual::STOMACH) strcpy(text, "Stomach");
    else if (i == Visual::DISCOMFORT) strcpy(text, "Discomfort");
    else if (i == Visual::VOLUME) strcpy(text, "Volume");
    else if (i == Visual::SPECIES) strcpy(text, "Species ID");
    else if (i == Visual::CROSSABLE) strcpy(text, "Crossable");
    else if (i == Visual::HEALTH) strcpy(text, "Health");
    else if (i == Visual::METABOLISM) strcpy(text, "Metabolism");
    else if (i == Visual::LUNGS) strcpy(text, "Land/Water");


    new GLUI_RadioButton(group_units, text);
  }
  _menu->add_checkbox_to_panel(rollout_vis, "Grid on", &_live.grid);

  GLUI_Rollout    *rollout_xyl  = new GLUI_Rollout(_menu, "Selection");
  GLUI_RadioGroup *group_select = new GLUI_RadioGroup(rollout_xyl,
                                                      &_live.selection);
  for (int        i             = 0; i < Select::SELECT_TYPES; i++) {
    char text[16] = "";
    if (i == Select::OLDEST) strcpy(text, "Oldest");
    else if (i == Select::BEST_GEN) strcpy(text, "Best Gen.");
    else if (i == Select::HEALTHY) strcpy(text, "Healthiest");
    else if (i == Select::PRODUCTIVE) strcpy(text, "Productive");
    else if (i == Select::AGGRESSIVE) strcpy(text, "Aggressive");
    else if (i == Select::NONE) strcpy(text, "off");
    else if (i == Select::MANUAL) strcpy(text, "Manual");
    else if (i == Select::RELATIVE) strcpy(text, "Relative");

    new GLUI_RadioButton(group_select, text);
    if (i == Select::NONE)
      _menu->add_checkbox_to_panel(rollout_xyl, "Follow Selected",
                                   &_live.follow);
  }
  new GLUI_Separator(rollout_xyl);
  _menu->add_button_to_panel(rollout_xyl, "Save Selected", 4, glui_handleRW);

  _menu->add_checkbox("DEBUG", &_live.debug);

  _menu->set_main_gfx_window(mainWin);
}

void GLView::handleRW(int action) { (void) action; }

void GLView::handleCloses(int action) { (void) action; }

void GLView::trySaveWorld(bool autosave) {}

void GLView::trySaveUnit() {}

void GLView::tryLoadUnit() {}

void GLView::countdownEvents() {
  int count = _world->events.size();
  if (8 < count) count = 8; //TODO: MAX EVENT
  for (int io = 0; io < count; io++) {
    _world->events[io].second--;
  }
}

void GLView::handleIdle() {
  if (glutGetWindow() != mainWin)
    glutSetWindow(mainWin);
  GLUI_Master.sync_live_all();
  _world->setClosed(_live.worldclosed);
  _world->setDebug((bool) _live.debug);

  _modcounter++;
  if (!_live.paused) _world->update();

  if (_live.autosave == 1 &&
      _world->modcounter % (_world->FRAMES_PER_EPOCH) == 0) {
    trySaveWorld(true);
    _world->addEvent("Auto-saved World!");
  }

  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  _frames++;
  if ((currentTime - _lastUpdate) >= 1000) {
    sprintf(_buf,
            "FPS: %d speed: %d Alive Units: %d Herbivores: %d Carnivores: %d Frugivores: %d Dead: %d Epoch: %d",
            _frames, _live.skipdraw, _world->getUnits() - _world->getDead(),
            _world->getHerbivores(), _world->getCarnivores(),
            _world->getFrugivores(), _world->getDead(), _world->epoch());
    glutSetWindowTitle(_buf);
    _frames     = 0;
    _lastUpdate = currentTime;
  }

  if (!_live.fastmode) {
    if (_live.skipdraw > 0) {
      if (_modcounter % _live.skipdraw == 0) {
        glutSetWindow(mainWin);
        renderScene();
        glutSetWindow(profileWin);
        renderProfile();
        glutSetWindow(statsWin);
        renderStats();
      }
    } else {
      clock_t endwait;
      float   mult = -0.005 * (_live.skipdraw - 1);
      endwait = clock() + mult * CLOCKS_PER_SEC;
      while (clock() < endwait) {}
      glutSetWindow(mainWin);
      renderScene();
      glutSetWindow(profileWin);
      renderProfile();
      glutSetWindow(statsWin);
      renderStats();
    }
  } else {
    if (_live.selection == Select::RELATIVE)
      _world->setSelection(_live.selection);
  }
}

void GLView::renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.129, 0.145, 0.168, 255);
  glPushMatrix();

  glTranslatef(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2,
               0.0f);
  glScalef(_scalemult, _scalemult, 1.0f);

  _world->setSelection(_live.selection);
  if (_world->getSelection() == -1 && _live.selection != Select::MANUAL &&
      _modcounter % 5 == 0)
    _live.selection = Select::NONE;

  if (_live.follow == 1) {
    float xi = 0, yi = 0;
    _world->getFollowLocation(xi, yi);

    if (xi != 0 && yi != 0) {
      if (abs(-xi - _translate.x) > 0.95 * conf::WIDTH ||
          abs(-yi - _translate.y) > 0.95 * conf::HEIGHT) {
        _translate.x = -xi;
        _translate.y = -yi;
      } else {
        float speed = conf::SNAP_SPEED;
        if (_scalemult > 0.5) speed = cap(speed * pow(_scalemult + 0.5, 3));
        _translate.x += speed * (-xi - _translate.x);
        _translate.y += speed * (-yi - _translate.y);
      }
    }
  }
  glTranslatef(_translate.x, _translate.y, 0.0f);
  _world->draw(this, _live.layersvis);
  glPopMatrix();
  glutSwapBuffers();
  countdownEvents();
}

void GLView::renderProfile() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.129, 0.145, 0.168, 0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  int ww = glutGet(GLUT_WINDOW_WIDTH);
  int wh = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(0, ww, 0, wh);
  glScalef(1, -1, 1);
  glTranslatef(0, -wh, 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  if (_world->getSelectedUnit() >= 0) {
    Evolve::Unit selected = _world->units[_world->getSelectedUnit()];
    glBegin(GL_QUADS);
    glColor4f(0.23, 0.25, 0.3, 1);
    glVertex3f(10, 10, 0);
    glVertex3f(10, 150, 0);
    glVertex3f(160, 150, 0);
    glVertex3f(160, 10, 0);

    glBegin(GL_QUADS);
    glColor4f(0.10, 0.113, 0.137, 1);
    glVertex3f(15, 35, 0);
    glVertex3f(15, 145, 0);
    glVertex3f(155, 145, 0);
    glVertex3f(155, 35, 0);

    glBegin(GL_QUADS);
    glColor4f(0.23, 0.25, 0.3, 1);
    glVertex3f(170, 10, 0);
    glVertex3f(170, 150, 0);
    glVertex3f(ww - 10, 150, 0);
    glVertex3f(ww - 10, 10, 0);

    glBegin(GL_QUADS);
    glColor4f(0.23, 0.25, 0.3, 1);
    glVertex3f(10, 155, 0);
    glVertex3f(10, 240, 0);
    glVertex3f(ww - 10, 240, 0);
    glVertex3f(ww - 10, 155, 0);

    glBegin(GL_QUADS);
    glColor4f(1, 0.5, 0.5, 1);
    glVertex3f(10, 245, 0);
    glVertex3f(10, wh - 10, 0);
    glVertex3f(ww - 10, wh - 10, 0);
    glVertex3f(ww - 10, 245, 0);

    glColor4f(0.10, 0.113, 0.137, 1);
    glVertex3f(175, 95, 0);
    glVertex3f(175, 145, 0);
    glVertex3f(ww - 15, 145, 0);
    glVertex3f(ww - 15, 95, 0);

    glColor3f(0, 0.6, 0);
    glVertex3f(176, 96, 0);
    glVertex3f(176, 112, 0);
    glVertex3f(selected.stomach[Stomach::PLANT] * 360 + (175), 112, 0);
    glVertex3f(selected.stomach[Stomach::PLANT] * 360 + (175), 96, 0);

    glColor3f(0.6, 0.6, 0);
    glVertex3f(176, 112, 0);
    glVertex3f(176, 128, 0);
    glVertex3f(selected.stomach[Stomach::FRUIT] * 360 + (175), 128, 0);
    glVertex3f(selected.stomach[Stomach::FRUIT] * 360 + (175), 112, 0);

    glColor3f(0.6, 0, 0);
    glVertex3f(176, 128, 0);
    glVertex3f(176, 144, 0);
    glVertex3f(selected.stomach[Stomach::MEAT] * 360 + (175), 144, 0);
    glVertex3f(selected.stomach[Stomach::MEAT] * 360 + (175), 128, 0);

    glColor4f(0.10, 0.113, 0.137, 1);
    glVertex3f(175, 55, 0);
    glVertex3f(175, 90, 0);
    glVertex3f(ww - 15, 90, 0);
    glVertex3f(ww - 15, 55, 0);
    glColor3f(0, 0.7, 0.7);
    glVertex3f(176, 56, 0);
    glVertex3f(176, 89, 0);
    glVertex3f(cap(selected.repcounter / _world->REPRATE) * 360 + (175), 89, 0);
    glVertex3f(cap(selected.repcounter / _world->REPRATE) * 360 + (175), 56, 0);

    glColor4f(0.10, 0.113, 0.137, 1);
    glVertex3f(175, 15, 0);
    glVertex3f(175, 50, 0);
    glVertex3f(ww - 15, 50, 0);
    glVertex3f(ww - 15, 15, 0);
    glColor3f(0, 0.8, 0);
    glVertex3f(176, 16, 0);
    glVertex3f(176, 49, 0);
    glVertex3f(selected.health / 2.0 * 360 + (175), 49, 0);
    glVertex3f(selected.health / 2.0 * 360 + (175), 16, 0);
    glEnd();

    drawUnit(selected, 90, 90, true);

    //Id
    sprintf(_buf, "ID: %d", selected.id);
    RenderString(90 - (strlen(_buf) * 2),
                 25, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Health
    sprintf(_buf, "Health: %.2f/2", selected.health);
    RenderString(((ww - 15) + 175) / 2 - (strlen(_buf) * 2),
                 35, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Child
    sprintf(_buf, "Child: %.2f/%.0f", selected.repcounter, _world->REPRATE);
    RenderString(((ww - 15) + 175) / 2 - (strlen(_buf) * 2),
                 75, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    if (selected.isHerbivore()) sprintf(_buf, "Herbivore");
    else if (selected.isFrugivore()) sprintf(_buf, "Frugivore");
    else if (selected.isCarnivore()) sprintf(_buf, "Carnivore");
    else sprintf(_buf, "Dead");
    RenderString(((ww - 15) + 175) / 2 - (strlen(_buf) * 2),
                 125, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Age
    sprintf(_buf, "Age: %d", selected.age);
    RenderString(15,
                 170, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Generation
    sprintf(_buf, "Gen: %d", selected.gencount);
    RenderString(115,
                 170, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Temperature Preference
    if (selected.temperature_preference < 0.3)
      sprintf(_buf, "Heat-loving(%.3f)", selected.temperature_preference);
    else if (selected.temperature_preference > 0.7)
      sprintf(_buf, "Cold-loving(%.3f)", selected.temperature_preference);
    else sprintf(_buf, "Temperate(%.3f)", selected.temperature_preference);
    RenderString(215,
                 230, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Lung-type
    if (selected.lungs < 0.3) sprintf(_buf, "Aquatic(%.3f)", selected.lungs);
    else if (selected.lungs > 0.7)
      sprintf(_buf, "Terrestrial(%.3f)", selected.lungs);
    else sprintf(_buf, "Amphibious(%.3f)", selected.lungs);
    RenderString(115,
                 230, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Radius
    sprintf(_buf, "Radius: %.2f", selected.radius);
    RenderString(15,
                 230, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);


    //Mutrates
    sprintf(_buf, "Mutrate1: %.3f", selected.MUTRATE1);
    RenderString(315,
                 230, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    sprintf(_buf, "Mutrate2: %.3f", selected.MUTRATE2);
    RenderString(415,
                 230, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Metabolism
    sprintf(_buf, "Metab: %.2f", selected.metabolism);
    RenderString(415,
                 210, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Hybrid
    if (selected.hybrid) sprintf(_buf, "Hybrid");
    else sprintf(_buf, "Budded");
    RenderString(315,
                 210, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Giving
    if (selected.give > 0.5) sprintf(_buf, "Generous");
    else if (selected.give > 0.01) sprintf(_buf, "Autocentric");
    else sprintf(_buf, "Selfish");
    RenderString(215,
                 210, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Spike
    if (selected.isSpikey(_world->SPIKELENGTH)) {
      float mw = selected.w1 > selected.w2 ? selected.w1 : selected.w2;
      if (mw < 0) mw = 0;
      float val = _world->SPIKEMULT * selected.spikeLength * mw;
      sprintf(_buf, "Spike: %.2f h", val);
    } else sprintf(_buf, "Not Spikey");
    RenderString(115,
                 210, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Stats: Children
    sprintf(_buf, "Children: %d", selected.children);
    RenderString(415,
                 170, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Trait: Strength
    if (selected.strength > 0.7) sprintf(_buf, "Strong!");
    else if (selected.strength > 0.3) sprintf(_buf, "Not Weak");
    else sprintf(_buf, "Weak!");
    RenderString(115,
                 190, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Stats: Killed
    sprintf(_buf, "Killed: %d", selected.killed);
    RenderString(15,
                 190, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Trait: Num Babies
    sprintf(_buf, "Num Babies: %d", selected.numbabies);
    RenderString(315,
                 170, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Output: Sexual projection
    if (selected.sexproject) sprintf(_buf, "Sexting");
    else sprintf(_buf, "Not Sexting");
    RenderString(215,
                 190, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Species ID (Genome)
    sprintf(_buf, "Gene: %d", selected.species);
    RenderString(215,
                 170, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Jumping status
    if (selected.jump <= 0) sprintf(_buf, "Grounded");
    else sprintf(_buf, "Airborne!");
    RenderString(315,
                 190, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Grab status
    if (selected.grabbing > 0.5) {
      if (selected.grabID == -1) sprintf(_buf, "Seeking");
      else sprintf(_buf, "Hold: %d", selected.grabID);
    } else sprintf(_buf, "Isolated");
    RenderString(415,
                 190, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);

    //Stats: Biting
    if (selected.jawrend != 0)
      sprintf(_buf, "Bite: %.2f h", abs(selected.jawPosition *
                                        _world->HEALTHLOSS_JAWSNAP));
    else sprintf(_buf, "Not Biting");
    RenderString(15,
                 210, GLUT_BITMAP_HELVETICA_12,
                 _buf, 0.8f, 1.0f, 1.0f);


    glBegin(GL_QUADS);
    float    col;
    float    yy           = 15;
    float    xx           = 15;
    float    ss           = 16;
    float    offx         = 0;
    for (int j            = 0; j < Input::INPUT_SIZE; j++) {
      col = selected.in[j];
      if (j == Input::TEMP)
        glColor3f(col, (2 - col) / 2, (1 - col));
      glColor3f(col, col, col);
      glVertex3f(15, 250 + ss * j, 0.0f);
      glVertex3f(15, 250 + xx + ss * j, 0.0f);
      glVertex3f(15 + yy, 250 + xx + ss * j, 0.0f);
      glVertex3f(15 + yy, 250 + ss * j, 0.0f);
      glEnd();
      if (j == Input::CLOCK1 || j == Input::CLOCK2 || j == Input::CLOCK3) {
        RenderString(8 + yy * 2 / 3, 257 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12,
                     "Q", 0.0f, 0.0f, 0.0f);
      } else if (j == Input::TEMP) {
        RenderString(8 + yy * 2 / 3, 257 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12,
                     "T", 0.8f, 0.5f, 0.0f);
      } else if (j == Input::HEARING1 || j == Input::HEARING2) {
        RenderString(8 + yy * 2 / 3, 257 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12,
                     "E", 1.0f, 1.0f, 1.0f);
      }
      glBegin(GL_QUADS);
    }
    yy += 5;
    for (int j = 0; j < Output::OUTPUT_SIZE; j++) {
      col = selected.out[j];
      if (j == Output::RED)
        glColor3f(col, 0, 0);
      else if (j == Output::GRE)
        glColor3f(0, col, 0);
      else if (j == Output::BLU)
        glColor3f(0, 0, col);
      else if (j == Output::JUMP)
        glColor3f(col, col, 0);
      else if (j == Output::GRAB)
        glColor3f(0, col, col);
      else if (j == Output::TONE)
        glColor3f((1 - col) * (1 - col), 1 - fabs(col - 0.5) * 2, col * col);
      else glColor3f(col, col, col);
      glVertex3f(ww - yy * 2, 250 + ss * j, 0.0f);
      glVertex3f(ww - yy * 2, 250 + xx + ss * j, 0.0f);
      glVertex3f(ww - yy * 2 + ss, 250 + xx + ss * j, 0.0f);
      glVertex3f(ww - yy * 2 + ss, 250 + ss * j, 0.0f);
      glEnd();
      if (j == Output::LEFT_WHEEL_B || j == Output::LEFT_WHEEL_F ||
          j == Output::RIGHT_WHEEL_B || j == Output::RIGHT_WHEEL_F) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "!", 0.0f, 1.0f, 0.0f);
      } else if (j == Output::VOLUME) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "V", 1.0f, 1.0f, 1.0f);
      } else if (j == Output::CLOCKF3) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "Q", 0.0f, 0.0f, 0.0f);
      } else if (j == Output::SPIKE) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "S", 1.0f, 0.0f, 0.0f);
      } else if (j == Output::PROJECT) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "P", 0.5f, 0.0f, 0.5f);
      } else if (j == Output::JAW) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, ">", 1.0f, 1.0f, 0.0f);
      } else if (j == Output::GIVE) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "G", 0.0f, 0.3f, 0.0f);
      } else if (j == Output::GRAB) {
        RenderString(ww - yy - 15, 255 + xx / 3 + ss * j,
                     GLUT_BITMAP_HELVETICA_12, "G", 0.0f, 0.6f, 0.6f);
      }
      glBegin(GL_QUADS);

    }
    glEnd();
    glBegin(GL_QUADS);
    offx = 0;
    yy   = 30;
    xx   = 15;
    ss   = 16;
    for (int j = 0; j < selected.brain._neurons.size(); j++) {
      col = selected.brain._neurons[j].out;
      glColor3f(col, col, col);

      glVertex3f(50 + yy, 250 + offx + ss * j, 0.0f);
      glVertex3f(50 + yy, 250 + offx + xx + ss * j, 0.0f);
      glVertex3f(50 + yy + ss, 250 + offx + xx + ss * j, 0.0f);
      glVertex3f(50 + yy + ss, 250 + offx + ss * j, 0.0f);

      if ((j + 1) % 15 == 0) {
        yy += ss * 2;
        offx -= ss * 15;
      }
    }
    glEnd();

    //SOUND
    glBegin(GL_QUADS);
    glColor4f(0.10, 0.113, 0.137, 1);
    glVertex3f(50, wh - 15, 0);
    glVertex3f(50, wh - 95, 0);
    glVertex3f(ww - 15, wh - 95, 0);
    glVertex3f(ww - 15, wh - 15, 0);

    //each ear gets its hearing zone plotted
    for (int q = 0; q < NUMEARS; q++) {
      glColor4f(1 - (q / (NUMEARS - 1)), q / (NUMEARS - 1), 0,
                0.10 + 0.10 * (1 - q / (NUMEARS - 1)));
      if (selected.hearlow[q] + _world->SOUNDPITCHRANGE * 2 <
          selected.hearhigh[q]) {
        glVertex3f(
                52 + 485 * cap(selected.hearlow[q] + _world->SOUNDPITCHRANGE),
                wh - 17, 0);
        glVertex3f(
                52 + 485 * cap(selected.hearlow[q] + _world->SOUNDPITCHRANGE),
                wh - 93, 0);
        glVertex3f(
                52 + 485 * cap(selected.hearhigh[q] - _world->SOUNDPITCHRANGE),
                wh - 93, 0);
        glVertex3f(
                52 + 485 * cap(selected.hearhigh[q] - _world->SOUNDPITCHRANGE),
                wh - 17, 0);
      }

      glVertex3f(52 + 485 * selected.hearlow[q], wh - 17, 0);
      glVertex3f(52 + 485 * selected.hearlow[q], wh - 93, 0);
      glVertex3f(52 + 485 * selected.hearhigh[q], wh - 93, 0);
      glVertex3f(52 + 485 * selected.hearhigh[q], wh - 17, 0);
    }
    glEnd();
    glBegin(GL_LINES);
    for (int e = 0; e < _world->selectedSounds.size(); e++) {
      float volume = (int) _world->selectedSounds[e];
      float tone   = _world->selectedSounds[e] - volume;
      volume /= 100;

      float fiz = 1;
      if (volume >= 1)
        volume = cap(volume - 1.0);
      else
        fiz = 0.4;

      if (tone == 0.25)
        glColor4f(0.0, 0.8, 0.0, fiz);
      else
        glColor4f(0.7, 0.7, 0.7, fiz);

      glVertex3f(52 + 485 * tone, wh - 93, 0);
      glVertex3f(52 + 485 * tone, wh - 17 * volume, 0);
    }
    glEnd();

    //now show our own sound, colored by tone
    glLineWidth(2);
    glBegin(GL_LINES);
    glColor4f((1 - selected.tone) * (1 - selected.tone),
              1 - fabs(selected.tone - 0.5) * 2, selected.tone * selected.tone,
              0.5);
    glVertex3f(52 + 176 * selected.tone, wh - 93, 0);
    glVertex3f(52 + 176 * selected.tone, wh - 17 * selected.volume, 0);
    glEnd();
    glLineWidth(1);

    //eyesight profile. Draw a box with colored disks
    glBegin(GL_QUADS);
    glColor4f(0.10, 0.113, 0.137, 1);
    glVertex3f(50, wh - 180, 0);
    glVertex3f(50, wh - 100, 0);
    glVertex3f(ww - 15, wh - 100, 0);
    glVertex3f(ww - 15, wh - 180, 0);
    glEnd();

    for (int q = 0; q < NUMEYES; q++) {
      glBegin(GL_POLYGON);
      glColor3f(selected.in[Input::EYES + q * 3],
                selected.in[Input::EYES + 1 + q * 3],
                selected.in[Input::EYES + 2 + q * 3]);
      Forms::drawCircle(
              Vector2d(75 + selected.eyedir[q] / 2 / M_PI * 445, wh - 140),
              selected.eyefov[q] / M_PI * 30);
      glEnd();
    }

    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex3f(75, wh - 140, 0);
    glVertex3f(ww - 30, wh - 140, 0);
    glVertex3f(50 + (ww - 15 - 50) / 2, wh - 100, 0);
    glVertex3f(50 + (ww - 15 - 50) / 2, wh - 180, 0);

    glVertex3f(75, wh - 100, 0);
    glVertex3f(75, wh - 180, 0);

    glVertex3f(ww - 30, wh - 100, 0);
    glVertex3f(ww - 30, wh - 180, 0);

    glEnd();
  }

  glPopMatrix();
  glPopMatrix();
  glutSwapBuffers();
}

void GLView::renderStats() {
  float mm = 3;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.129, 0.145, 0.168, 255);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  int ww = glutGet(GLUT_WINDOW_WIDTH);
  int wh = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(0, ww, 0, wh);
  glScalef(1, -1, 1);
  glTranslatef(0, -wh, 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glBegin(GL_QUADS);
  glColor4f(0, 0.4, 0.5, 0.55);
  glVertex3f(10, 10, 0);
  glVertex3f(10, wh - 10, 0);
  glVertex3f(ww - 10, wh - 10, 0);
  glVertex3f(ww - 10, 10, 0);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0, 0, 0); //border around graphs and feedback

  glVertex3f(20, 20, 0);
  glVertex3f(20, 230, 0);

  glColor3f(0, 0, 0.8); //hybrid count
//  for(int q=0;q<conf::RECORD_SIZE-1;q++) {
//    if(q==_world->ptr-1) continue;
//    glVertex3f(-2020 + q*10, conf::HEIGHT - mm*_world->numHybrid[q],0);
//    glVertex3f(-2020 +(q+1)*10, conf::HEIGHT - mm*_world->numHybrid[q+1],0);
//  }
//  glColor3f(0,1,0); //herbivore count
//  for(int q=0;q<conf::RECORD_SIZE-1;q++) {
//    if(q==_world->ptr-1) continue;
//    glVertex3f(-2020 + q*10,conf::HEIGHT -mm*_world->numHerbivore[q],0);
//    glVertex3f(-2020 +(q+1)*10,conf::HEIGHT -mm*_world->numHerbivore[q+1],0);
//  }
//  glColor3f(1,0,0); //carnivore count
//  for(int q=0;q<conf::RECORD_SIZE-1;q++) {
//    if(q==_world->ptr-1) continue;
//    glVertex3f(-2020 + q*10,conf::HEIGHT -mm*_world->numCarnivore[q],0);
//    glVertex3f(-2020 +(q+1)*10,conf::HEIGHT -mm*_world->numCarnivore[q+1],0);
//  }
//  glColor3f(0.7,0.7,0); //frugivore count
//  for(int q=0;q<conf::RECORD_SIZE-1;q++) {
//    if(q==_world->ptr-1) continue;
//    glVertex3f(-2020 + q*10,conf::HEIGHT -mm*_world->numFrugivore[q],0);
//    glVertex3f(-2020 +(q+1)*10,conf::HEIGHT -mm*_world->numFrugivore[q+1],0);
//  }
//  glColor3f(0,0,0); //total count
//  for(int q=0;q<conf::RECORD_SIZE-1;q++) {
//    if(q==_world->ptr-1) continue;
//    glVertex3f(-2020 + q*10,conf::HEIGHT -mm*_world->numTotal[q],0);
//    glVertex3f(-2020 +(q+1)*10,conf::HEIGHT -mm*_world->numTotal[q+1],0);
//  }
//  glColor3f(0.8,0.8,0.6); //dead count
//  for(int q=0;q<conf::RECORD_SIZE-1;q++) {
//    if(q==_world->ptr-1) continue;
//    glVertex3f(-2020 + q*10,conf::HEIGHT -mm*(_world->numDead[q]+_world->numTotal[q]),0);
//    glVertex3f(-2020 +(q+1)*10,conf::HEIGHT -mm*(_world->numDead[q+1]+_world->numTotal[q+1]),0);
//  }

  //current population vertical bars
  glVertex3f(-2020 + _world->ptr * 10, conf::HEIGHT, 0);
  glVertex3f(-2020 + _world->ptr * 10, conf::HEIGHT - mm * _world->getUnits(),
             0);
  glColor3f(0, 0, 0);
  glVertex3f(-2020 + _world->ptr * 10, conf::HEIGHT, 0);
  glVertex3f(-2020 + _world->ptr * 10, conf::HEIGHT - mm * _world->getAlive(),
             0);
  glEnd();

  //labels for current population bars
  sprintf(_buf2, "%i dead", _world->getDead());
  RenderString(-2016 + _world->ptr * 10, conf::HEIGHT - mm * _world->getUnits(),
               GLUT_BITMAP_HELVETICA_12, _buf2, 0.8f, 0.8f, 0.6f);
  sprintf(_buf2, "%i units", _world->getAlive());
  RenderString(-2016 + _world->ptr * 10, conf::HEIGHT - mm * _world->getAlive(),
               GLUT_BITMAP_HELVETICA_12, _buf2, 0.0f, 0.0f, 0.0f);

  glPopMatrix();
  glPopMatrix();
  glutSwapBuffers();
}

void GLView::drawUnit(const Evolve::Unit &unit, float x, float y, bool ghost) {
  float n;
  float r  = unit.radius;
  float rp = unit.radius + 2.5;

  if (_live.unitsvis != Visual::NONE) {

    //color assignment
    float red        = 0, gre = 0, blu = 0;
    float discomfort = 0;

    //first, calculate colors which also have indicator boxes
    discomfort = cap(sqrt(abs(2.0 * abs(unit.pos.y / conf::HEIGHT - 0.5) -
                              unit.temperature_preference)));
    if (discomfort < 0.005) discomfort = 0;

    float stomach_red = cap(pow(unit.stomach[Stomach::MEAT], 2) +
                            pow(unit.stomach[Stomach::FRUIT], 2) -
                            pow(unit.stomach[Stomach::PLANT], 2));
    float stomach_gre = cap(pow(unit.stomach[Stomach::PLANT], 2) / 2 +
                            pow(unit.stomach[Stomach::FRUIT], 2) -
                            pow(unit.stomach[Stomach::MEAT], 2));

    float species_red = (cos((float) unit.species / 97 * M_PI) + 1.0) / 2.0;
    float species_gre = (sin((float) unit.species / 47 * M_PI) + 1.0) / 2.0;
    float species_blu = (cos((float) unit.species / 21 * M_PI) + 1.0) / 2.0;

    float metab_red = unit.metabolism;
    float metab_gre = 0.6 * unit.metabolism;
    float metab_blu = 0.2 + 0.4 * abs(unit.metabolism * 2 - 1);

    float lung_red = 0.2 + 2 * unit.lungs * (1 - unit.lungs);
    float lung_gre = 0.4 * unit.lungs + 0.3;
    float lung_blu = 0.6 * (1 - unit.lungs) + 0.2;
    //land (0.2,0.7,0.2), amph (0.2,0.5,0.5), water (0.2,0.3,0.8)


    //now colorize units and other things
    if (_live.unitsvis == Visual::RGB) { //real rgb values
      red = unit.red;
      gre = unit.gre;
      blu = unit.blu;

    } else if (_live.unitsvis == Visual::STOMACH) { //stomach
      red = stomach_red;
      gre = stomach_gre;

    } else if (_live.unitsvis == Visual::DISCOMFORT) { //temp discomfort
      red = discomfort;
      gre = (2 - discomfort) / 2;
      blu = 1 - discomfort;

    } else if (_live.unitsvis == Visual::VOLUME) { //volume
      red = unit.volume;
      gre = unit.volume;
      blu = unit.volume;

    } else if (_live.unitsvis == Visual::SPECIES) { //species
      red = species_red;
      gre = species_gre;
      blu = species_blu;

    } else if (_live.unitsvis ==
               Visual::CROSSABLE) { //crossover-compatable to selection
      //all other units look grey if unrelated or if none is selected, b/c then we don't have a reference
      red = 0.8;
      gre = 0.8;
      blu = 0.8;

      if (_world->getSelectedUnit() >= 0) {
        float deviation = abs(unit.species -
                              _world->units[_world->getSelectedUnit()].species); //species deviation check
        if (deviation == 0) { //exact copies
          red = 0.2;
        } else if (deviation <= _world->MAXDEVIATION) {
          //reproducable relatives
          red = 0;
          gre = 0;
        } else if (deviation <= 3 * _world->MAXDEVIATION) {
          //un-reproducable relatives
          red = 0.6;
          gre = 0.4;
        }
      }

    } else if (_live.unitsvis == Visual::HEALTH) { //health
      gre = unit.health <= 0.1 ? unit.health : powf(unit.health / 2, 0.5);
      red = ((int) (unit.health * 1000) % 2 == 0) ? (1.0 - unit.health / 2) *
                                                    gre : (1.0 -
                                                           unit.health / 2);

    } else if (_live.unitsvis == Visual::METABOLISM) {
      red = metab_red;
      gre = metab_gre;
      blu = metab_blu;

    } else if (_live.unitsvis == Visual::LUNGS) {
      red = lung_red;
      gre = lung_gre;
      blu = lung_blu;
    }


    //handle selected unit

    if (unit.id == _world->getSelection() && !ghost) {
      //draw selection
      glLineWidth(2);
      glBegin(GL_LINES);
      glColor3f(1, 1, 1);
      for (int k = 0; k < 17; k++) {
        n = k * (M_PI / 8);
        glVertex3f(x + (unit.radius + 4 / _scalemult) * sin(n),
                   y + (unit.radius + 4 / _scalemult) * cos(n), 0);
        n = (k + 1) * (M_PI / 8);
        glVertex3f(x + (unit.radius + 4 / _scalemult) * sin(n),
                   y + (unit.radius + 4 / _scalemult) * cos(n), 0);
      }
      glEnd();
      glLineWidth(1);

      if (_scalemult > .2) {
        glPushMatrix();
        glTranslatef(x - 90, y + 23, 0);

        //draw profile(s)
        float col;
        float yy = 15;
        float xx = 15;
        float ss = 16;

        glPopMatrix();

        if (_world->isDebug()) {
          //draw DIST range on selected in DEBUG
          glBegin(GL_LINES);
          glColor3f(1, 0, 1);
          for (int k = 0; k < 17; k++) {
            n = k * (M_PI / 8);
            glVertex3f(x + _world->DIST * sin(n), y + _world->DIST * cos(n), 0);
            n = (k + 1) * (M_PI / 8);
            glVertex3f(x + _world->DIST * sin(n), y + _world->DIST * cos(n), 0);
          }
          glEnd();

          //now spike, share, and grab effective zones
          glBegin(GL_POLYGON);
          glColor4f(1, 0, 0, 0.35);
          glVertex3f(x, y, 0);
          glVertex3f(x + (r + unit.spikeLength * _world->SPIKELENGTH) *
                         cos(unit.angle + M_PI / 8),
                     y + (r + unit.spikeLength * _world->SPIKELENGTH) *
                         sin(unit.angle + M_PI / 8), 0);
          glVertex3f(x + (r + unit.spikeLength * _world->SPIKELENGTH) *
                         cos(unit.angle),
                     y + (r + unit.spikeLength * _world->SPIKELENGTH) *
                         sin(unit.angle), 0);
          glVertex3f(x + (r + unit.spikeLength * _world->SPIKELENGTH) *
                         cos(unit.angle - M_PI / 8),
                     y + (r + unit.spikeLength * _world->SPIKELENGTH) *
                         sin(unit.angle - M_PI / 8), 0);
          glVertex3f(x, y, 0);
          glEnd();

          //grab is currently a range-only thing, change?
          glBegin(GL_POLYGON);
          glColor4f(0, 1, 1, 0.15);
          Forms::drawCircle(Vector2d(x, y), r + _world->GRABBING_DISTANCE);
          glEnd();

          //health-sharing
          glBegin(GL_POLYGON);
          glColor4f(0, 0.5, 0, 0.25);
          Forms::drawCircle(Vector2d(x, y), _world->FOOD_SHARING_DISTANCE);
          glEnd();
        }
      }

      if (_world->isDebug()) {
        glEnd();
        glBegin(GL_LINES);
        float    offx = 0;
        int      ss   = 30;
        int      yy   = ss;
        int      xx   = ss;
        for (int j    = 0; j < conf::BRAINSIZE; j++) {
          for (int k = 0; k < CONNS; k++) {
            int j2 = unit.brain._neurons[j].id[k];

            float x1 = 0;
            float y1 = 0;
            if (j < Input::INPUT_SIZE) {
              x1 = j * ss;
              y1 = yy;
            } else {
              x1 = ((j - Input::INPUT_SIZE) % 30) * ss;
              y1 = yy + ss + 2 * ss * ((int) (j - Input::INPUT_SIZE) / 30);
            }

            float x2 = 0;
            float y2 = 0;
            if (j2 < Input::INPUT_SIZE) {
              x2 = j2 * ss;
              y2 = yy;
            } else {
              x2 = ((j2 - Input::INPUT_SIZE) % 30) * ss;
              y2 = yy + ss + 2 * ss * ((int) (j2 - Input::INPUT_SIZE) / 30);
            }

            float ww = unit.brain._neurons[j].w[k];
            if (ww < 0) glColor3f(-ww, 0, 0);
            else glColor3f(0, 0, ww);

            glVertex3f(x1, y1, 0);
            glVertex3f(x2, y2, 0);
          }
        }
      }
    }

    //draw indicator of this unit... used for various events
    if (unit.indicator > 0) {
      glBegin(GL_POLYGON);
      glColor4f(unit.ir, unit.ig, unit.ib, 0.75);
      Forms::drawCircle(Vector2d(x, y), r + ((int) unit.indicator));
      glEnd();
    }

    //draw giving/receiving
    if (unit.dfood != 0) {
      glBegin(GL_POLYGON);
      float mag = cap(abs(unit.dfood) / _world->FOODTRANSFER) * 2 / 3;
      if (unit.dfood > 0) glColor3f(0, mag, 0);
      else glColor3f(mag, 0, 0); //draw sharing as a thick green or red outline
      for (int k = 0; k < 17; k++) {
        n = k * (M_PI / 8);
        glVertex3f(x + rp * sin(n), y + rp * cos(n), 0);
        n = (k + 1) * (M_PI / 8);
        glVertex3f(x + rp * sin(n), y + rp * cos(n), 0);
      }
      glEnd();
    }

    float tra = unit.health == 0 ? 0.4
                                 : 1; //mult for dead units, displayed with more transparent parts

    if (_scalemult > .1 ||
        ghost) { //dont render eyes, ears, or boost if zoomed too far out, but always render them on ghosts
      //draw eyes
      for (int q = 0; q < NUMEYES; q++) {
        glBegin(GL_LINES);
        if (_live.profilevis == Profile::EYES) {
          //color eyes based on actual input if we're on the eyesight profile
          glColor4f(unit.in[Input::EYES + q * 3],
                    unit.in[Input::EYES + 1 + q * 3],
                    unit.in[Input::EYES + 2 + q * 3], 0.75 * tra);
        } else glColor4f(0.5, 0.5, 0.5, 0.75 * tra);
        glVertex3f(x, y, 0);
        float aa = unit.angle + unit.eyedir[q];
        glVertex3f(x + (r + 30) * cos(aa), y + (r + 30) * sin(aa), 0);
        glEnd();
      }

      //ears
      for (int q = 0; q < NUMEARS; q++) {
        glBegin(GL_POLYGON);
        //color ears differently if we are set on the sound profile
        if (_live.profilevis == Profile::SOUND)
          glColor4f(1 - (q / (NUMEARS - 1)), q / (NUMEARS - 1), 0, 0.75);
        else glColor4f(0.6, 0.6, 0, 0.5);
        float aa = unit.angle + unit.eardir[q];
        Forms::drawCircle(Vector2d(x + r * cos(aa), y + r * sin(aa)), 2.3);
        glEnd();
      }

      //boost blur
      if (unit.boost) {
        for (int q = 1; q < 4; q++) {
          Vector2f displace(r / 4 * q * (unit.w1 + unit.w2), 0);
          displace.rotate(unit.angle + M_PI);

          glBegin(GL_POLYGON);
          glColor4f(red, gre, blu, 0.25);
          Forms::drawCircle(Vector2d(x + displace.x, y + displace.y), r);
          glEnd();
        }
      }

      //vis grabbing
      if ((_scalemult > .3 || ghost) && unit.grabbing > 0.5) {
        glLineWidth(2);
        glBegin(GL_LINES);

        glColor4f(0.0, 0.7, 0.7, 0.75);
        glVertex3f(x, y, 0);
        float mult = unit.grabID == -1 ? 1 : 0;
        float aa   = unit.angle + M_PI / 8 * mult;
        float ab   = unit.angle - M_PI / 8 * mult;
        glVertex3f(x + (_world->GRABBING_DISTANCE + r) * cos(aa),
                   y + (_world->GRABBING_DISTANCE + r) * sin(aa), 0);
        glVertex3f(x, y, 0);
        glVertex3f(x + (_world->GRABBING_DISTANCE + r) * cos(ab),
                   y + (_world->GRABBING_DISTANCE + r) * sin(ab), 0);
        glEnd();
        glLineWidth(1);

/* unit-unit directed grab vis code. Works but coords are wrong from World.cpp
glLineWidth(2);
				glBegin(GL_LINES);
				
				glColor4f(0.0,0.7,0.7,0.75);
				glVertex3f(x,y,0);
		
				if(unit.grabID!=-1) glVertex3f(unit.grabx, unit.graby, 0);
				else {
					float aa= unit.angle+M_PI/8;
					float ab= unit.angle-M_PI/8;
					glVertex3f(x+(_world->GRABBING_DISTANCE+r)*cos(aa), y+(_world->GRABBING_DISTANCE+r)*sin(aa), 0);
					glVertex3f(x,y,0);
					glVertex3f(x+(_world->GRABBING_DISTANCE+r)*cos(ab), y+(_world->GRABBING_DISTANCE+r)*sin(ab), 0);
				}
				glEnd();
				glLineWidth(1);
*/
      }

    }

    glBegin(GL_POLYGON);
    //body
    glColor4f(red, gre, blu, tra);
    Forms::drawCircle(Vector2d(x, y), r);
    glEnd();

    glBegin(GL_LINES);

    //outline and spikes are effected by the zoom magnitude
    float blur = cap(4.5 * _scalemult - 0.5);
    if (ghost) blur = 1; //disable effect for static rendering

    //jaws
    if ((_scalemult > .08 || ghost) && unit.jawrend > 0) {
      //dont render jaws if zoomed too far out, but always render them on ghosts, and only if they've been active within the last few ticks
      glColor4f(0.9, 0.9, 0, blur);
      float mult = 1 - powf(abs(unit.jawPosition), 0.5);
      glVertex3f(x + r * cos(unit.angle), y + r * sin(unit.angle), 0);
      glVertex3f(x + (10 + r) * cos(unit.angle + M_PI / 8 * mult),
                 y + (10 + r) * sin(unit.angle + M_PI / 8 * mult), 0);
      glVertex3f(x + r * cos(unit.angle), y + r * sin(unit.angle), 0);
      glVertex3f(x + (10 + r) * cos(unit.angle - M_PI / 8 * mult),
                 y + (10 + r) * sin(unit.angle - M_PI / 8 * mult), 0);
    }

    //outline
    float out_red = 0, out_gre = 0, out_blu = 0;
    if (unit.jump > 0) { //draw jumping as yellow outline
      out_red = 0.8;
      out_gre = 0.8;
    }
    if (unit.health <= 0) { //draw dead as grey outline
      glColor4f(0.7, 0.7, 0.7, tra);
    } else
      glColor3f(cap(out_red * blur + (1 - blur) * red),
                cap(out_gre * blur + (1 - blur) * gre),
                cap(out_blu * blur + (1 - blur) * blu));
    for (int k = 0; k < 17; k++) {
      n = k * (M_PI / 8);
      glVertex3f(x + r * sin(n), y + r * cos(n), 0);
      n = (k + 1) * (M_PI / 8);
      glVertex3f(x + r * sin(n), y + r * cos(n), 0);
    }

    //sound waves!
    if (_live.unitsvis == Visual::VOLUME && !ghost && unit.volume > 0) {
      float    volume = unit.volume;
      float    count  = unit.tone * 11 + 1;
      for (int l      = 0; l <= (int) count; l++) {
        float dist = _world->DIST * (l / count) +
                     4 * (_world->modcounter % (int) ((_world->DIST) / 4));
        if (dist > _world->DIST) dist -= _world->DIST;
        glColor4f((1 - unit.tone) * (1 - unit.tone),
                  1 - fabs(unit.tone - 0.5) * 2, unit.tone * unit.tone,
                  cap((1 - dist / _world->DIST) * volume));

        for (int k = 0; k < 32; k++) {
          n = k * (M_PI / 16);
          glVertex3f(x + dist * sin(n), y + dist * cos(n), 0);
          n = (k + 1) * (M_PI / 16);
          glVertex3f(x + dist * sin(n), y + dist * cos(n), 0);
        }
      }
    }

    //and spike, if harmful
    if ((_scalemult > .08 && unit.isSpikey(_world->SPIKELENGTH)) || ghost) {
      //dont render spike if zoomed too far out, but always render it on ghosts
      glColor4f(0.7, 0, 0, blur);
      glVertex3f(x, y, 0);
      glVertex3f(x + (_world->SPIKELENGTH * unit.spikeLength) * cos(unit.angle),
                 y + (_world->SPIKELENGTH * unit.spikeLength) * sin(unit.angle),
                 0);
    }
    glEnd();

    //some final debug stuff that is shown even on ghosts:
    if (_world->isDebug() || ghost) {
      //wheels and wheel speeds
      float wheelangle = unit.angle + M_PI / 2;
      glBegin(GL_LINES);
      glColor3f(0, 1, 0);
      glVertex3f(x + unit.radius / 2 * cos(wheelangle),
                 y + unit.radius / 2 * sin(wheelangle), 0);
      glVertex3f(x + unit.radius / 2 * cos(wheelangle) +
                 20 * unit.w1 * cos(unit.angle),
                 y + unit.radius / 2 * sin(wheelangle) +
                 20 * unit.w1 * sin(unit.angle), 0);
      wheelangle -= M_PI;
      glVertex3f(x + unit.radius / 2 * cos(wheelangle),
                 y + unit.radius / 2 * sin(wheelangle), 0);
      glVertex3f(x + unit.radius / 2 * cos(wheelangle) +
                 20 * unit.w2 * cos(unit.angle),
                 y + unit.radius / 2 * sin(wheelangle) +
                 20 * unit.w2 * sin(unit.angle), 0);
      glEnd();

      glBegin(GL_POLYGON);
      glColor3f(0, 1, 0);
      Forms::drawCircle(Vector2d(+unit.radius / 2 * cos(wheelangle),
                                 y + unit.radius / 2 * sin(wheelangle)), 1);
      glEnd();
      wheelangle += M_PI;
      glBegin(GL_POLYGON);
      Forms::drawCircle(Vector2d(+unit.radius / 2 * cos(wheelangle),
                                 y + unit.radius / 2 * sin(wheelangle)), 1);
      glEnd();
    }

    if (!ghost) { //only draw extra infos if not a ghost

      if (_scalemult > .3) {//hide extra visual data if really far away

        //debug stuff
        if (_world->isDebug()) {
          //debug sight lines: connect to anything selected unit sees
          glBegin(GL_LINES);
          for (int i = 0; i < (int) _world->linesA.size(); i++) {
            glColor3f(1, 1, 1);
            glVertex3f(_world->linesA[i].x, _world->linesA[i].y, 0);
            glVertex3f(_world->linesB[i].x, _world->linesB[i].y, 0);
          }
          _world->linesA.resize(0);
          _world->linesB.resize(0);
          glEnd();

          //debug cell smell box: outlines all cells the selected unit is "smelling"
          if (unit.id == _world->getSelection()) {
            int minx, maxx, miny, maxy;
            int scx = (int) (unit.pos.x / conf::CZ);
            int scy = (int) (unit.pos.y / conf::CZ);

            minx = (scx - _world->DIST / conf::CZ / 2) > 0 ?
                   (scx - _world->DIST / conf::CZ / 2) * conf::CZ : 0;
            maxx = (scx + 1 + _world->DIST / conf::CZ / 2) <
                   conf::WIDTH / conf::CZ ?
                   (scx + 1 + _world->DIST / conf::CZ / 2) * conf::CZ
                                          : conf::WIDTH;
            miny = (scy - _world->DIST / conf::CZ / 2) > 0 ?
                   (scy - _world->DIST / conf::CZ / 2) * conf::CZ : 0;
            maxy = (scy + 1 + _world->DIST / conf::CZ / 2) <
                   conf::HEIGHT / conf::CZ ?
                   (scy + 1 + _world->DIST / conf::CZ / 2) * conf::CZ
                                           : conf::HEIGHT;

            glBegin(GL_LINES);
            glColor3f(0, 1, 0);
            glVertex3f(minx, miny, 0);
            glVertex3f(minx, maxy, 0);
            glVertex3f(minx, maxy, 0);
            glVertex3f(maxx, maxy, 0);
            glVertex3f(maxx, maxy, 0);
            glVertex3f(maxx, miny, 0);
            glVertex3f(maxx, miny, 0);
            glVertex3f(minx, miny, 0);
            glEnd();
          }
        }

        //tags: quick HUD of basic bot traits/stats
        int xo      = 8 + unit.radius;
        int yo      = -21;
        int sep     = 2;
        int le      = 9;
        int wid     = 5;
        int numtags = 8;

        //health
        glBegin(GL_QUADS);
        glColor3f(0, 0, 0);
        glVertex3f(x + xo, y + yo, 0);
        glVertex3f(x + xo + 5, y + yo, 0);
        glVertex3f(x + xo + 5, y + yo + 42, 0);
        glVertex3f(x + xo, y + yo + 42, 0);

        glColor3f(0, 0.8, 0);
        glVertex3f(x + xo, y + yo + 21 * (2 - unit.health), 0);
        glVertex3f(x + xo + 5, y + yo + 21 * (2 - unit.health), 0);
        glVertex3f(x + xo + 5, y + yo + 42, 0);
        glVertex3f(x + xo, y + yo + 42, 0);

        //repcounter/energy
        xo += 7;
        glBegin(GL_QUADS);
        glColor3f(0, 0, 0);
        glVertex3f(x + xo, y + yo, 0);
        glVertex3f(x + xo + 5, y + yo, 0);
        glVertex3f(x + xo + 5, y + yo + 42, 0);
        glVertex3f(x + xo, y + yo + 42, 0);

        glColor3f(0, 0.7, 0.7);
        glVertex3f(x + xo, y + yo + 42 * cap(unit.repcounter / _world->REPRATE),
                   0);
        glVertex3f(x + xo + 5,
                   y + yo + 42 * cap(unit.repcounter / _world->REPRATE), 0);
        glVertex3f(x + xo + 5, y + yo + 42, 0);
        glVertex3f(x + xo, y + yo + 42, 0);

        //indicator tags
        xo += 7 + sep;
        for (int i = 0; i < numtags; i++) {
          int xmult = (int) floor((float) (i / 4));
          int ymult = i % 4;

          //different tag color schemes go here
          if (i == 0) {
            if (unit.hybrid) glColor3f(0, 0, 0.8); //hybrid?
            else if (unit.sexproject) glColor4f(0, 0.8, 0.8, 0.5);
            else continue;
          } else if (i == 1)
            glColor3f(stomach_red, stomach_gre, 0); //stomach type
          else if (i == 2)
            glColor3f(unit.volume, unit.volume,
                      unit.volume); //sound volume emitted
          else if (i == 3)
            glColor3f(discomfort, (2 - discomfort) / 2,
                      (1 - discomfort)); //temp discomfort
          else if (i == 4)
            glColor3f(lung_red, lung_gre,
                      lung_blu); //land/water lungs requirement
          else if (i == 7)
            glColor3f(metab_red, metab_gre, metab_blu); //metabolism
          else if (i == 5) { //ear 1 volume value heard
            float hear = unit.in[Input::HEARING1];
            if (hear == 0) continue;
            else glColor3f(hear, hear, hear); //! =D
          } else if (i == 6) { //ear 2 volume value heard
            float hear = unit.in[Input::HEARING2];
            if (hear == 0) continue;
            else glColor3f(hear, hear, hear);
          }

          glVertex3f(x + xo + (wid + sep) * xmult, y + yo + (le + sep) * ymult,
                     0);
          glVertex3f(x + xo + (wid + sep) * xmult + wid,
                     y + yo + (le + sep) * ymult, 0);
          glVertex3f(x + xo + (wid + sep) * xmult + wid,
                     y + yo + (le + sep) * ymult + le, 0);
          glVertex3f(x + xo + (wid + sep) * xmult,
                     y + yo + (le + sep) * ymult + le, 0);
        }
        glEnd();
      }

      if (_scalemult > .5) { // hide the number stats when zoomed out
        //generation count
        sprintf(_buf2, "%i", unit.gencount);
        RenderString(x - rp * 1.414, y - rp * 1.414, GLUT_BITMAP_HELVETICA_12,
                     _buf2, 0.8f, 1.0f, 1.0f);

        //age
        sprintf(_buf2, "%i", unit.age);
        float red = _world->HEALTHLOSS_AGING == 0 ? 0 : cap(
                (float) unit.age / _world->MAXAGE);
        //will be redder the closer it is to MAXAGE if health loss by aging is enabled
        RenderString(x - rp * 1.414, y - rp * 1.414 - 12,
                     GLUT_BITMAP_HELVETICA_12, _buf2, 0.8f, 1.0 - red,
                     1.0 - red);

        //species id
        sprintf(_buf2, "%i", unit.species);
        RenderString(x - rp * 1.414, y - rp * 1.414 - 24,
                     GLUT_BITMAP_HELVETICA_12, _buf2, species_red, species_gre,
                     species_blu);
      }
    }
  }
}

void GLView::drawData() {
  float mm = 3;
  if (_live.layersvis != 0) {
    glBegin(GL_QUADS);
    glColor4f(0, 0, 0.1, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(conf::WIDTH, 0, 0);
    glVertex3f(conf::WIDTH, conf::HEIGHT, 0);
    glVertex3f(0, conf::HEIGHT, 0);
    glEnd();
  }
}

void GLView::drawStatic() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int ww = glutGet(GLUT_WINDOW_WIDTH);
  int wh = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(0, ww, 0, wh);
  glScalef(1, -1, 1);
  glTranslatef(0, -wh, 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  if (_live.paused)
    RenderString(10, 20, GLUT_BITMAP_HELVETICA_12, "Paused", 0.5f, 0.5f, 0.5f);
  if (_live.follow != 0) {
    if (_world->getSelectedUnit() >= 0)
      RenderString(10, 40, GLUT_BITMAP_HELVETICA_12, "Following", 0.5f, 0.5f,
                   0.5f);
    else
      RenderString(10, 40, GLUT_BITMAP_HELVETICA_12, "No Follow Target", 1.0f,
                   0.5f, 0.5f);
  }
  if (_live.selection == Select::RELATIVE)
    RenderString(10, 60, GLUT_BITMAP_HELVETICA_12, "Relative Autoselect Mode",
                 0.5f, 0.8f, 0.5f);
  if (_world->isClosed())
    RenderString(10, 80, GLUT_BITMAP_HELVETICA_12, "Closed World", 0.5f, 0.5f,
                 0.5f);
  if (_world->isDebug()) {
    sprintf(_buf, "Plant: %i units",
            (int) (_world->getFoodSupp() - _world->getHazardSupp()));
    RenderString(5, 140, GLUT_BITMAP_HELVETICA_12, _buf, 0.5f, 0.5f, 1.0f);
    sprintf(_buf, "Fruit: %i units",
            (int) (_world->getFruitSupp() - _world->getHazardSupp()));
    RenderString(5, 160, GLUT_BITMAP_HELVETICA_12, _buf, 0.5f, 0.5f, 1.0f);
    sprintf(_buf, "Meat: %i units",
            (int) (_world->getMeatSupp() - _world->getHazardSupp()));
    RenderString(5, 180, GLUT_BITMAP_HELVETICA_12, _buf, 0.5f, 0.5f, 1.0f);
    sprintf(_buf, "Haz: %i units", (int) (-_world->getHazardSupp()));
    RenderString(5, 200, GLUT_BITMAP_HELVETICA_12, _buf, 0.5f, 0.5f, 1.0f);
    sprintf(_buf, "_modcounter: %i", _world->modcounter);
    RenderString(5, 240, GLUT_BITMAP_HELVETICA_12, _buf, 0.5f, 0.5f, 1.0f);
    sprintf(_buf, "GL _modcounter: %i", _modcounter);
    RenderString(5, 260, GLUT_BITMAP_HELVETICA_12, _buf, 0.5f, 0.5f, 1.0f);
  }

  if (_world->isDebug())
    Forms::drawRepere(Vector2d(ww, wh), Vector2d(25, 25));

  float ss       = 18;
  float movezero = 0;
  int   fadetime = conf::EVENTS_HALFLIFE - 20;

  int count = _world->events.size();
  if (conf::EVENTS_DISP < count) count = conf::EVENTS_DISP;

  for (int io = 0; io < count; io++) {
    int counter = _world->events[io].second;

    float fade = cap((-abs((float) counter) + conf::EVENTS_HALFLIFE) / 20);

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
                 GLUT_BITMAP_HELVETICA_12, _world->events[io].first, 0.0f, 0.0f,
                 0.0f, fade);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


void GLView::drawCell(int x, int y, float quantity) {
  if (_live.layersvis != 0) { //0: white
    glBegin(GL_QUADS);
    if (_live.layersvis ==
        Evolve::Layer::PLANTS + 1) { //plant food: green w/ navy blue background
      glColor4f(0.0, quantity, 0.1, 1);
    } else if (_live.layersvis ==
               Evolve::Layer::MEATS +
               1) { //meat food: red/burgundy w/ navy blue bg
      glColor4f(quantity, 0.0, 0.1, 1);
    } else if (_live.layersvis ==
               Evolve::Layer::HAZARDS +
               1) { //hazards: purple/munita w/ navy blue bg
      glColor4f(quantity, 0.0, quantity * 0.9 + 0.1, 1);
    } else if (_live.layersvis ==
               Evolve::Layer::FRUITS + 1) { //fruit: yellow w/ navy blue bg
      glColor4f(quantity * 0.8, quantity * 0.8, 0.1, 1);
    } else if (_live.layersvis == Evolve::Layer::LAND +
                                  1) { //land: green if 1, blue if 0, navy blue otherwise (debug)
      if (quantity == 1) glColor4f(0.3, 0.7, 0.3, 1);
      else if (quantity == 0) glColor4f(0.3, 0.3, 0.9, 1);
      else glColor4f(0, 0, 0.1, 1);
    } else if (_live.layersvis ==
               Evolve::Layer::LIGHT + 1) {
      if (_world->MOONLIT)
        glColor4f(0.5 * quantity + 0.5, 0.5 * quantity + 0.5,
                  quantity * 0.3 + 0.5, 1);
      else glColor4f(quantity, quantity, quantity * 0.7 + 0.1, 1);
    } else if (_live.layersvis == Evolve::Layer::TEMP +
                                  1) { //temp: yellow near the equator, blue at the poles
      float col = cap(
              2 * abs((float) y * conf::CZ / conf::HEIGHT - 0.5) - 0.02);
      glColor3f(1 - col, 1 - col / 2, col);
    }

    //code to make cells into divided boxes when zoomed close up
    float gadjust = 0;
    if (_scalemult > 0.8 || _live.grid)
      gadjust = _scalemult <= 0 ? 0 : 0.5 / _scalemult;
    glVertex3f(x * conf::CZ + gadjust, y * conf::CZ + gadjust, 0);
    glVertex3f(x * conf::CZ + conf::CZ - gadjust, y * conf::CZ + gadjust, 0);
    glVertex3f(x * conf::CZ + conf::CZ - gadjust,
               y * conf::CZ + conf::CZ - gadjust, 0);
    glVertex3f(x * conf::CZ + gadjust, y * conf::CZ + conf::CZ - gadjust, 0);
    glEnd();
  }
}

void GLView::setWorld(Evolve::World *w) {
  _world = w;
}