/* ************************************************************************** *
 *   glview.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_GLVIEW_HPP
#define EVOLVE_GLVIEW_HPP

#ifndef GLVIEW_H
#define GLVIEW_H

#include "IView.hpp"
#include "World.hpp"
#include "glui.h"
#include "Forms.hpp"

class GLView;

extern GLView *GLVIEW;

void gl_processNormalKeys(unsigned char key, int x, int y);
void gl_processSpecialKeys(int key, int x, int y);
void gl_processReleasedKeys(unsigned char key, int x, int y);
void gl_menu(int key);
void gl_processMouse(int button, int state, int x, int y);
void gl_processMouseActiveMotion(int x, int y);
void gl_processMousePassiveMotion(int x, int y);
void gl_changeSize(int w, int h);
void gl_handleIdle();
void gl_renderScene();
void glui_handleRW(int action);
void glui_handleCloses(int action);

class GLView : public Evolve::IView {

public:
    GLView(Evolve::World *w);
    virtual ~GLView();

    virtual void
    drawUnit(const Evolve::Unit &a, float x, float y, bool ghost = 0);
    virtual void
    drawCell(int x, int y, float quantity);
    virtual void drawData();
    virtual void drawStatic();

    virtual void trySaveWorld(bool autosave = false);
//    virtual void trySaveUnit(){};
//    virtual void tryLoadUnit(){};

    void setWorld(Evolve::World *w);

    //GLUT functions
    void processNormalKeys(unsigned char key, int x, int y);
    void processSpecialKeys(int key, int x, int y);
    void processReleasedKeys(unsigned char key, int x, int y);
    void processMouse(int button, int state, int x, int y);
    void processMouseActiveMotion(int x, int y);
    void processMousePassiveMotion(int x, int y);
    void menu(int key);
    void menuS(int key);
    void changeSize(int w, int h);
    void handleIdle();
    void renderScene();
    void handleRW(int action);
    void handleCloses(int action);

    void glCreateMenu();
    int m_id;
    int mainWin;
    int profileWin;
    int settingsWin;
    int statsWin;
    void gluiCreateMenu();

private:
    void countdownEvents();

    Evolve::World    *world;
    int              live_worldclosed;
    int              live_paused;
    int              live_fastmode;
    int              live_skipdraw;
    int              live_unitsvis;
    int              live_layersvis;
    int              live_profilevis;
    int              live_selection;
    int              live_follow;
    int              live_autosave;
    int              live_grid;
    int              live_debug;
    bool             debug;
    GLUI             *Menu;
    GLUI_FileBrowser *Browser;
    std::string      file_name;
    GLUI             *Loader;
    GLUI             *Saver;
    GLUI             *Alert;
    GLUI_EditText    *Filename;
    char             filename[30];
    char             buf[100];
    char             buf2[10];
    int              modcounter;
    int              lastUpdate;
    int              frames;
    float            scalemult;
    float            xtranslate, ytranslate;
    int              downb[3];
    int              mousex, mousey;
    bool             mousedrag;
};

#endif // GLVIEW_H


#endif //EVOLVE_GLVIEW_HPP
