/* ************************************************************************** *
 *   Color.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 19/04/17                               #+#     #+#  #+#         *
 *   Updated: 19/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_COLOR_HPP
#define EVOLVE_COLOR_HPP

namespace Evolve {
    class Color {
    public:
        unsigned char _red, _green, _blue, _alpha;

        Color()
                : _red(0x00), _green(0x00), _blue(0x00), _alpha(0xFF) {}

        Color(unsigned char red, unsigned char green, unsigned char blue,
              unsigned char alpha = 0xFF)
                : _red(red), _green(green), _blue(blue), _alpha(alpha) {}

        Color(float red, float green, float blue, float alpha = 1.0)
                : _red(red * 255.0 + .5), _green(green * 255.0 + .5),
                  _blue(blue * 255.0 + .5), _alpha(alpha * 255.0 + .5) {}

        Color(const Color &rhs)
                : _red(rhs._red), _green(rhs._green), _blue(rhs._blue),
                  _alpha(rhs._alpha) {}

        unsigned char &getRed() { return _red; }

        unsigned char &getGreen() { return _green; }

        unsigned char &getBlue() { return _blue; }

        unsigned char &getAlpha() { return _alpha; }

        const unsigned char &getRed() const { return _red; }

        const unsigned char &getGreen() const { return _green; }

        const unsigned char &getBlue() const { return _blue; }

        const unsigned char &getAlpha() const { return _alpha; }

        void setFRed(float val) { _red = val * 255.0 + .5; }

        void setFGreen(float val) { _green = val * 255.0 + .5; }

        void setFblue(float val) { _blue = val * 255.0 + .5; }

        void setFAlpha(float val) { _alpha = val * 255.0 + .5; }

        float getFRed() const { return _red / 255.0; }

        float getFGreen() const { return _green / 255.0; }

        float getFblue() const { return _blue / 255.0; }

        float getFAlpha() const { return _alpha / 255.0; }

        unsigned int rgba() {
          return (_red << 24) | (_green << 16) | (_blue << 8) | (_alpha << 0);
        }
    };
}

#endif //EVOLVE_COLOR_HPP
