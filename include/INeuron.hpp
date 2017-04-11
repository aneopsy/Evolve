//
// Created by Aneosy on 02/04/2017.
//

#ifndef EVOLVE_NEURON_HPP
#define EVOLVE_NEURON_HPP

# include                   <vector>

# define CONNS 3
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

class                       INeuron {
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
