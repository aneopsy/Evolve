//
// Created by Aneosy on 31/03/2017.
//

#ifndef EVOLVE_DRAWS_HPP
# define EVOLVE_DRAWS_HPP

# include <IBrain.hpp>

namespace                       DRAWS {
    namespace Input {
        enum {
            EYES = 0,
            xEYES= EYES+NUMEYES*3-1,
            HEALTH,
            RANDOM,
            CLOCK1,
            CLOCK2,
            CLOCK3,
            HEARING1,
            HEARING2,
            BOT_SMELL,
            BLOOD,
            TEMP,
            PLAYER,
            FRUIT_SMELL,
            MEAT_SMELL,
            HAZARD_SMELL,
            WATER_SMELL,
            INPUT_SIZE
        };
    };

    namespace Output {
        enum {
            //LEFT RIGHT BOOST JUMP R G B VOLUME GIVING SPIKE CHOICE STIMULANT
            // 0	 1	   2    3   4 5 6   7	   8	  9	    10		11
            LEFT_WHEEL_F = 0,
            RIGHT_WHEEL_F,
            LEFT_WHEEL_B,
            RIGHT_WHEEL_B,
            BOOST,
            JUMP,
            RED,
            GRE,
            BLU,
            VOLUME,
            TONE,
            GIVE,
            SPIKE,
            JAW,
            GRAB,
            PROJECT,
            STIMULANT,
            CLOCKF3,
            OUTPUT_SIZE
        };
    };

    class                       Neuron : public INeuron {
    public:
        Neuron(t_brain const &settings);

        std::vector<float>      w;
        std::vector<int>        id;
        std::vector<int>        type;

        int                     seed;
        float                   kp;
        float                   gw;
        float                   bias;

        float                   target;
        float                   out;
        float                   oldout;
    };

    class                       Brain : public IBrain {
    public:
                                Brain(t_brain const &settings);
                                ~Brain();
                                Brain(const Brain &other);
        virtual Brain           &operator=(const Brain &other);
        void                    tick(std::vector<float> &in, std::vector<float> &out);
        float                   getActivity();
        void                    initMutate(float MR, float MR2);
        void                    liveMutate(float MR, float MR2, std::vector<float> &out);
        Brain                   crossover(const Brain &other);
        t_brain                 getBrain() const;
        std::vector <Neuron>    _neurons;

    private:
        t_brain                 _brain;
    };
}
#endif //EVOLVE_DRAWS_HPP
