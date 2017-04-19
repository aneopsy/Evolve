/* ************************************************************************** *
 *   DWRAON.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_DWRAON_HPP
#define EVOLVE_DWRAON_HPP

# include <IBrain.hpp>

namespace                       DWRAON {
    class                       Neuron : public ANeuron {
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
        std::vector <Neuron>    _neurons;

    private:
        t_brain                 _brain;
    };
}

#endif //EVOLVE_DWRAON_HPP
