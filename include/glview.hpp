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

    typedef struct s_live {
        int worldclosed;
        int paused;
        int fastmode;
        int skipdraw;
        int unitsvis;
        int layersvis;
        int profilevis;
        int selection;
        int follow;
        int autosave;
        int grid;
        int debug;
    } t_live;

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
    void renderProfile();
    void renderStats();
    void handleRW(int action);
    void handleCloses(int action);

    void glCreateMenu();
    void gluiCreateMenu();

    int m_id;
    int mainWin;
    int profileWin;
    int settingsWin;
    int statsWin;

private:
    void countdownEvents();

    Evolve::World    *_world;
    t_live           _live;
    bool             _debug;
    GLUI             *_menu;
    GLUI_FileBrowser *_Browser;
    std::string      _file_name;
    GLUI             *_Loader;
    GLUI             *_Saver;
    GLUI             *_Alert;
    GLUI_EditText    *_Filename;
    char             _filename[30];
    char             _buf[100];
    char             _buf2[10];
    int              _modcounter;
    int              _lastUpdate;
    int              _frames;
    float            _scalemult;
    int              _downb[7];
    Vector2f         _translate;
    Vector2d         _mouse;
    bool             _mousedrag;
};

#endif // GLVIEW_H


#endif //EVOLVE_GLVIEW_HPP
