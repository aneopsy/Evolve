/* ************************************************************************** *
 *   View.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_VIEW_HPP
#define EVOLVE_VIEW_HPP

#include "Unit.hpp"

namespace Evolve {
    class IView {
    public:
        virtual void
        drawUnit(const Unit &a, float x, float y, bool ghost = 0) = 0;
        virtual void drawCell(int x, int y, float quantity) = 0;
        virtual void drawData() = 0;
        virtual void drawStatic() = 0;

        virtual void trySaveWorld(bool autosave = false) = 0;
//        virtual void trySaveAgent() = 0;
//        virtual void tryLoadAgent() = 0;
    };
}

#endif //EVOLVE_VIEW_HPP
