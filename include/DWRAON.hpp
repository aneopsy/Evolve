//
// Created by Aneosy on 02/04/2017.
//

#ifndef EVOLVE_DWRAON_HPP
#define EVOLVE_DWRAON_HPP

# include <IBrain.hpp>

namespace                       DWRAON {
    class                       Neuron : public INeuron {
    public:
        Neuron(t_brain const &settings);
        int                     genre;
    };

    class                       Brain : public IBrain {
    public:
        Brain(t_brain const &settings);
        ~Brain();
        Brain(const Brain &other);
        virtual Brain           &operator=(const Brain &other);
        void                    tick(std::vector<float> &in, std::vector<float> &out);
        float                   getActivity();

    private:
        std::vector <Neuron>    _neurons;
        t_brain                 _brain;
    };
}

#endif //EVOLVE_DWRAON_HPP