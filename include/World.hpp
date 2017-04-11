/* ************************************************************************** *
 *   World.hpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#ifndef EVOLVE_WORLD_HPP
#define EVOLVE_WORLD_HPP

#include <vector>
#include <Vector.hpp>

#include "IView.hpp"
#include "Settings.hpp"

namespace Evolve {
    typedef struct stats_s {
        int   STATherbivores;
        int   STATfrugivores;
        int   STATcarnivores;
        int   STATterrans;
        int   STATaquatic;
        int   STATalive;
        int   STATdead;
        int   STATspiked;
        int   STAThybrids;
        int   STATbestherbi;
        int   STATbestfrugi;
        int   STATbestcarni;
        int   STATbestterran;
        int   STATbestaquatic;
        int   STATbesthybrid;
        int   STATplants;
        int   STATfruits;
        int   STATmeats;
        int   STAThazards;
        float STATallplant;
        float STATallfruit;
        float STATallmeat;
        float STATallhazard;
    } stats_t;

    class World {
    public:
        World();
        ~World();

        void update();
        void reset();
        void spawn();
        void readConfig();
        void writeConfig();

        void draw(IView *view, int layer);

        bool isClosed() const;
        void setClosed(bool close);

        bool isDebug() const;
        void setDebug(bool state);
        std::vector <Vector2f> linesA;
        std::vector <Vector2f> linesB;

        void positionOfInterest(int type, float &xi, float &yi);

        int getSelection() const;

        int   pinput1;
        float pleft;
        float pright;
        bool  pcontrol;
        void setControl(bool state);

        void setSelection(int type);
        void setSelectedUnit(int idx = -1);
        int getSelectedUnit() const;
        int getClosestRelative(int idx = -1) const;
        void selectedHeal();
        void selectedKill();
        void selectedBabys();
        void selectedMutate();
        void getFollowLocation(float &xi, float &yi);

        bool isAutoselecting() const;
        void setAutoselect(bool state);

        int epoch() const;

        bool processMouse(int button, int state, int x, int y, float scale);

        void
        addUnits(int num, int set_stomach = -1, float nx = -1, float ny = -1,
                  bool set_lungs = true);

        int numHerbivore[conf::RECORD_SIZE];
        int numCarnivore[conf::RECORD_SIZE];
        int numFrugivore[conf::RECORD_SIZE];
        int numHybrid[conf::RECORD_SIZE];
        int numDead[conf::RECORD_SIZE];
        int numTotal[conf::RECORD_SIZE];
        int ptr;

        int modcounter;
        int current_epoch;
        int idcounter;

        std::vector <Unit> units;

        void setInputs();
        void brainsTick();
        void processOutputs();
        void
        processInteractions();

        void healthTick();

        std::vector<const char *> deaths;

        std::vector<float> selectedSounds;

        std::vector <std::pair<const char *, int>> events;
        void addEvent(const char *text);

        int getHerbivores() const;
        int getCarnivores() const;
        int getFrugivores() const;
        int getLungLand() const;
        int getLungWater() const;
        int getUnits() const;
        int getHybrids() const;
        int getSpiked() const;
        int getAlive() const;
        int getDead() const;
        int getFood() const;
        int getFruit() const;
        int getMeat() const;
        int getHazards() const;
        float getFoodSupp() const;
        float getFruitSupp() const;
        float getMeatSupp() const;
        float getHazardSupp() const;

        int   CW;
        int   CH;
        float cells[Layer::LAYERS][
                      conf::WIDTH / conf::CZ][
                      conf::HEIGHT / conf::CZ];

        int   MINFOOD;
        float INITFOODDENSITY;
        int   INITFOOD;
        float INITFRUITDENSITY;
        int   INITFRUIT;

        int NUMBOTS;
        int ENOUGHBOTS;
        int TOOMANYBOTS;

        int REPORTS_PER_EPOCH;
        int FRAMES_PER_EPOCH;
        int FRAMES_PER_DAY;

        int   CONTINENTS;
        float OCEANPERCENT;
        bool  MOONLIT;
        float GRAVITYACCEL;
        float REACTPOWER;
        float SPIKEMULT;
        int   BRAINSIZE;
        float BOTSPEED;
        float BOOSTSIZEMULT;
        float SOUNDPITCHRANGE;
        float FOODTRANSFER;
        float MEANRADIUS;
        float SPIKESPEED;
        int   FRESHKILLTIME;
        int   TENDERAGE;
        float MINMOMHEALTH;
        bool  FUN;
        float REPRATE;
        bool  OVERHEAL_REPFILL;
        float LEARNRATE;
        float MAXDEVIATION;
        float MUTCHANCE;
        float MUTSIZE;
        int   MAXAGE;

        float DIST;
        float SPIKELENGTH;
        float TOOCLOSE;
        float FOOD_SHARING_DISTANCE;
        float SEXTING_DISTANCE;
        float GRABBING_DISTANCE;

        float HEALTHLOSS_WHEELS;
        float HEALTHLOSS_BOOSTMULT;
        float HEALTHLOSS_BADTEMP;
        float HEALTHLOSS_AGING;
        float HEALTHLOSS_BRAINUSE;
        float HEALTHLOSS_BUMP;
        float HEALTHLOSS_SPIKE_EXT;
        float HEALTHLOSS_BADAIR;
        float HEALTHLOSS_NOOXYGEN;
        float HEALTHLOSS_ASSEX;
        float HEALTHLOSS_JAWSNAP;

        float STOMACH_EFF;

        float FOODINTAKE;
        float FOODDECAY;
        float FOODGROWTH;
        float FOODWASTE;
        int   FOODADDFREQ;
        float FOODSPREAD;
        int   FOODRANGE;

        float FRUITINTAKE;
        float FRUITDECAY;
        float FRUITWASTE;
        int   FRUITADDFREQ;
        float FRUITREQUIRE;

        float MEATINTAKE;
        float MEATDECAY;
        float MEATWASTE;
        float MEATVALUE;

        int   HAZARDFREQ;
        float HAZARDDECAY;
        float HAZARDDEPOSIT;
        float HAZARDDAMAGE;

    private:
        void writeReport();

        void reproduce(int ai, int bi);

        void cellsRandomFill(int layer, float amount, int number);
        void cellsLandMasses();
        void
        findStats();

        std::vector <std::string> tips;

        bool   CLOSED;
        bool   DEBUG;
        bool   AUTOSELECT;
        int    SELECTION;
        stats_t stats;
    };
}

#endif //EVOLVE_WORLD_HPP