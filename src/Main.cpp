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
#include <DRAWS.hpp>
#include <Utils.hpp>
#include <ctime>

#include <GL/glut.h>
#include <glui.h>
#include "World.hpp"
#include "glview.hpp"
#include "Settings.hpp"

GLView* GLVIEW = new GLView(0);

int main(int argc, char **argv)
{
  srand(time(0));

  printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );
  Evolve::World* world = new Evolve::World();
  GLVIEW->setWorld(world);
  world->setDebug(true);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(250,20);
  glutInitWindowSize(conf::WWIDTH, conf::WHEIGHT);
  GLVIEW->win1= glutCreateWindow("Evolve");

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

  GLVIEW->glCreateMenu();

  try{
    glutMainLoop();
  } catch( std::bad_alloc &){
    printf("Out of memory!\n");
  } catch( std::bad_exception &){
    printf("Error!\n");
  }
  return 0;
}
