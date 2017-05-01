/* ************************************************************************** *
 *   ANeuron.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_NEURON_HPP
#define EVOLVE_NEURON_HPP

# include                   <vector>

# define CONNS 5
# define NUMEYES 6

typedef struct              s_brain {
    int                     nbneuron;
    int                     inputsize;
    int                     outputsize;
    float                   directinput;
    float                   deadconns;
    float                   changeconn;
    float                   memconn;
    float                   learnrate;
}                           t_brain;

class                       ANeuron {
public:
    std::vector<float>      w;
    std::vector<int>        id;
    std::vector<int>        type;

    float                   kp;
    float                   gw;
    float                   bias;

    float                   target;
    float                   out;
    float                   oldout;
};

#endif //EVOLVE_NEURON_HPP
