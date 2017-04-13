//
// Created by Aneosy on 02/04/2017.
//

#ifndef EVOLVE_IBRAIN_HPP
#define EVOLVE_IBRAIN_HPP

#include <ANeuron.hpp>

class                       IBrain {
public:

    virtual                 ~IBrain(){}
    virtual void            tick(std::vector<float> &in, std::vector<float> &out) = 0;
    virtual float           getActivity() = 0;
};

#endif //EVOLVE_IBRAIN_HPP
