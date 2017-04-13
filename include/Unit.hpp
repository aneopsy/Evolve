/* ************************************************************************** *
 *   Unit.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_UNIT_HPP
#define EVOLVE_UNIT_HPP

#include "Vector.hpp"
#include "DRAWS.hpp"
#include "Settings.hpp"

#include <vector>
#include <string>

namespace Evolve {
    class Unit {
    public:
        Unit(t_brain settings, float MEANRADIUS, float REPRATE, float MUTARATE1,
             float MUTARATE2);

        void printSelf();
        void initEvent(float size, float r, float g, float b);

        void tick();
        void setActivity();
        void writeIfKilled(const char *cause);

        Unit reproduce(Unit that, float REPRATE);

        void setHerbivore();
        void setCarnivore();
        void setFrugivore();
        void setPos(float x, float y);
        void borderRectify();

        bool isHerbivore() const;
        bool isCarnivore() const;
        bool isFrugivore() const;
        bool isTerran() const;
        bool isAquatic() const;
        bool isSpikey(float SPIKELENGTH) const;

        int                      id;
        Vector2f                 pos;
        float                    angle;
        float                    health;
        int                      age;
        float                    MUTRATE1;
        float                    MUTRATE2;
        float                    radius;
        //sens
        bool                     near;
        int                      freshkill;
        int                      species;
        int                      gencount;
        float                    repcounter;
        int                      numbabies;
        float                    temperature_preference;
        float                    lungs;
        float                    metabolism;
        float                    stomach[Stomach::FOOD_TYPES];
        //sensor
        float                    eye_see_unit_mod;
        std::vector<float>       eyefov;
        std::vector<float>       eyedir;
        float                    hear_mod;
        std::vector<float>       eardir;
        std::vector<float>       hearlow;
        std::vector<float>       hearhigh;
        float                    clockf1, clockf2, clockf3;
        float                    blood_mod;
        float                    smell_mod;
        //brain
        DRAWS::Brain             brain;
        std::vector<float>       in;
        std::vector<float>       out;
        float                    brainact;
        //caract
        float                    w1;
        float                    w2;
        float                    strength;
        bool                     boost;
        float                    jump;
        float                    red, gre, blu;
        float                    volume;
        float                    tone;
        float                    give;
        float                    spikeLength;
        float                    jawPosition;
        float                    jawOldPos;
        int                      grabID;
        float                    grabbing;
        bool                     sexproject;
        //stats
        bool                     hybrid;
        const char               *death;
        int                      children;
        int                      killed;
        int                      hits;
        float                    indicator;
        float                    ir, ig, ib;
        float                    dfood;
        float                    dfoodx;
        float                    dfoody;
        float                    grabx;
        float                    graby;
        int                      jawrend;
        std::vector<std::string> mutations;
    };
}

#endif //EVOLVE_UNIT_HPP
