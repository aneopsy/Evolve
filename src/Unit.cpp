/* ************************************************************************** *
 *   Unit.cpp
 *                                                       :::      ::::::::    *
 *                                                     :+: :+:    :+:    :+:  *
 *                                                    +:+   +:+   +:+    +:+  *
 *   By: THEIS Paul                                  +#++:++#++:  +#++:++#+   *
 *                                                   +#+     +#+  +#+         *
 *   Created: 11/04/17                               #+#     #+#  #+#         *
 *   Updated: 11/04/17                               #+#     #+#  #+#         *
 *   Project: Evolve
 * ************************************************************************** */

#include <Utils.hpp>
#include "Unit.hpp"

Evolve::Unit::Unit(t_brain settings, float MEANRADIUS, float REPRATE,
                   float MUTARATE1, float MUTARATE2) :
        brain(settings) {
  id       = 0;
  pos      = Vector2f(randf(0, conf::WIDTH), randf(0, conf::HEIGHT));
  angle    = randf(-M_PI, M_PI);
  health   = 1 + randf(0.5, 0.75);
  age      = 0;
  MUTRATE1 = MUTARATE1 +
             MEANRADIUS * randf(-conf::METAMUTRATE1, conf::METAMUTRATE1) * 3;
  MUTRATE2 = MUTARATE2 +
             MEANRADIUS * randf(-conf::METAMUTRATE2, conf::METAMUTRATE2) * 15;
  radius   = randf(MEANRADIUS * 0.2, MEANRADIUS * 2.2);

  near                   = false;
  freshkill              = 0;
  species                = randi(-conf::NUMBOTS * 200, conf::NUMBOTS * 200);
  gencount               = 0;
  repcounter             = REPRATE;
  numbabies              = randi(1, 6);
  temperature_preference = cap(
          randn(2.0 * abs(pos.y / conf::HEIGHT - 0.5), 0.05));
  lungs                  = randf(0, 1);
  metabolism             = randf(0.25, 0.75);

  float herb, carn, frui;
  int   overtype         = randi(0, Stomach::FOOD_TYPES - 1);
  herb = randf(0, 1);
  carn = randf(0, 1);
  frui = randf(0, 1);
  stomach[Stomach::PLANT] = herb;
  stomach[Stomach::MEAT]  = carn;
  stomach[Stomach::FRUIT] = frui;
  if (herb + carn + frui < 1) {
    stomach[overtype] += 1 - herb - carn - frui;
    cap(stomach[overtype]);
  }

//  eye_see_unit_mod= randf(0.6, 4);
  eyefov.resize(NUMEYES, 0);
  eyedir.resize(NUMEYES, 0);
  for (int i = 0; i < NUMEYES; i++) {
    eyefov[i] = randf(0.3, 2);
    eyedir[i] = randf(0, 2 * M_PI);
  }

  //ears
  hear_mod = randf(0.1, 3);
  eardir.resize(NUMEARS, 0);
  hearlow.resize(NUMEARS, 0);
  hearhigh.resize(NUMEARS, 0);
  for (int i = 0; i < NUMEARS; i++) {
    eardir[i] = randf(0, 2 * M_PI);
    float temp1 = randf(0, 1);
    float temp2 = randf(0, 1);
    if (temp1 > temp2) {
      hearlow[i]  = temp2;
      hearhigh[i] = temp1;
    } else {
      hearlow[i]  = temp1;
      hearhigh[i] = temp2;
    }
  }
  clockf1   = randf(5, 100);
  clockf2   = randf(5, 100);
  clockf3   = 5;
  blood_mod = randf(0.1, 3);
  smell_mod = randf(0.1, 3);

  in.resize(Input::INPUT_SIZE, 0);
  out.resize(Output::OUTPUT_SIZE, 0);
  brainact = 0;

  w1          = 0;
  w2          = 0;
  strength    = randf(0.01, 1);
  boost       = false;
  jump        = 0;
  red         = 0.5;
  gre         = 0.5;
  blu         = 0.5;
  volume      = 1;
  tone        = 0.5;
  give        = 0;
  spikeLength = 0;
  jawPosition = 0;
  jawOldPos   = 0;
  grabID      = -1;
  grabbing    = 0;
  sexproject  = false;

  //stats
  hybrid    = false;
  death     = "Killed by Unknown Factor!";
  children  = 0;
  killed    = 0;
  hits      = 0;
  indicator = 0;
  ir        = 0;
  ig        = 0;
  ib        = 0;
  jawrend   = 0;
  dfood     = 0;
}

void Evolve::Unit::printSelf() {
  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("AGENT, ID: %i\n", id);
  printf("Species # = %i\n", species);
  printf("Metabolism = %f\n", metabolism);
  printf("Lungs = %f\n", lungs);
  printf("MUTRATE1 = %f, MUTRATE2 = %f\n", MUTRATE1, MUTRATE2);
  printf("children= %i\n", children);
  printf("killed= %i\n", killed);
  if (isHerbivore()) printf("Herbivore\n");
  if (isCarnivore()) printf("Carnivore\n");
  if (isFrugivore()) printf("Frugivore\n");
  for (int i = 0; i < (int) mutations.size(); i++) {
    std::cout << mutations[i];
  }
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void Evolve::Unit::initEvent(float size, float r, float g, float b) {
  indicator = size;
  ir        = r;
  ig        = g;
  ib        = b;
}

void Evolve::Unit::setActivity() {
  brainact = brain.getActivity();
}

void Evolve::Unit::tick() {
  brain.tick(in, out);
}

Evolve::Unit Evolve::Unit::reproduce(Evolve::Unit that, float REPRATE) {
  float MR= randf(std::min(this->MUTRATE1,that.MUTRATE1),std::max(this->MUTRATE1,that.MUTRATE1));
  float MR2= randf(std::min(this->MUTRATE2,that.MUTRATE2), std::max(this->MUTRATE2,that.MUTRATE2));

  Unit a2(this->brain.getBrain(), 0, REPRATE, MR, MR2);


  //spawn the baby somewhere closeby behind the mother
  //we want to spawn behind so that units dont accidentally kill their young right away
  //note that this relies on bots actally driving forward, not backward. Well let natural selection choose who lives and who dies
  Vector2f fb(this->radius*2.5,0);
  fb.rotate(this->angle+M_PI+this->numbabies*randf(-0.4,0.4));
  a2.pos= this->pos + fb;
  a2.borderRectify();

  //basic trait inheritance
  a2.gencount= std::max(this->gencount+1,that.gencount+1);
  a2.numbabies= randf(0,1)<0.5 ? this->numbabies : that.numbabies;
  a2.metabolism= randf(0,1)<0.5 ? this->metabolism : that.metabolism;
  a2.stomach[Stomach::PLANT]= randf(0,1)<0.5 ? this->stomach[Stomach::PLANT]: that.stomach[Stomach::PLANT];
  a2.stomach[Stomach::MEAT]= randf(0,1)<0.5 ? this->stomach[Stomach::MEAT]: that.stomach[Stomach::MEAT];
  a2.stomach[Stomach::FRUIT]= randf(0,1)<0.5 ? this->stomach[Stomach::FRUIT]: that.stomach[Stomach::FRUIT];
  a2.species= randf(0,1)<0.5 ? this->species : that.species;
  a2.radius= randf(0,1)<0.5 ? this->radius : that.radius;

  a2.MUTRATE1= randf(0,1)<0.5 ? this->MUTRATE1 : that.MUTRATE1;
  a2.MUTRATE2= randf(0,1)<0.5 ? this->MUTRATE2 : that.MUTRATE2;
  a2.clockf1= randf(0,1)<0.5 ? this->clockf1 : that.clockf1;
  a2.clockf2= randf(0,1)<0.5 ? this->clockf2 : that.clockf2;

  a2.smell_mod= randf(0,1)<0.5 ? this->smell_mod : that.smell_mod;
  a2.hear_mod= randf(0,1)<0.5 ? this->hear_mod : that.hear_mod;
  a2.eye_see_unit_mod= randf(0,1)<0.5 ? this->eye_see_unit_mod : that.eye_see_unit_mod;
//	a2.eye_see_cell_mod= randf(0,1)<0.5 ? this->eye_see_cell_mod : that.eye_see_cell_mod;
  a2.blood_mod= randf(0,1)<0.5 ? this->blood_mod : that.blood_mod;

  a2.temperature_preference= randf(0,1)<0.5 ? this->temperature_preference : that.temperature_preference;
  a2.lungs= randf(0,1)<0.5 ? this->lungs : that.lungs;

  a2.eardir= randf(0,1)<0.5 ? this->eardir : that.eardir;
  a2.hearlow= randf(0,1)<0.5 ? this->hearlow : that.hearlow;
  a2.hearhigh= randf(0,1)<0.5 ? this->hearhigh : that.hearhigh;

  a2.eyefov= randf(0,1)<0.5 ? this->eyefov : that.eyefov;
  a2.eyedir= randf(0,1)<0.5 ? this->eyedir : that.eyedir;

  a2.strength= randf(0,1)<0.5 ? this->strength : that.strength;


  //---MUTATIONS---//
  if (randf(0,1)<MR/10) a2.numbabies= (int) (randn(a2.numbabies, MR2*40));
  if (a2.numbabies<1) a2.numbabies= 1; //technically, 0 babies is perfectly logical, but in this sim it is pointless, so it's disallowed
  if (randf(0,1)<MR) a2.metabolism= cap(randn(a2.metabolism, MR2*3));
  if (randf(0,1)<MR*5) a2.stomach[Stomach::PLANT]= cap(randn(a2.stomach[Stomach::PLANT], MR2*10)); //*15 was a bit big
  if (randf(0,1)<MR*5) a2.stomach[Stomach::MEAT]= cap(randn(a2.stomach[Stomach::MEAT], MR2*10));
  if (randf(0,1)<MR*5) a2.stomach[Stomach::FRUIT]= cap(randn(a2.stomach[Stomach::FRUIT], MR2*10));
  if (randf(0,1)<MR*20) a2.species+= (int) (randn(0, MR2*120));
  if (randf(0,1)<MR*5) a2.radius= randn(a2.radius, MR2*10);
  if (a2.radius<1) a2.radius= 1;

  if (randf(0,1)<MR) a2.MUTRATE1= abs(randn(a2.MUTRATE1, conf::METAMUTRATE1));
  if (randf(0,1)<MR) a2.MUTRATE2= abs(randn(a2.MUTRATE2, conf::METAMUTRATE2));

  if (randf(0,1)<MR) a2.clockf1= randn(a2.clockf1, MR2/2);
  if (a2.clockf1<2) a2.clockf1= 2;
  if (randf(0,1)<MR) a2.clockf2= randn(a2.clockf2, MR2/2);
  if (a2.clockf2<2) a2.clockf2= 2;

  if (randf(0,1)<MR) a2.smell_mod= randn(a2.smell_mod, MR2);
  if (randf(0,1)<MR) a2.hear_mod= randn(a2.hear_mod, MR2);
  if (randf(0,1)<MR) a2.eye_see_unit_mod= randn(a2.eye_see_unit_mod, MR2);
//	if (randf(0,1)<MR) a2.eye_see_cell_mod= randn(a2.eye_see_cell_mod, MR2);
  if (randf(0,1)<MR) a2.blood_mod= randn(a2.blood_mod, MR2);

  if (randf(0,1)<MR*2) a2.temperature_preference= cap(randn(a2.temperature_preference, MR2*2));
  if (randf(0,1)<MR*2) a2.lungs= cap(randn(a2.lungs, MR2*2));

  for(int i=0;i<NUMEARS;i++){
    if (randf(0,1)<MR/60) {
      int origear= randi(0,NUMEARS);
//			if(randf(0,1)<MR*5) a2.eardir[i]= a2.eardir[origear];
      if(randf(0,1)<MR*5) a2.hearlow[i]= a2.hearlow[origear];
      if(randf(0,1)<MR*5) a2.hearhigh[i]= a2.hearhigh[origear];
    }

    if(randf(0,1)<MR*2) a2.eardir[i] = randn(a2.eardir[i], MR2*5);
    if(a2.eardir[i]<0) a2.eardir[i] = 0;
    if(a2.eardir[i]>2*M_PI) a2.eardir[i] = 2*M_PI;
    if(randf(0,1)<MR) a2.hearlow[i]= randn(a2.hearlow[i], MR2);
    if(randf(0,1)<MR) a2.hearhigh[i]= randn(a2.hearhigh[i], MR2);

    if (a2.hearlow[i]>a2.hearhigh[i]) {
      float temp= a2.hearlow[i];
      a2.hearlow[i]= a2.hearhigh[i];
      a2.hearhigh[i]= temp;
    }
  }

  for(int i=0;i<eyedir.size();i++){
    if (randf(0,1)<MR/100) {
      //LOW-CHANCE COPY EVENT (multiply by NUMEYES for real probability)
      int origeye= randi(0,eyedir.size());
      if(randf(0,1)<MR*5) a2.eyedir[i]= a2.eyedir[origeye];
      if(randf(0,1)<MR*5) a2.eyefov[i]= a2.eyefov[origeye];
    }

    if(randf(0,1)<MR) a2.eyefov[i] = randn(a2.eyefov[i], MR2/2);
    if(a2.eyefov[i]<0) a2.eyefov[i] = 0;
    if(a2.eyefov[i]>M_PI) a2.eyefov[i] = M_PI; //eyes cannot wrap around bot

    if(randf(0,1)<MR*2) a2.eyedir[i] = randn(a2.eyedir[i], MR2*5);
    if(a2.eyedir[i]<0) a2.eyedir[i] = 0;
    if(a2.eyedir[i]>2*M_PI) a2.eyedir[i] = 2*M_PI;
    //not going to loop coordinates; 0,2pi is bots front again, so it provides a good point to "bounce" off of
  }

  //create brain here
  a2.brain= this->brain.crossover(that.brain);
  a2.brain.initMutate(MR,MR2);

  if (randf(0,1)<MR*2) a2.strength= cap(randn(a2.strength, MR2));

  a2.initEvent(20,0.8,0.8,0.8); //grey event means we were just born! Welcome!

  return a2;
}

void Evolve::Unit::setHerbivore() {
  this->stomach[Stomach::PLANT] = randf(0.7, 1);
  this->stomach[Stomach::MEAT]  = randf(0, 0.3);
  this->stomach[Stomach::FRUIT] = randf(0, 0.3);
}

void Evolve::Unit::setCarnivore() {
  this->stomach[Stomach::PLANT] = randf(0, 0.3);
  this->stomach[Stomach::MEAT]  = randf(0.7, 1);
  this->stomach[Stomach::FRUIT] = randf(0, 0.3);
}

void Evolve::Unit::setFrugivore() {
  this->stomach[Stomach::PLANT] = randf(0, 0.3);
  this->stomach[Stomach::MEAT]  = randf(0, 0.3);
  this->stomach[Stomach::FRUIT] = randf(0.7, 1);
}

void Evolve::Unit::setPos(float x, float y) {
  this->pos.x = x;
  this->pos.y = y;
  this->borderRectify();
}

void Evolve::Unit::borderRectify() {
  if (this->pos.x < 0)
    this->pos.x = this->pos.x + conf::WIDTH * ((int) ((0 - this->pos.x) /
                                                      conf::WIDTH) + 1);
  if (this->pos.x >= conf::WIDTH)
    this->pos.x = this->pos.x - conf::WIDTH * ((int) ((this->pos.x -
                                                       conf::WIDTH) /
                                                      conf::WIDTH) + 1);
  if (this->pos.y < 0)
    this->pos.y = this->pos.y + conf::HEIGHT * ((int) ((0 - this->pos.y) /
                                                       conf::HEIGHT) + 1);
  if (this->pos.y >= conf::HEIGHT)
    this->pos.y = this->pos.y - conf::HEIGHT * ((int) ((this->pos.y -
                                                        conf::HEIGHT) /
                                                       conf::HEIGHT) + 1);
}

bool Evolve::Unit::isHerbivore() const {
  if (health > 0 && stomach[Stomach::PLANT] >= stomach[Stomach::MEAT] &&
      stomach[Stomach::PLANT] >= stomach[Stomach::FRUIT])
    return true;
  return false;
}

bool Evolve::Unit::isCarnivore() const {
  if (health > 0 && stomach[Stomach::MEAT] >= stomach[Stomach::PLANT] &&
      stomach[Stomach::MEAT] >= stomach[Stomach::FRUIT])
    return true;
  return false;
}

bool Evolve::Unit::isFrugivore() const {
  if (health > 0 && stomach[Stomach::FRUIT] >= stomach[Stomach::PLANT] &&
      stomach[Stomach::FRUIT] >= stomach[Stomach::MEAT])
    return true;
  return false;
}

bool Evolve::Unit::isTerran() const {
  if (lungs >= 0.5) return true;
  return false;
}

bool Evolve::Unit::isAquatic() const {
  if (lungs < 0.5) return true;
  return false;
}

bool Evolve::Unit::isSpikey(float SPIKELENGTH) const {
  if (spikeLength * SPIKELENGTH >= radius) return true;
  return false;
}

void Evolve::Unit::writeIfKilled(const char *cause) {
  if (this->health < 0) {
    this->death     = cause;
    this->health    = 0;
    this->indicator = -1;
  }
}