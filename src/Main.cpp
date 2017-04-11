//
// Created by Aneosy on 31/03/2017.
//
#include <iostream>
#include <DRAWS.hpp>
#include <DRAWS.hpp>
#include <Utils.hpp>
#include <ctime>


int                         main(void) {
    std::vector<float>      in(20);
    std::vector<float>      out(9);

    std::cout << "Evolve Starting..." << std::endl;
    t_brain settings;
    settings.nbneuron = 100;
    settings.inputsize = 20;
    settings.outputsize = 9;
    settings.directinput = 0.1;
    settings.deadconns = 0.3;
    settings.changeconn = 0.15;
    settings.memconn = 0.01;
    settings.learnrate = 0.001;
    DRAWS::Brain brain(settings);

    for (int x = 0; x < 1000; ++x) {
        for (int j = 0; j < settings.inputsize; j++) {
            in[j] = randf(0, 1);
        }
        brain.tick(in, out);
        for (std::vector<float>::const_iterator i = out.begin(); i != out.end(); ++i)
            std::cout << *i << std::endl;
       std::cout << "activity: " <<  brain.getActivity() << std::endl;
    }
    return (0);
}