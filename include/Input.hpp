/* ************************************************************************** *
 *   Input.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 23/03/17                               #+#     #+#  #+#         *
 *   Updated: 23/03/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_INPUT_HPP
#define EVOLVE_INPUT_HPP

namespace Evolve {
    struct InputT {
        InputT(void) {
          _state   = None;
          _key     = None;
          _unicode = None;
        }

        InputT(int const state, int const key, int const unicode) {
          _state   = state;
          _key     = key;
          _unicode = unicode;
        }

        ~InputT(void) {}

        int _state;
        int _key;
        int _unicode;

        enum InputState {
            None,
            KeyPressed,
            KeyRelease
        };
    };

    enum Input {
        NONE,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        ESCAPE,
        ENTER,
        SPACE,
        NEXT_LIB,
        PREV_LIB,
        NEXT_GAME,
        PREV_GAME
    };
}

namespace std {
    template<>
    class less<Evolve::InputT> {
    public:
        bool operator()(Evolve::InputT const &lhs, Evolve::InputT const &rhs) {
          if (lhs._state != rhs._state)
            return (lhs._state < rhs._state);
          return (lhs._key < rhs._key);
        }
    };
}

#endif //EVOLVE_INPUT_HPP
