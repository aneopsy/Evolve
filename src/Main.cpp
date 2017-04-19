/* ************************************************************************** *
 *   Main.cpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#include <iostream>
#include <DRAWS.hpp>
#include <Utils.hpp>
#include <ctime>

#include <GL/glut.h>
#include <glui.h>
#include "World.hpp"
#include "glview.hpp"

GLView *GLVIEW = new GLView(0);

int main(int argc, char **argv) {
  srand(time(0));

  printf("GLUI version: %3.2f\n", GLUI_Master.get_version());
  Evolve::World *world = new Evolve::World();
  GLVIEW->setWorld(world);
  world->setDebug(true);

  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(conf::WIDTH - 550, 0);
  glutInitWindowSize(550, 1000);
  GLVIEW->profileWin = glutCreateWindow("Profile");

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(0, conf::HEIGHT - 150);
  glutInitWindowSize(1200, 150);
  GLVIEW->statsWin = glutCreateWindow("Stats");

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(250, 0);
  glutInitWindowSize(conf::WWIDTH, conf::WHEIGHT);
  GLVIEW->mainWin = glutCreateWindow("Evolve");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_FLAT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  GLUI_Master.set_glutDisplayFunc(gl_renderScene);
  GLUI_Master.set_glutIdleFunc(gl_handleIdle);
  GLUI_Master.set_glutReshapeFunc(gl_changeSize);

  glutKeyboardFunc(gl_processNormalKeys);
  glutSpecialFunc(gl_processSpecialKeys);
  glutKeyboardUpFunc(gl_processReleasedKeys);
  glutMouseFunc(gl_processMouse);
  glutMotionFunc(gl_processMouseActiveMotion);
  glutPassiveMotionFunc(gl_processMousePassiveMotion);

  GLVIEW->gluiCreateMenu();
  GLVIEW->glCreateMenu();

  try {
    glutMainLoop();
  } catch (std::bad_alloc &) {
    printf("Out of memory!\n");
  } catch (std::bad_exception &) {
    printf("Error!\n");
  }
  return 0;
}


/*
int                         main(void) {
    std::vector<float>      in(20);
    std::vector<float>      out(9);

    std::cout << "Evolve Starting..." << std::endl;
    t_brain settings;
    settings.nbneuron = 100;
    settings.inputsize = 20;
    settings.outputsize = 9;
    settings.directinput = 0.1;
    settings.deadconns = 0.3;
    settings.changeconn = 0.15;
    settings.memconn = 0.01;
    settings.learnrate = 0.001;
    DRAWS::Brain brain(settings);

    for (int x = 0; x < 1000; ++x) {
        for (int j = 0; j < settings.inputsize; j++) {
            in[j] = randf(0, 1);
        }
        brain.tick(in, out);
        for (std::vector<float>::const_iterator i = out.begin(); i != out.end(); ++i)
            std::cout << *i << std::endl;
       std::cout << "activity: " <<  brain.getActivity() << std::endl;
    }
    return (0);
}
*/
