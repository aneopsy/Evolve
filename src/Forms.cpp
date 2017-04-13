/* ************************************************************************** *
 *   Forms.cpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 13/04/17                               #+#     #+#  #+#         *
 *   Updated: 13/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#include "Forms.hpp"

void Forms::drawRepere(Vector2d dim, Vector2d size) {
  glBegin(GL_LINES);
  //green axis
  glColor4f(0, 1, 0, 0.3);
  glVertex3f(dim.x / 2, dim.y / 2, 0);
  glVertex3f(dim.x / 2, dim.y / 2 + size.y, 0);
  //red axis
  glColor4f(1, 0, 0, 0.3);
  glVertex3f(dim.x / 2, dim.y / 2, 0);
  glVertex3f(dim.x / 2 + size.x, dim.y / 2, 0);
  glEnd();
}

void Forms::drawCircle(Vector2f dim, float r) {
  float    n;
  for (int k = 0; k < 17; k++) {
    n = k * (M_PI / 8);
    glVertex3f(dim.x + r * sin(n), dim.y + r * cos(n), 0);
  }
}

void Forms::drawQuadrant(Vector2f dim, float r, Vector2f size) {
  glVertex3f(dim.x, dim.y, 0);
  float    n;
  for (int k = 0; k < (int) ((size.y - size.x) * 8 / M_PI + 1); k++) {
    n = k * (M_PI / 8) + size.x;
    glVertex3f(dim.x + r * sin(n), dim.y + r * cos(n), 0);
  }
  glVertex3f(dim.x + r * sin(size.y), size.y + r * cos(size.y), 0);
  glVertex3f(dim.x, dim.y, 0);
}