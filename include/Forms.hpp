/* ************************************************************************** *
 *   Forms.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 13/04/17                               #+#     #+#  #+#         *
 *   Updated: 13/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_FORMS_HPP
#define EVOLVE_FORMS_HPP

#include "Vector.hpp"
#include <GL/gl.h>

namespace Forms {
    void drawRepere(Vector2d dim, Vector2d size);
    void drawCircle(Vector2f dim, float r);
    void drawQuadrant(Vector2f dim, float r, Vector2f size);
}


#endif //EVOLVE_FORMS_HPP
