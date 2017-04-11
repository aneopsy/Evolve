/* ************************************************************************** *
 *   World.cpp
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
#include "World.hpp"

using namespace std;

Evolve::World::World() :
        CLOSED(false),
        DEBUG(true),
        SELECTION(-1),
        pcontrol(false),
        pright(0),
        pleft(0),
        pinput1(0) {
  reset();
  spawn();
  printf("World Made!\n");
}

void Evolve::World::reset() {
  addEvent("World Started!");
}

void Evolve::World::spawn() {}

void Evolve::World::cellsLandMasses() {}

void Evolve::World::readConfig() {}

void Evolve::World::writeConfig() {}

void Evolve::World::update() {}

void Evolve::World::setInputs() {}

void Evolve::World::brainsTick() {}

void Evolve::World::processOutputs() {}

void Evolve::World::processInteractions() {}

void Evolve::World::healthTick() {}

void Evolve::World::setSelection(int type) { (void) type; }

void Evolve::World::setSelectedUnit(int idx) { (void) idx; }

int Evolve::World::getSelectedUnit() const {}

int Evolve::World::getClosestRelative(int idx) const { (void) idx; }

int Evolve::World::getSelection() const { return SELECTION; }

void Evolve::World::selectedHeal() {}

void Evolve::World::selectedKill() {}

void Evolve::World::selectedBabys() {}

void Evolve::World::selectedMutate() {}

void Evolve::World::getFollowLocation(float &xi, float &yi) {
  (void) xi;
  (void) yi;
}

void Evolve::World::draw(IView *view, int layer) {
  (void) layer;
  view->drawData();
  view->drawStatic();
}

void Evolve::World::addUnits(int num, int set_stomach, float nx, float ny,
                             bool set_lungs) {}

void Evolve::World::reproduce(int i1, int i2) {}

void Evolve::World::writeReport() {}

void Evolve::World::setClosed(bool close) {
  CLOSED = close;
}

bool Evolve::World::isClosed() const {
  return CLOSED;
}

int Evolve::World::getHerbivores() const {
  return stats.STATherbivores;
}

int Evolve::World::getFrugivores() const {
  return stats.STATfrugivores;
}

int Evolve::World::getCarnivores() const {
  return stats.STATcarnivores;
}

int Evolve::World::getLungLand() const {
  return stats.STATterrans;
}

int Evolve::World::getLungWater() const {
  return stats.STATaquatic;
}

int Evolve::World::getUnits() const {
  return units.size();
}

int Evolve::World::getHybrids() const {
  return stats.STAThybrids;
}

int Evolve::World::getSpiked() const {
  return stats.STATspiked;
}

int Evolve::World::getDead() const {
  return stats.STATdead;
}

int Evolve::World::getAlive() const {
  return stats.STATalive;
}

int Evolve::World::getFood() const //count plant cells with 75% max or more
{
  return stats.STATplants;
}

int Evolve::World::getFruit() const //count fruit cells with 50% max or more
{
  return stats.STATfruits;
}

int Evolve::World::getMeat() const //count meat cells with 50% max or more
{
  return stats.STATmeats;
}

int Evolve::World::getHazards() const //count hazard cells with 50% max or more
{
  return stats.STAThazards;
}

float Evolve::World::getFoodSupp() const {
  return stats.STATallplant * FOODINTAKE / 2 *
         10;// Stat*INTAKE/maxhealth*statfrequency
}

float Evolve::World::getFruitSupp() const {
  return stats.STATallfruit * FRUITINTAKE / 2 * 10;
}

float Evolve::World::getMeatSupp() const {
  return stats.STATallmeat * MEATINTAKE / 2 * 10;
}

float Evolve::World::getHazardSupp() const {
  return stats.STATallhazard * HAZARDDAMAGE / 2 * 10;
}

void Evolve::World::findStats() {}

/*
int World::getSelected() const //returns the ID of the selected unit
{
	int id= -1;
	for (int i=0; i<(int)units.size(); i++) {
		if (units[i].selectflag) id= i;
	}
	return id;
}*/

int Evolve::World::epoch() const {
  return current_epoch;
}

void Evolve::World::setControl(bool state) {
  //reset left and right wheel controls
  pleft  = 0;
  pright = 0;

  pcontrol = state;
}

bool Evolve::World::isAutoselecting() const {
  return AUTOSELECT;
}

void Evolve::World::setAutoselect(bool state) {
  AUTOSELECT = state;
}

void Evolve::World::cellsRandomFill(int layer, float amount, int number) {
  for (int i = 0; i < number; i++) {
    int cx = randi(0, CW);
    int cy = randi(0, CH);
    cells[layer][cx][cy] = amount;
  }
}

void Evolve::World::setDebug(bool state) {
  DEBUG = state;
}

bool Evolve::World::isDebug() const {
  return DEBUG;
}

void Evolve::World::addEvent(const char *text) {
/*======LIST OF CURRENT EVENT LABELS======//
"World Started!"						world.cpp	~ln 155									26
[Tips]									world.cpp	~ln 620 & 642 Note: all tips are entered into a vector at init, ~ln 111
"The Selected unit Was Mutated!"		world.cpp	~ln 725		Note: for some reason, this is the longest possible messege
"The Selected unit Died!"				world.cpp	~ln 790		Note: units killed manually will not be reported to the event log	12
"The Selected unit Killed Another!"	world.cpp	~ln 1468 & 1504
"The Selected unit Stabbed Another!"	world.cpp	~ln 1472
"The Selected unit Bit Another!"		world.cpp	~ln 1508
"No More Living Relatives!"				world.cpp	~ln 1656
"Auto-saved World!"						glview.cpp	~ln 663


*/
  std::pair<const char *, int> data;
  data.first  = text;
  data.second = 2;//conf::EVENTS_HALFLIFE;
  events.push_back(data);
}

bool
Evolve::World::processMouse(int button, int state, int x, int y, float scale) {
  if (button == 0 && state == 1) {
    float mind = 1e10;
    int   mini = -1;
    float d;

//    for (int i = 0; i < (int) units.size(); i++) {
//      d = pow(x - units[i].pos.x, 2) + pow(y - units[i].pos.y, 2);
//      if (d < mind) {
//        mind = d;
//        mini = i;
//      }
//    }
    if (mind < 3500 / scale) {
      //toggle selection of this agent
      setSelectedUnit(mini);
      return true;
    } else return false;
  } else return false;
}