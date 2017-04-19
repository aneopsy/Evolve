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
        _closed(false),
        _debug(true),
        _selection(-1),
        pcontrol(false),
        pright(0),
        pleft(0),
        pinput1(0) {

  MINFOOD          = conf::MINFOOD;
  INITFOODDENSITY  = conf::INITFOODDENSITY;
  INITFRUITDENSITY = conf::INITFRUITDENSITY;

  NUMBOTS     = conf::NUMBOTS;
  ENOUGHBOTS  = conf::ENOUGHBOTS;
  TOOMANYBOTS = conf::TOOMANYBOTS;

  REPORTS_PER_EPOCH = conf::REPORTS_PER_EPOCH;
  FRAMES_PER_EPOCH  = conf::FRAMES_PER_EPOCH;
  FRAMES_PER_DAY    = conf::FRAMES_PER_DAY;

  CONTINENTS       = conf::CONTINENTS;
  OCEANPERCENT     = conf::OCEANPERCENT;
  MOONLIT          = conf::MOONLIT;
  GRAVITYACCEL     = conf::GRAVITYACCEL;
  REACTPOWER       = conf::REACTPOWER;
  SPIKEMULT        = conf::SPIKEMULT;
  BRAINSIZE        = conf::BRAINSIZE;
  BOTSPEED         = conf::BOTSPEED;
  BOOSTSIZEMULT    = conf::BOOSTSIZEMULT;
  SOUNDPITCHRANGE  = conf::SOUNDPITCHRANGE;
  FOODTRANSFER     = conf::FOODTRANSFER;
  MEANRADIUS       = conf::MEANRADIUS;
  SPIKESPEED       = conf::SPIKESPEED;
  FRESHKILLTIME    = conf::FRESHKILLTIME;
  TENDERAGE        = conf::TENDERAGE;
  MINMOMHEALTH     = conf::MINMOMHEALTH;
  FUN              = 0;
  REPRATE          = conf::REPRATE;
  OVERHEAL_REPFILL = conf::OVERHEAL_REPFILL;
//    LEARNRATE= conf::LEARNRATE;
  MAXDEVIATION     = conf::MAXDEVIATION;
  MUTCHANCE        = conf::MUTCHANCE;
  MUTSIZE          = conf::MUTSIZE;
  MAXAGE           = conf::MAXAGE;

  DIST                  = conf::DIST;
  SPIKELENGTH           = conf::SPIKELENGTH;
  TOOCLOSE              = conf::TOOCLOSE;
  FOOD_SHARING_DISTANCE = conf::FOOD_SHARING_DISTANCE;
  SEXTING_DISTANCE      = conf::SEXTING_DISTANCE;
  GRABBING_DISTANCE     = conf::GRABBING_DISTANCE;

  HEALTHLOSS_WHEELS    = conf::HEALTHLOSS_WHEELS;
  HEALTHLOSS_BOOSTMULT = conf::HEALTHLOSS_BOOSTMULT;
  HEALTHLOSS_BADTEMP   = conf::HEALTHLOSS_BADTEMP;
  HEALTHLOSS_AGING     = conf::HEALTHLOSS_AGING;
  HEALTHLOSS_BRAINUSE  = conf::HEALTHLOSS_BRAINUSE;
  HEALTHLOSS_BUMP      = conf::HEALTHLOSS_BUMP;
  HEALTHLOSS_SPIKE_EXT = conf::HEALTHLOSS_SPIKE_EXT;
  HEALTHLOSS_BADAIR    = conf::HEALTHLOSS_BADAIR;
  HEALTHLOSS_NOOXYGEN  = conf::HEALTHLOSS_NOOXYGEN;
  HEALTHLOSS_ASSEX     = conf::HEALTHLOSS_ASSEX;
  HEALTHLOSS_JAWSNAP   = conf::HEALTHLOSS_JAWSNAP;

  STOMACH_EFF = conf::STOMACH_EFF;

  FOODINTAKE  = conf::FOODINTAKE;
  FOODDECAY   = conf::FOODDECAY;
  FOODGROWTH  = conf::FOODGROWTH;
  FOODWASTE   = conf::FOODWASTE;
  FOODADDFREQ = conf::FOODADDFREQ;
  FOODSPREAD  = conf::FOODSPREAD;
  FOODRANGE   = conf::FOODRANGE;

  FRUITINTAKE  = conf::FRUITINTAKE;
  FRUITDECAY   = conf::FRUITDECAY;
  FRUITWASTE   = conf::FRUITWASTE;
  FRUITADDFREQ = conf::FRUITADDFREQ;
  FRUITREQUIRE = conf::FRUITREQUIRE;

  MEATINTAKE = conf::MEATINTAKE;
  MEATDECAY  = conf::MEATDECAY;
  MEATWASTE  = conf::MEATWASTE;
  MEATVALUE  = conf::MEATVALUE;

  HAZARDFREQ    = conf::HAZARDFREQ;
  HAZARDDECAY   = conf::HAZARDDECAY;
  HAZARDDEPOSIT = conf::HAZARDDEPOSIT;
  HAZARDDAMAGE  = conf::HAZARDDAMAGE;

  reset();
  spawn();
  printf("World Made!\n");
}

void Evolve::World::reset() {
  current_epoch = 0;
  modcounter    = 0;
  idcounter     = 0;

  //try loading constants config
  readConfig();

  //tidy up constants
  INITFOOD  = (int) (INITFOODDENSITY * conf::WIDTH * conf::HEIGHT);
  INITFRUIT = (int) (INITFRUITDENSITY * conf::WIDTH * conf::HEIGHT);
  if (_debug) printf("INITFOOD/INITFRUIT: %i, %i\n", INITFOOD, INITFRUIT);
  CW = conf::WIDTH / conf::CZ;
  CH = conf::HEIGHT /
       conf::CZ; //note: should add custom variables from loaded savegames here possibly
  if (BRAINSIZE < (Input::INPUT_SIZE + Output::OUTPUT_SIZE)) {
    printf("your BRAINSIZE config value is too small. Please increase\n");
    BRAINSIZE =
            Input::INPUT_SIZE + Output::OUTPUT_SIZE; //avoid too-small brains
  }

  printf("sanitizing units.\n");
  units.clear();

  //handle layers
  for (int cx = 0; cx < (int) CW; cx++) {
    for (int cy = 0; cy < (int) CH; cy++) {
      for (int l = 0; l < Layer::LAYERS; l++) {
        cells[l][cx][cy] = 0;
      }
//			cells[TEMPLAYER][cx][cy]= 2.0*abs((float)cy/CH - 0.5); [old temperature indicating code]
    }
  }

  //open report file; null it up if it exists
  FILE *fr = fopen("report.txt", "w");
  fclose(fr);

  ptr = 0;

  addEvent("World Started!");
}

void Evolve::World::spawn() {
  findStats();
  printf("growing food.\n");
  while (getFood() < INITFOOD) {
    findStats();
    int rx = randi(0, CW);
    int ry = randi(0, CH);
    if (cells[Layer::PLANTS][rx][ry] != 0)
      continue;
    cells[Layer::PLANTS][rx][ry] = conf::FOODMAX;
    if (getFruit() < INITFRUIT) {
      cells[Layer::FRUITS][rx][ry] = conf::FRUITMAX;
    }
  }
  cellsLandMasses();
  printf("programming bots.\n");
  addUnits(30, Stomach::PLANT); //TODO: CHANGE
  findStats();
}

void Evolve::World::cellsLandMasses() {
  //creates land masses for the layer given
  int leftcount = CW * CH;

  printf("clearing land.\n");
  for (int i = 0; i < CW; i++) {
    for (int j = 0; j < CH; j++) {
      cells[Layer::LAND][i][j] = -1; //"null" all cells
    }
  }

  for (int i = 0;
       i < 1.5 * (1 - pow(OCEANPERCENT, 6)) * (CONTINENTS + 1); i++) {
    //spawn init continents (land= 1)
    int cx = randi(0, CW);
    int cy = randi(0, CH);
    cells[Layer::LAND][cx][cy] = 1;
  }
  for (int i = 0; i < 1.5 * (sqrt((float) CW * CH) / 1000 *
                             pow((float) 2.5, 5 * OCEANPERCENT) *
                             (CONTINENTS + 1)) - 1; i++) {
    //spawn oceans (water= 0)
    int cx                     = randi(0, CW);
    int cy                     = randi(0, CH);
    cells[Layer::LAND][cx][cy] = 0;
  }

  printf("moving tectonic plates.\n");
  while (leftcount != 0) {
    for (int i = 0; i < CW; i++) {
      for (int j = 0; j < CH; j++) {
        //land spread
        if (cells[Layer::LAND][i][j] == 1) {
          int ox = randi(i - 1, i + 2);
          int oy = randi(j - 1, j + 2);
          if (ox < 0) ox += CW;
          if (ox > CW - 1) ox -= CW;
          if (oy < 0) oy += CH;
          if (oy > CH - 1) oy -= CH;
          if (cells[Layer::LAND][ox][oy] == -1 && randf(0, 1) < 0.9)
            cells[Layer::LAND][ox][oy] = 1;
        }

        //water spread
        if (cells[Layer::LAND][i][j] == 0) {
          int ox = randi(i - 1, i + 2);
          int oy = randi(j - 1, j + 2);
          if (ox < 0) ox += CW;
          if (ox > CW - 1) ox -= CW;
          if (oy < 0) oy += CH;
          if (oy > CH - 1) oy -= CH;
          if (cells[Layer::LAND][ox][oy] == -1 && randf(0, 1) < 0.9)
            cells[Layer::LAND][ox][oy] = 0;
        }
      }
    }

    leftcount = 0;
    for (int i = 0; i < CW; i++) {
      for (int j = 0; j < CH; j++) {
        if (cells[Layer::LAND][i][j] == -1) {
          leftcount++;
        }
      }
    }
  }
}

void Evolve::World::readConfig() {
  //get world constants from config file, settings.cfg
  char  line[64], *pos;
  char  var[16];
  char  dataval[16];
  int   i; //integer buffer
  float f; //float buffer

  //first check version number
  FILE *cf = fopen("settings.cfg", "r");
  if (cf) {
    printf("tweaking constants.\n");
    while (!feof(cf)) {
      fgets(line, sizeof(line), cf);
      pos = strtok(line, "\n");
      sscanf(line, "%s%s", var, dataval);

      if (strcmp(var, "V=") == 0) { //strcmp= 0 when the arguements equal
        sscanf(dataval, "%f", &f);
        if (f != conf::VERSION) {
          fclose(cf);
          printf("CONFIG FILE VERSION MISMATCH!\n EXPECTED 'V= %3.2f'\n",
                 conf::VERSION);
          printf("Exit now to save your custom settings, otherwise hit enter to overwrite. . .");
          cin.get();
          writeConfig();
          readConfig();
          break;
        }
      } else if (strcmp(var, "MINFOOD=") == 0) {
        sscanf(dataval, "%i", &i);
        MINFOOD = i;
      } else if (strcmp(var, "INITFOODDENSITY=") == 0) {
        sscanf(dataval, "%f", &f);
        INITFOODDENSITY = f;
      } else if (strcmp(var, "INITFOOD=") == 0) {
        sscanf(dataval, "%i", &i);
        INITFOOD = i;
      } else if (strcmp(var, "INITFRUITDENSITY=") == 0) {
        sscanf(dataval, "%f", &f);
        INITFRUITDENSITY = f;
      } else if (strcmp(var, "INITFRUIT=") == 0) {
        sscanf(dataval, "%i", &i);
        INITFRUIT = i;
      } else if (strcmp(var, "NUMBOTS=") == 0) {
        sscanf(dataval, "%i", &i);
        NUMBOTS = i;
      } else if (strcmp(var, "ENOUGHBOTS=") == 0) {
        sscanf(dataval, "%i", &i);
        ENOUGHBOTS = i;
      } else if (strcmp(var, "TOOMANYBOTS=") == 0) {
        sscanf(dataval, "%i", &i);
        TOOMANYBOTS = i;
      } else if (strcmp(var, "REPORTS_PER_EPOCH=") == 0) {
        sscanf(dataval, "%i", &i);
        REPORTS_PER_EPOCH = i;
      } else if (strcmp(var, "FRAMES_PER_EPOCH=") == 0) {
        sscanf(dataval, "%i", &i);
        FRAMES_PER_EPOCH = i;
      } else if (strcmp(var, "FRAMES_PER_DAY=") == 0) {
        sscanf(dataval, "%i", &i);
        FRAMES_PER_DAY = i;
      } else if (strcmp(var, "CONTINENTS=") == 0) {
        sscanf(dataval, "%i", &i);
        CONTINENTS = i;
      } else if (strcmp(var, "OCEANPERCENT=") == 0) {
        sscanf(dataval, "%f", &f);
        OCEANPERCENT = f;
      } else if (strcmp(var, "MOONLIT=") == 0) {
        sscanf(dataval, "%i", &i);
        MOONLIT = (bool) i;
      } else if (strcmp(var, "GRAVITYACCEL=") == 0) {
        sscanf(dataval, "%f", &f);
        GRAVITYACCEL = f;
      } else if (strcmp(var, "REACTPOWER=") == 0) {
        sscanf(dataval, "%f", &f);
        REACTPOWER = f;
      } else if (strcmp(var, "SPIKEMULT=") == 0) {
        sscanf(dataval, "%f", &f);
        SPIKEMULT = f;
      } else if (strcmp(var, "BRAINSIZE=") == 0) {
        sscanf(dataval, "%i", &i);
        BRAINSIZE = i;
      } else if (strcmp(var, "BOTSPEED=") == 0) {
        sscanf(dataval, "%f", &f);
        BOTSPEED = f;
      } else if (strcmp(var, "BOOSTSIZEMULT=") == 0) {
        sscanf(dataval, "%f", &f);
        BOOSTSIZEMULT = f;
      } else if (strcmp(var, "SOUNDPITCHRANGE=") == 0) {
        sscanf(dataval, "%f", &f);
        SOUNDPITCHRANGE = f;
      } else if (strcmp(var, "FOODTRANSFER=") == 0) {
        sscanf(dataval, "%f", &f);
        FOODTRANSFER = f;
      } else if (strcmp(var, "MEANRADIUS=") == 0) {
        sscanf(dataval, "%f", &f);
        MEANRADIUS = f;
      } else if (strcmp(var, "SPIKESPEED=") == 0) {
        sscanf(dataval, "%f", &f);
        SPIKESPEED = f;
      } else if (strcmp(var, "FRESHKILLTIME=") == 0) {
        sscanf(dataval, "%i", &i);
        FRESHKILLTIME = i;
      } else if (strcmp(var, "TENDERAGE=") == 0) {
        sscanf(dataval, "%i", &i);
        TENDERAGE = i;
      } else if (strcmp(var, "MINMOMHEALTH=") == 0) {
        sscanf(dataval, "%f", &f);
        MINMOMHEALTH = f;
      } else if (strcmp(var, "FUN=") == 0) {
        sscanf(dataval, "%i", &i);
        FUN = (bool) i;
      } else if (strcmp(var, "REPRATE=") == 0) {
        sscanf(dataval, "%f", &f);
        REPRATE = f;
      } else if (strcmp(var, "OVERHEAL_REPFILL=") == 0) {
        sscanf(dataval, "%i", &i);
        OVERHEAL_REPFILL = (bool) i;
//			}else if(strcmp(var, "LEARNRATE=")==0){
//				sscanf(dataval, "%f", &f);
//				LEARNRATE= f;
      } else if (strcmp(var, "MAXDEVIATION=") == 0) {
        sscanf(dataval, "%f", &f);
        MAXDEVIATION = f;
      } else if (strcmp(var, "MUTCHANCE=") == 0) {
        sscanf(dataval, "%f", &f);
        MUTCHANCE = f;
      } else if (strcmp(var, "MUTSIZE=") == 0) {
        sscanf(dataval, "%f", &f);
        MUTSIZE = f;
      } else if (strcmp(var, "MAXAGE=") == 0) {
        sscanf(dataval, "%i", &i);
        MAXAGE = i;
      } else if (strcmp(var, "DIST=") == 0) {
        sscanf(dataval, "%f", &f);
        DIST = f;
      } else if (strcmp(var, "SPIKELENGTH=") == 0) {
        sscanf(dataval, "%f", &f);
        SPIKELENGTH = f;
      } else if (strcmp(var, "TOOCLOSE=") == 0) {
        sscanf(dataval, "%f", &f);
        TOOCLOSE = f;
      } else if (strcmp(var, "FOOD_SHARING_DISTANCE=") == 0) {
        sscanf(dataval, "%f", &f);
        FOOD_SHARING_DISTANCE = f;
      } else if (strcmp(var, "SEXTING_DISTANCE=") == 0) {
        sscanf(dataval, "%f", &f);
        SEXTING_DISTANCE = f;
      } else if (strcmp(var, "GRABBING_DISTANCE=") == 0) {
        sscanf(dataval, "%f", &f);
        GRABBING_DISTANCE = f;
      } else if (strcmp(var, "HEALTHLOSS_WHEELS=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_WHEELS = f;
      } else if (strcmp(var, "HEALTHLOSS_BOOSTMULT=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_BOOSTMULT = f;
      } else if (strcmp(var, "HEALTHLOSS_BADTEMP=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_BADTEMP = f;
      } else if (strcmp(var, "HEALTHLOSS_AGING=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_AGING = f;
      } else if (strcmp(var, "HEALTHLOSS_BRAINUSE=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_BRAINUSE = f;
      } else if (strcmp(var, "HEALTHLOSS_BUMP=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_BUMP = f;
      } else if (strcmp(var, "HEALTHLOSS_SPIKE_EXT=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_SPIKE_EXT = f;
      } else if (strcmp(var, "HEALTHLOSS_BADAIR=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_BADAIR = f;
      } else if (strcmp(var, "HEALTHLOSS_NOOXYGEN=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_NOOXYGEN = f;
      } else if (strcmp(var, "HEALTHLOSS_ASSEX=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_ASSEX = f;
      } else if (strcmp(var, "HEALTHLOSS_JAWSNAP=") == 0) {
        sscanf(dataval, "%f", &f);
        HEALTHLOSS_JAWSNAP = f;
      } else if (strcmp(var, "STOMACH_EFF=") == 0) {
        sscanf(dataval, "%f", &f);
        STOMACH_EFF = f;
      } else if (strcmp(var, "FOODINTAKE=") == 0) {
        sscanf(dataval, "%f", &f);
        FOODINTAKE = f;
      } else if (strcmp(var, "FOODDECAY=") == 0) {
        sscanf(dataval, "%f", &f);
        FOODDECAY = f;
      } else if (strcmp(var, "FOODGROWTH=") == 0) {
        sscanf(dataval, "%f", &f);
        FOODGROWTH = f;
      } else if (strcmp(var, "FOODWASTE=") == 0) {
        sscanf(dataval, "%f", &f);
        FOODWASTE = f;
      } else if (strcmp(var, "FOODADDFREQ=") == 0) {
        sscanf(dataval, "%i", &i);
        FOODADDFREQ = i;
      } else if (strcmp(var, "FOODSPREAD=") == 0) {
        sscanf(dataval, "%f", &f);
        FOODSPREAD = f;
      } else if (strcmp(var, "FOODRANGE=") == 0) {
        sscanf(dataval, "%i", &i);
        FOODRANGE = i;
      } else if (strcmp(var, "FRUITINTAKE=") == 0) {
        sscanf(dataval, "%f", &f);
        FRUITINTAKE = f;
      } else if (strcmp(var, "FRUITDECAY=") == 0) {
        sscanf(dataval, "%f", &f);
        FRUITDECAY = f;
      } else if (strcmp(var, "FRUITWASTE=") == 0) {
        sscanf(dataval, "%f", &f);
        FRUITWASTE = f;
      } else if (strcmp(var, "FRUITADDFREQ=") == 0) {
        sscanf(dataval, "%i", &i);
        FRUITADDFREQ = i;
      } else if (strcmp(var, "FRUITREQUIRE=") == 0) {
        sscanf(dataval, "%f", &f);
        FRUITREQUIRE = f;
      } else if (strcmp(var, "MEATINTAKE=") == 0) {
        sscanf(dataval, "%f", &f);
        MEATINTAKE = f;
      } else if (strcmp(var, "MEATDECAY=") == 0) {
        sscanf(dataval, "%f", &f);
        MEATDECAY = f;
      } else if (strcmp(var, "MEATWASTE=") == 0) {
        sscanf(dataval, "%f", &f);
        MEATWASTE = f;
      } else if (strcmp(var, "MEATVALUE=") == 0) {
        sscanf(dataval, "%f", &f);
        MEATVALUE = f;
      } else if (strcmp(var, "HAZARDFREQ=") == 0) {
        sscanf(dataval, "%i", &i);
        HAZARDFREQ = i;
      } else if (strcmp(var, "HAZARDDECAY=") == 0) {
        sscanf(dataval, "%f", &f);
        HAZARDDECAY = f;
      } else if (strcmp(var, "HAZARDDEPOSIT=") == 0) {
        sscanf(dataval, "%f", &f);
        HAZARDDEPOSIT = f;
      } else if (strcmp(var, "HAZARDDAMAGE=") == 0) {
        sscanf(dataval, "%f", &f);
        HAZARDDAMAGE = f;
      }
    }
    if (cf) fclose(cf);

  } else {
    printf("settings.cfg did not exist! Its ok, I gave you a new one to tinker with\n");
    writeConfig();
  }
}

void Evolve::World::writeConfig() {
  FILE *cf = fopen("settings.cfg", "w");

  fprintf(cf,
          "settings.cfg\nThis file will be regenerated with defualt values if missing (regeneration is not necissary for the program to work)\nModify any value in this file to change constants in Evunits.\nAll changes will require a reset or relaunch to take effect\n");
  fprintf(cf, "\n");
  fprintf(cf,
          "Any lines that the program can't recognize are silently ignored (will work on a fix that tells the user what was/wasnt changed) Please note the sim only takes the last entry of a flag's value if there are duplicates. Also, can you find the hidden config flag?...\n");
  fprintf(cf, "\n");
  fprintf(cf,
          "Remember to RIGHT-CLICK->RELOAD CONFIG or RELAUNCH the program after modification!!!");
  fprintf(cf, "\n\n");
  fprintf(cf,
          "V= %f \t\t\t//Version number, internal, if different than program's, will ask user to overwrite or exit\n",
          conf::VERSION);
  fprintf(cf,
          "MINFOOD= %i \t\t\t//Minimum number of food cells which must have food during simulation. 0= off\n",
          conf::MINFOOD);
  fprintf(cf,
          "INITFOODDENSITY= %f \t//initial density of full food cells. Use 'INITFOOD= #' to set a number\n",
          conf::INITFOODDENSITY);
  fprintf(cf,
          "//INITFOOD= 0 \t\t\t//remove '//' from the flag to enable, overrides INITFOODDENSITY\n");
  fprintf(cf, "INITFRUITDENSITY= %f \t//initial density of full fruit cells\n",
          conf::INITFRUITDENSITY);
  fprintf(cf, "//INITFRUIT= 0\n");
  fprintf(cf, "\n");
  fprintf(cf, "NUMBOTS= %i \t\t\t//initial and minimum number of units\n",
          conf::NUMBOTS);
  fprintf(cf,
          "ENOUGHBOTS= %i \t\t//if more units than this number, random spawns no longer occur\n",
          conf::ENOUGHBOTS);
  fprintf(cf,
          "TOOMANYBOTS= %i \t\t//number of units at which the full NOAIR healthloss is applied\n",
          conf::TOOMANYBOTS);
  fprintf(cf, "\n");
  fprintf(cf,
          "REPORTS_PER_EPOCH= %i \t\t//number of times to record data per epoch. 0 for off. (David Coleman)\n",
          conf::REPORTS_PER_EPOCH);
  fprintf(cf,
          "FRAMES_PER_EPOCH= %i \t//number of frames before epoch is incremented by 1.\n",
          conf::FRAMES_PER_EPOCH);
  fprintf(cf,
          "FRAMES_PER_DAY= %i \t\t//number of frames it takes for the daylight cycle to go completely around the map\n",
          conf::FRAMES_PER_DAY);
  fprintf(cf, "\n");
  fprintf(cf,
          "CONTINENTS= %i \t\t\t//number of 'continents' generated on the land layer. Not guarenteed to be accurate\n",
          conf::CONTINENTS);
  fprintf(cf,
          "OCEANPERCENT= %f \t\t//decimal ratio of terrain layer which will be ocean. Aproximately\n",
          conf::OCEANPERCENT);
  fprintf(cf,
          "MOONLIT= %i \t\t\t//true-false flag for letting units see other units at night. 0= no eyesight, 1= see units at half light\n",
          conf::MOONLIT);
  fprintf(cf,
          "GRAVITYACCEL= %f \t\t//how fast a bot will 'fall' after jumping. 0= jump disabled, 0.1+ = super-gravity]\n",
          conf::GRAVITYACCEL);
  fprintf(cf,
          "REACTPOWER= %f \t\t//the restoring force between two colliding units. 0= no reaction (disables TOOCLOSE and all collisions)\n",
          conf::REACTPOWER);
  fprintf(cf, "SPIKEMULT= %f \t\t//health multiplier of spike injury\n",
          conf::SPIKEMULT);
  fprintf(cf,
          "BRAINSIZE= %i \t\t\t//number _neurons in every unit brain. Will not make brains smaller than Inputs + Outputs. Saved per world, will override for loaded worlds\n",
          conf::BRAINSIZE);
  fprintf(cf, "\n");
  fprintf(cf, "BOTSPEED= %f \t\t//fastest possible speed of units\n",
          conf::BOTSPEED);
  fprintf(cf,
          "BOOSTSIZEMULT= %f \t//how much boost do units get when boost is active?\n",
          conf::BOOSTSIZEMULT);
  fprintf(cf,
          "SOUNDPITCHRANGE= %f \t//range below hearhigh and above hearlow within which external sounds fade in\n",
          conf::SOUNDPITCHRANGE);
  fprintf(cf,
          "FOODTRANSFER= %f \t\t//how much health is transfered between two units trading food per tick? =0 disables all generousity\n",
          conf::FOODTRANSFER);
  fprintf(cf,
          "MEANRADIUS= %f \t\t//average unit radius (0.2*,2.2*) (only applies to random units, no limits on mutations)\n",
          conf::MEANRADIUS);
  fprintf(cf, "SPIKESPEED= %f \t\t//how quickly can the spike be extended?\n",
          conf::SPIKESPEED);
  fprintf(cf,
          "FRESHKILLTIME= %i \t\t//number of ticks after a spike, collision, or bite that an unit will still drop full meat\n",
          conf::FRESHKILLTIME);
  fprintf(cf,
          "TENDERAGE= %i \t\t\t//age of units where full meat and hazard is finally given. Drop values are age/TENDERAGE before then. =0 off\n",
          conf::TENDERAGE);
  fprintf(cf,
          "MINMOMHEALTH= %f \t\t//minimum amount of health required for an unit to have a child\n",
          conf::MINMOMHEALTH);
  fprintf(cf,
          "REPRATE= %f \t\t//amount of food required to be consumed for an unit to reproduce\n",
          conf::REPRATE);
  fprintf(cf,
          "OVERHEAL_REPFILL= %i \t\t//true-false flag for letting units redirect overfill health (>2) to repcounter. 1= conserves matter, 0= balanced most efficient metabolism\n",
          conf::OVERHEAL_REPFILL);
//	fprintf(cf,	"LEARNRATE= %f\n", conf::LEARNRATE);
  fprintf(cf,
          "MAXDEVIATION= %f \t//maximum difference in species ID a crossover reproducing unit will be willing to tolerate\n",
          conf::MAXDEVIATION);
  fprintf(cf,
          "MUTCHANCE= %f \t\t//the chance of mutations occuring (note that various mutations modify this value up or down)\n",
          conf::MUTCHANCE);
  fprintf(cf,
          "MUTSIZE= %f \t\t//the magnitude of mutations (note that various mutations modify this value up or down)\n",
          conf::MUTSIZE);
  fprintf(cf,
          "MAXAGE= %i \t\t\t//Age at which the full HEALTHLOSS_AGING amount is applied to an unit\n",
          conf::MAXAGE);
  fprintf(cf, "\n");
  fprintf(cf,
          "DIST= %f \t\t//how far the senses can detect other units or cells\n",
          conf::DIST);
  fprintf(cf,
          "SPIKELENGTH= %f \t\t//full spike length. Can not be more than DIST\n",
          conf::SPIKELENGTH);
  fprintf(cf,
          "TOOCLOSE= %f \t\t//how much two units can be overlapping before they take damage. -1 disables event\n",
          conf::TOOCLOSE);
  fprintf(cf,
          "FOOD_SHARING_DISTANCE= %f //how far away can food be shared between bots?\n",
          conf::FOOD_SHARING_DISTANCE);
  fprintf(cf,
          "SEXTING_DISTANCE= %f \t//how far away can two units sexually reproduce?\n",
          conf::SEXTING_DISTANCE);
  fprintf(cf,
          "GRABBING_DISTANCE= %f \t//how far away can a bot grab another? Can not be more than DIST\n",
          conf::GRABBING_DISTANCE);
  fprintf(cf, "\n");
  fprintf(cf,
          "HEALTHLOSS_WHEELS= %f \t//How much health is lost for a bot driving at full speed\n",
          conf::HEALTHLOSS_WHEELS);
  fprintf(cf,
          "HEALTHLOSS_BOOSTMULT= %f \t//how much boost costs (set to 1 to nullify boost cost; its a multiplier)\n",
          conf::HEALTHLOSS_BOOSTMULT);
  fprintf(cf,
          "HEALTHLOSS_BADTEMP= %f \t//how quickly health drains in nonpreferred temperatures\n",
          conf::HEALTHLOSS_BADTEMP);
  fprintf(cf, "HEALTHLOSS_AGING= %f \t//health lost at MAXAGE\n",
          conf::HEALTHLOSS_AGING);
  fprintf(cf,
          "HEALTHLOSS_BRAINUSE= %f \t//how much health is reduced for each box in the brain being active\n",
          conf::HEALTHLOSS_BRAINUSE);
  fprintf(cf,
          "HEALTHLOSS_BUMP= %f \t//how much health is lost upon collision. Note that =0 does not disable the event (see TOOCLOSE)\n",
          conf::HEALTHLOSS_BUMP);
  fprintf(cf,
          "HEALTHLOSS_SPIKE_EXT= %f \t//how much health a bot looses for extending spike\n",
          conf::HEALTHLOSS_SPIKE_EXT);
  fprintf(cf,
          "HEALTHLOSS_BADAIR= %f \t//how much health is lost if in totally opposite environment\n",
          conf::HEALTHLOSS_BADAIR);
  fprintf(cf,
          "HEALTHLOSS_NOOXYGEN= %f \t//how much bots are penalized when total units = TOOMANYBOTS\n",
          conf::HEALTHLOSS_NOOXYGEN);
  fprintf(cf,
          "HEALTHLOSS_ASSEX= %f \t//multiplier for radius/MEANRADIUS penalty on mother for asexual reproduction\n",
          conf::HEALTHLOSS_ASSEX);
  fprintf(cf,
          "HEALTHLOSS_JAWSNAP= %f \t//when jaws snap fully (0->1), this is the damage applied to bots in front\n",
          conf::HEALTHLOSS_JAWSNAP);
  fprintf(cf, "\n");
  fprintf(cf,
          "STOMACH_EFF= %f \t\t//the worst possible multiplier produced from having at least two stomach types at 100%. 1/4 is harsh. 1 is no effect\n",
          conf::STOMACH_EFF);
  fprintf(cf, "\n");
  fprintf(cf,
          "FOODINTAKE= %f \t\t//how much plant food can feed an unit per tick?\n",
          conf::FOODINTAKE);
  fprintf(cf,
          "FOODDECAY= %f \t\t//how much does food decay(+)/grow(-) on a cell which already has food?\n",
          conf::FOODDECAY);
  fprintf(cf,
          "FOODGROWTH= %f \t\t//how much does food increase by on a cell with both plant and hazard? (fertilizer). =0 disables\n",
          conf::FOODGROWTH);
  fprintf(cf,
          "FOODWASTE= %f \t\t//how much food disapears when an unit eats it?\n",
          conf::FOODWASTE);
//	fprintf(cf, "FOODMAX= %f\n", conf::FOODMAX);
  fprintf(cf,
          "FOODADDFREQ= %i \t\t//how often does a random cell get set to full food?\n",
          conf::FOODADDFREQ);
  fprintf(cf,
          "FOODSPREAD= %f \t\t//probability of a fruit cell seeding food to a nearby cell. 0.0002= VERY fast food growth\n",
          conf::FOODSPREAD);
  fprintf(cf,
          "FOODRANGE= %i \t\t\t//distance that a single cell of food can seed. Units in cells.\n",
          conf::FOODRANGE);
  fprintf(cf, "\n");
  fprintf(cf,
          "FRUITINTAKE= %f \t\t//how much fruit can feed an unit per tick?\n",
          conf::FRUITINTAKE);
  fprintf(cf,
          "FRUITDECAY= %f \t\t//how much fruit decays on a cell with plant life less than FRUITREQUIRE?\n",
          conf::FRUITDECAY);
  fprintf(cf,
          "FRUITWASTE= %f \t\t//how much fruit disapears when an unit eats it?\n",
          conf::FRUITWASTE);
//	fprintf(cf, "FRUITMAX= %f\n", conf::FRUITMAX);
  fprintf(cf,
          "FRUITADDFREQ= %i \t\t//how often does a high-plant-food cell get set to full fruit?\n",
          conf::FRUITADDFREQ);
  fprintf(cf,
          "FRUITREQUIRE= %f \t\t//minimum plant food on same cell required for fruit to persist; below, FRUITDECAY is applied\n",
          conf::FRUITREQUIRE);
  fprintf(cf, "\n");
  fprintf(cf, "MEATINTAKE= %f \t\t//how much meat can feed an unit per tick?\n",
          conf::MEATINTAKE);
  fprintf(cf,
          "MEATDECAY= %f \t\t//how much meat decays/grows on a cell? -= MEATDECAY/[meat_present] (faster decay at lower values)\n",
          conf::MEATDECAY);
  fprintf(cf,
          "MEATWASTE= %f \t\t//how much meat disapears when an unit eats it?\n",
          conf::MEATWASTE);
//	fprintf(cf, "MEATMAX= %f\n", conf::MEATMAX);
  fprintf(cf, "MEATVALUE= %f \t\t//how much meat a bot's body is worth?\n",
          conf::MEATVALUE);
  fprintf(cf, "\n");
  fprintf(cf, "HAZARDFREQ= %i \t\t\t//how often an instant hazard appears?\n",
          conf::HAZARDFREQ);
  fprintf(cf,
          "HAZARDDECAY= %f \t\t//how much non-event hazard decays/grows on a cell per tick?\n",
          conf::HAZARDDECAY);
  fprintf(cf,
          "HAZARDDEPOSIT= %f \t//how much hazard is placed by an unit per tick?\n",
          conf::HAZARDDEPOSIT);
  fprintf(cf,
          "HAZARDDAMAGE= %f \t\t//how much health an unit looses while on a filled hazard cell per tick? (note that 9/10 of this is max waste damage)\n",
          conf::HAZARDDAMAGE);
//	fprintf(cf, "HAZARDMAX= %f\n", conf::HAZARDMAX);
  fclose(cf);
}

void Evolve::World::update() {
  modcounter++;
  vector<int> dt;

  if (REPORTS_PER_EPOCH > 0 &&
      modcounter % (FRAMES_PER_EPOCH / REPORTS_PER_EPOCH) == 0) {
    findStats();
    //write report and record counts
    num.herbivore[ptr] = getHerbivores();
    num.carnivore[ptr] = getCarnivores();
    num.frugivore[ptr] = getFrugivores();
    num.hybrid[ptr]    = getHybrids();
    num.dead[ptr]      = getDead();
    num.total[ptr]     = getAlive();
    ptr++;
    if (ptr == conf::RECORD_SIZE) ptr = 0;

    writeReport();

    deaths.clear();
  } else if (modcounter % 12 == 0) findStats();

  if (modcounter >= FRAMES_PER_EPOCH) {
    modcounter = 0;
    current_epoch++;
  }
  if ((modcounter % FOODADDFREQ == 0 && !_closed) || getFood() < MINFOOD) {
    int cx = randi(0, CW);
    int cy = randi(0, CH);
    cells[Layer::PLANTS][cx][cy] = conf::FOODMAX;
  }
  if (modcounter % HAZARDFREQ == 0) {
    int cx                        = randi(0, CW);
    int cy                        = randi(0, CH);
    cells[Layer::HAZARDS][cx][cy] = capm(
            (cells[Layer::HAZARDS][cx][cy] / conf::HAZARDMAX / 90 + 0.99) *
            conf::HAZARDMAX, 0, conf::HAZARDMAX);
  }
  if (modcounter % FRUITADDFREQ == 0) {
    bool trigger = false;
    while (!trigger) {
      int cx = randi(0, CW);
      int cy = randi(0, CH);
      if (cells[Layer::PLANTS][cx][cy] > 0) {
        cells[Layer::FRUITS][cx][cy] = conf::FRUITMAX;
        trigger = true;
      }
    }
  }

#pragma omp parallel for schedule(dynamic)
  for (int cx = 0; cx < (int) CW; cx++) {
    for (int cy = 0; cy < (int) CH; cy++) {
      if (cells[Layer::PLANTS][cx][cy] > 0 &&
          cells[Layer::PLANTS][cx][cy] < conf::FOODMAX) {
        cells[Layer::PLANTS][cx][cy] -= FOODDECAY;
        if (cells[Layer::HAZARDS][cx][cy] > 0) {
          cells[Layer::PLANTS][cx][cy] +=
                  FOODGROWTH * cells[Layer::HAZARDS][cx][cy] /
                  conf::HAZARDMAX;
        }
      }
      if (randf(0, 1) < FOODSPREAD &&
          cells[Layer::FRUITS][cx][cy] >= 0.5 * conf::FRUITMAX) {
        int ox = randi(cx - 1 - FOODRANGE, cx + 2 + FOODRANGE);
        int oy = randi(cy - 1 - FOODRANGE, cy + 2 + FOODRANGE);
        if (ox < 0) ox += CW;
        if (ox > CW - 1) ox -= CW;
        if (oy < 0) oy += CH;
        if (oy > CH - 1)
          oy -= CH;
        if (cells[Layer::PLANTS][ox][oy] <= conf::FOODMAX * 3 / 4)
          cells[Layer::PLANTS][ox][oy] += conf::FOODMAX / 4;
      }
      cells[Layer::PLANTS][cx][cy] = capm(cells[Layer::PLANTS][cx][cy], 0,
                                          conf::FOODMAX);

      if (cells[Layer::MEATS][cx][cy] > 0) {
        cells[Layer::MEATS][cx][cy] -= MEATDECAY /
                                       (cells[Layer::MEATS][cx][cy] /
                                        conf::MEATMAX);
      }
      cells[Layer::MEATS][cx][cy]  = capm(cells[Layer::MEATS][cx][cy], 0,
                                          conf::MEATMAX);

      if (cells[Layer::HAZARDS][cx][cy] > 0) {
        cells[Layer::HAZARDS][cx][cy] -= HAZARDDECAY;
      }
      if (cells[Layer::HAZARDS][cx][cy] > conf::HAZARDMAX * 9 / 10 &&
          randf(0, 1) < 0.0625) {
        cells[Layer::HAZARDS][cx][cy] =
                90 * (cells[Layer::HAZARDS][cx][cy] / conf::HAZARDMAX - 0.99) *
                conf::HAZARDMAX;
      }
      cells[Layer::HAZARDS][cx][cy] = capm(cells[Layer::HAZARDS][cx][cy], 0,
                                           conf::HAZARDMAX);

      if (cells[Layer::PLANTS][cx][cy] <= conf::FOODMAX * FRUITREQUIRE &&
          cells[Layer::FRUITS][cx][cy] > 0) {
        cells[Layer::FRUITS][cx][cy] -= FRUITDECAY;
      }
      cells[Layer::FRUITS][cx][cy]  = capm(cells[Layer::FRUITS][cx][cy], 0,
                                           conf::FRUITMAX);

      cells[Layer::LIGHT][cx][cy] = capm(0.5 + sin((cx * 2 * M_PI) / CW -
                                                   (modcounter + current_epoch *
                                                                 FRAMES_PER_EPOCH) *
                                                   2 * M_PI / FRAMES_PER_DAY),
                                         0, 1);
    }
  }

  setInputs();

  brainsTick();

#pragma omp parallel for
  for (int i = 0; i < (int) units.size(); i++) {
    Unit *a                                               = &units[i];

    if (a->indicator != 0) a->indicator -= 1;
    if (a->indicator < -conf::CARCASSFRAMES) a->indicator = 0;

    if (a->jawrend > 0) a->jawrend -= 1;

    if (a->health > 0) {
      if (HEALTHLOSS_BRAINUSE != 0) a->setActivity();

      if (a->freshkill > 0) a->freshkill -= 1;

      if (randf(0, 1) < 0.0001) {
        float MR  = a->MUTRATE1;
        float MR2 = a->MUTRATE2;
        a->brain.liveMutate(MR, MR2, a->out);
        a->initEvent(30, 0.5, 0, 1.0);
        if (a->id == _selection) addEvent("The Selected Unit Was Mutated!");
      }

      if (modcounter % 100 == 0) a->age += 1;
    }
  }

  processOutputs();

  healthTick();

  if (modcounter % 5 == 0) {
    for (int i = 0; i < (int) units.size(); i++) {
      if (units[i].repcounter < 0 && units[i].health >= MINMOMHEALTH) {

        if (units[i].sexproject) {
          for (int j = 0; j < (int) units.size(); j++) {
            if (i == j || !(units[j].sexproject)) continue;
            float d         = (units[i].pos - units[j].pos).length();
            float deviation = abs(units[i].species -
                                  units[j].species);
            if (d <= SEXTING_DISTANCE && deviation <= MAXDEVIATION) {
              if (_debug) printf("Attempting to have children...");
              reproduce(i, j);
              units[i].repcounter = conf::REPRATE;
              units[j].repcounter = conf::REPRATE;
              if (_debug) printf(" Success!\n");
              break;
              continue;
            }
          }
        } else {
          if (_debug) printf("Attempting budding...");
          reproduce(i, i);
          units[i].repcounter = conf::REPRATE;
          if (_debug) printf(" Success!\n");
          continue;
        }
      }
      if (FUN && randf(0, 1) < 0.4) {
        if (units[i].indicator <= 0) units[i].indicator = 60;
        units[i].ir = randf(0, 1);
        units[i].ig = randf(0, 1);
        units[i].ib = randf(0, 1);
      }
    }
  }

  processInteractions();

  for (int i = 0; i < (int) units.size(); i++) {
    if (units[i].health < 0) units[i].writeIfKilled("Killed by Something ?");
    if (units[i].health == 0 && units[i].indicator == -1) {
      if (_selection == units[i].id) {
        printf("The Selected Unit was %s!\n", units[i].death);
        addEvent("The Selected Unit Died!");
      }

      int cx = (int) units[i].pos.x / conf::CZ;
      int cy = (int) units[i].pos.y / conf::CZ;

      float meat        = cells[Layer::MEATS][cx][cy];
      float agemult     = 1.0;
      float freshmult   = 0.25;
      float stomachmult = (4 - 3 * units[i].stomach[Stomach::MEAT]) / 4;
      if (units[i].age < TENDERAGE)
        agemult   = units[i].age /
                    TENDERAGE;
      if (units[i].freshkill > 0)
        freshmult = 1;

      meat += MEATVALUE * conf::MEATMAX * agemult * freshmult * stomachmult;
      cells[Layer::MEATS][cx][cy] = capm(meat, 0, conf::MEATMAX);

      deaths.push_back(units[i].death);
    }
  }

  vector<Unit>::iterator iter = units.begin();
  while (iter != units.end()) {
    if (iter->health <= 0 && iter->indicator == 0) {
      iter = units.erase(iter);
    } else {
      ++iter;
    }
  }

  if (!_closed) {
    int alive = units.size() - getDead();
    if (alive < NUMBOTS) {
      if (_debug) printf("Attempting unit conservation program...");
      addUnits(NUMBOTS - alive);
      if (_debug) printf(" Success!\n");
    }
    if (modcounter % 50 == 0 && alive < ENOUGHBOTS) {
      if (randf(0, 1) < 0.5) {
        if (_debug) printf("Attempting random spawn...");
        addUnits(
                1); //every now and then add random bot in if population too low
        if (_debug) printf(" Success!\n");
      }
    }
  }

  vector<std::pair<const char *, int> >::iterator item = events.begin();
  while (item != events.end()) {
    if (item->second < -conf::EVENTS_HALFLIFE) {
      item = events.erase(item);
    } else {
      ++item;
    }
  }
}

void Evolve::World::setInputs() {
  float PI8  = M_PI / 8 / 2; //pi/8/2
  float PI38 = 3 * PI8; //3pi/8/2
  float PI4  = M_PI / 4;

  selectedSounds.clear();

#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int) units.size(); i++) {
    if (units[i].health <= 0) continue;
    Unit *a = &units[i];

    int scx = (int) a->pos.x / conf::CZ;
    int scy = (int) a->pos.y / conf::CZ;

    a->in[Input::HEALTH] = cap(a->health / 2);

    float light = cells[Layer::LIGHT][scx][scy];

    vector<float> r(NUMEYES, 0.25 * light);
    vector<float> g(NUMEYES, 0.25 * light);
    vector<float> b(NUMEYES, 0.25 * light);

    float smellsum = 0;

    vector<float> hearsum(NUMEARS, 0);

    float blood = 0;

    int minx, maxx, miny, maxy;

    minx = max((scx - DIST / conf::CZ / 2), (float) 0);
    maxx = min((scx + 1 + DIST / conf::CZ / 2), (float) CW);
    miny = max((scy - DIST / conf::CZ / 2), (float) 0);
    maxy = min((scy + 1 + DIST / conf::CZ / 2), (float) CH);

    float fruit = 0, meat = 0, hazard = 0, water = 0;

    for (scx = minx; scx < maxx; scx++) {
      for (scy = miny; scy < maxy; scy++) {
        fruit += cells[Layer::FRUITS][scx][scy] / conf::FRUITMAX;
        meat += cells[Layer::MEATS][scx][scy] / conf::MEATMAX;
        hazard += cells[Layer::HAZARDS][scx][scy] / conf::HAZARDMAX;
        water += (1 - cells[Layer::LAND][scx][scy]);
      }
    }
    fruit *= a->smell_mod / (maxx - minx) / (maxy - miny);
    meat *= a->smell_mod / (maxx - minx) / (maxy - miny);
    hazard *= a->smell_mod / (maxx - minx) / (maxy - miny);
    water *= a->smell_mod / (maxx - minx) / (maxy - miny);

    for (int j = 0; j < (int) units.size(); j++) {
      if (i == j) continue;
      Unit *a2 = &units[j];

      if (a->pos.x < a2->pos.x - DIST || a->pos.x > a2->pos.x + DIST
          || a->pos.y > a2->pos.y + DIST || a->pos.y < a2->pos.y - DIST)
        continue;

      float d = (a->pos - a2->pos).length();

      if (d < DIST) {

        smellsum += a->smell_mod;
        for (int q = 0; q < NUMEARS; q++) {
          Vector2f v(a->radius, 0);
          v.rotate(a->angle + a->eardir[q]);
          Vector2f earpos  = a->pos + v;
          float    eardist = (earpos - a2->pos).length();
          for (int n       = 0; n < 2; n++) {
            float otone = 0, ovolume = 0;
            if (n == 0) {
              otone   = a2->tone;
              ovolume = a2->volume;
            } else if (n == 1) {
              otone   = 0.25;
              ovolume = (max(fabs(a2->w1), fabs(a2->w2)));
            }
            if (getSelection() == a->id) {
              int   volfact   = (int) (a->hear_mod * (1 - eardist / DIST) *
                                       ovolume * 100);
              float finalfact = otone + volfact;
              selectedSounds.push_back(finalfact);
            }

            if (otone >= a->hearhigh[q]) continue;
            if (otone <= a->hearlow[q]) continue;
            if (getSelection() == a->id) {
              selectedSounds[selectedSounds.size() - 1] += 100;
            }
            float tonemult = cap(min((a->hearhigh[q] - otone) / SOUNDPITCHRANGE,
                                     (otone - a->hearlow[q]) /
                                     SOUNDPITCHRANGE));
            hearsum[q] +=
                    a->hear_mod * (1 - eardist / DIST) * ovolume * tonemult;
          }
        }

        float ang       = (a2->pos - a->pos).get_angle();

        if (light != 0 || MOONLIT) {
          for (int q = 0; q < NUMEYES; q++) {
            float aa = a->angle + a->eyedir[q];
            if (aa < -M_PI) aa += 2 * M_PI;
            if (aa > M_PI) aa -= 2 * M_PI;
            float diff1 = aa - ang;
            if (fabs(diff1) > M_PI) diff1 = 2 * M_PI - fabs(diff1);
            diff1 = fabs(diff1);
            float fov = a->eyefov[q];
            if (diff1 < fov) {
              float lightmult = light;
              if (MOONLIT) lightmult = max((float) 0.5, light);
              float mul1 = lightmult * a->eye_see_unit_mod *
                           (fabs(fov - diff1) / fov) * (1 - d / DIST) *
                           (1 - d / DIST);
              r[q] += mul1 * a2->red;
              g[q] += mul1 * a2->gre;
              b[q] += mul1 * a2->blu;
              if (a->id == _selection && isDebug()) {
                linesA.push_back(a->pos);
                linesB.push_back(a2->pos);
              }
            }
          }
        }
        float forwangle = a->angle;
        float diff4     = forwangle - ang;
        if (fabs(forwangle) > M_PI) diff4 = 2 * M_PI - fabs(forwangle);
        diff4 = fabs(diff4);
        if (diff4 < PI38) {
          float mul4 = ((PI38 - diff4) / PI38) * (1 - d / DIST);
          blood += a->blood_mod * mul4 *
                   (1 - units[j].health / 2);
        }
      }
    }

    float    temp              =
                     (float) 2.0 * abs(a->pos.y / conf::HEIGHT - 0.5);
    for (int i                 = 0; i < NUMEYES; i++) {
      a->in[Input::EYES + i * 3]     = cap(r[i]);
      a->in[Input::EYES + 1 + i * 3] = cap(g[i]);
      a->in[Input::EYES + 2 + i * 3] = cap(b[i]);
    }
    a->in[Input::CLOCK1]       = abs(
            sin((modcounter + current_epoch * FRAMES_PER_EPOCH) / a->clockf1));
    a->in[Input::CLOCK2]       = abs(
            sin((modcounter + current_epoch * FRAMES_PER_EPOCH) / a->clockf2));
    a->in[Input::CLOCK3]       = abs(
            sin((modcounter + current_epoch * FRAMES_PER_EPOCH) / a->clockf3));
    a->in[Input::HEARING1]     = cap(hearsum[0]);
    a->in[Input::HEARING2]     = cap(hearsum[1]);
    a->in[Input::BOT_SMELL]    = cap(smellsum);
    a->in[Input::BLOOD]        = cap(blood);
    a->in[Input::TEMP]         = temp;
    if (a->id == _selection) {
      a->in[Input::PLAYER] = pinput1;
    } else {
      a->in[Input::PLAYER] = 0.0;
    }
    a->in[Input::FRUIT_SMELL]  = cap(fruit);
    a->in[Input::MEAT_SMELL]   = cap(meat);
    a->in[Input::HAZARD_SMELL] = cap(hazard);
    a->in[Input::WATER_SMELL]  = cap(water);
    a->in[Input::RANDOM]       = cap(randn(a->in[Input::RANDOM], 0.08));
  }
}

void Evolve::World::brainsTick() {
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int) units.size(); i++) {
    if (units[i].health <= 0) continue;
    units[i].tick();
  }
}

void Evolve::World::processOutputs() {
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int) units.size(); i++) {
    Unit *a    = &units[i];

    if (a->health <= 0) {
      float sp = (a->w1 + a->w2) / 2;
      a->w1 = 0.75 * sp;
      a->w2 = 0.75 * sp;

      a->volume *= 0.9;
      if (a->volume < 0.001) a->volume = 0;
      continue;
    }
    if (a->jump <= 0) {
      if (pcontrol && a->id == _selection) {
        a->w1 = pright;
        a->w2 = pleft;
      } else {
        a->w1 += a->strength *
                 (a->out[Output::LEFT_WHEEL_F] - a->out[Output::LEFT_WHEEL_B] -
                  a->w1);
        a->w2 += a->strength * (a->out[Output::RIGHT_WHEEL_F] -
                                a->out[Output::RIGHT_WHEEL_B] - a->w2);
      }
    }
    a->red += 0.2 * (a->out[Output::RED] - a->red);
    a->gre += 0.2 * (a->out[Output::GRE] - a->gre);
    a->blu += 0.2 * (a->out[Output::BLU] - a->blu);
    if (a->jump <= 0)
      a->boost = a->out[Output::BOOST] > 0.5;
    if (a->health > 0) a->volume = a->out[Output::VOLUME];
    else a->volume  = 0;
    a->tone         = a->out[Output::TONE];
    a->give         = a->out[Output::GIVE];
    if (a->health > 0)
      a->sexproject = a->out[Output::PROJECT] >= 0.5 ? true : false;
    else a->sexproject = false;

    float g = a->out[Output::SPIKE];
    if (a->spikeLength < g) {
      a->spikeLength += SPIKESPEED;
      a->health -= HEALTHLOSS_SPIKE_EXT;
      a->writeIfKilled("Killed by Spike Raising");
    } else if (a->spikeLength > g)
      a->spikeLength = g;
    a->grabbing      = a->out[Output::GRAB];
    if (GRAVITYACCEL > 0) {
      float height = (a->out[Output::JUMP] - 0.5) * 2;
      if (a->jump == 0 && height > 0 && a->age > 0) a->jump = height;
    }
    float newjaw     = cap(a->out[Output::JAW] - a->jawOldPos);
    if (a->jawPosition > 0) {
      a->jawPosition *= -1;
      a->jawrend = 15;
    } else if (a->jawPosition < 0)
      a->jawPosition = min(0.0, a->jawPosition + 0.05 * (2 + a->jawPosition));
    else if (a->age > 0 && newjaw > 0.01) a->jawPosition = newjaw;
    a->jawOldPos = a->out[Output::JAW];
    a->clockf3 += 0.5 * ((95 * (1 - a->out[Output::CLOCKF3]) + 5) - a->clockf3);
    if (a->clockf3 > 100) a->clockf3 = 100;
    if (a->clockf3 < 2) a->clockf3   = 2;
  }

#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int) units.size(); i++) {
    Unit *a   = &units[i];

    a->jump -= GRAVITYACCEL;
    if (a->jump < -1)
      a->jump = 0;

    Vector2f v1(sqrt(a->radius / MEANRADIUS) * 5, 0);
    v1.rotate(a->angle + M_PI / 2);

    float BW1 = BOTSPEED * a->w1;
    float BW2 = BOTSPEED * a->w2;
    if (a->boost) {
      BW1 = BW1 * BOOSTSIZEMULT;
      BW2 = BW2 * BOOSTSIZEMULT;
    }

    Vector2f vv1 = v1;
    Vector2f vv2 = -v1;
    vv1.rotate(-BW1);
    vv2.rotate(BW2);
    a->pos     = a->pos + (vv1 - v1) + (vv2 + v1);

    if (a->jump <= 0) {
      a->angle += BW2 - BW1;
    }
    if (a->angle < -M_PI)
      a->angle = M_PI - (-M_PI - a->angle);
    if (a->angle > M_PI)
      a->angle = -M_PI + (a->angle - M_PI);

    a->borderRectify();
  }
}

void Evolve::World::processInteractions() {
#pragma omp parallel for
  for (int i = 0; i < (int) units.size(); i++) {
    Unit *a = &units[i];

    int scx = (int) a->pos.x / conf::CZ;
    int scy = (int) a->pos.y / conf::CZ;

    if (a->health > 0 && a->jump <= 0) {
      float intake   = 0;
      float speedmul = 1 - 0.5 * max(abs(a->w1), abs(a->w2));

      float food = cells[Layer::PLANTS][scx][scy];
      if (food > 0) {
        float plantintake = min(food, FOODINTAKE) * a->stomach[Stomach::PLANT] *
                            (1 -
                             a->stomach[Stomach::MEAT] * (1 - STOMACH_EFF)) *
                            (1 -
                             a->stomach[Stomach::FRUIT] * (1 - STOMACH_EFF)) *
                            speedmul;
        cells[Layer::PLANTS][scx][scy] -= min(food, FOODWASTE * plantintake /
                                                    FOODINTAKE);
        intake += plantintake;
      }

      float meat = cells[Layer::MEATS][scx][scy];
      if (meat > 0) {
        float meatintake = min(meat, MEATINTAKE) * a->stomach[Stomach::MEAT] *
                           (1 -
                            a->stomach[Stomach::PLANT] * (1 - STOMACH_EFF)) *
                           (1 -
                            a->stomach[Stomach::FRUIT] * (1 - STOMACH_EFF)) *
                           speedmul;
        cells[Layer::MEATS][scx][scy] -= min(meat, MEATWASTE * meatintake /
                                                   MEATINTAKE);
        intake += meatintake;
      }

      float fruit = cells[Layer::FRUITS][scx][scy];
      if (fruit > 0) {
        float fruitintake =
                      min(fruit, FRUITINTAKE) * a->stomach[Stomach::FRUIT] *
                      (1 - a->stomach[Stomach::MEAT] * (1 - STOMACH_EFF)) *
                      (1 - a->stomach[Stomach::PLANT] * (1 - STOMACH_EFF)) *
                      speedmul;
        cells[Layer::FRUITS][scx][scy] -= min(fruit, FRUITWASTE * fruitintake /
                                                     FRUITINTAKE);
        intake += fruitintake;
      }

      if (a->health >= MINMOMHEALTH) {
        a->repcounter -= a->metabolism * intake;
        a->health += (1 - a->metabolism) * intake;
      } else a->health += intake;

      float hazard  = cells[Layer::HAZARDS][scx][scy];
      float agemult = 1.0;
      if (hazard > 0) {
        if (a->age < TENDERAGE) agemult = 0.5 + 0.5 * units[i].age / TENDERAGE;
        a->health -= HAZARDDAMAGE * agemult * hazard;
        a->writeIfKilled("Killed by a Hazard");
      }
      if (modcounter % 5 == 0) {
        if (hazard < conf::HAZARDMAX * 9 / 10)
          hazard += HAZARDDEPOSIT * a->health;
        cells[Layer::HAZARDS][scx][scy] = capm(hazard, 0,
                                               conf::HAZARDMAX * 9 / 10);
      }

      float air = cells[Layer::LAND][scx][scy];
      if (pow(air - a->lungs, 2) >= 0.01) {
        a->health -= HEALTHLOSS_BADAIR * pow(air - a->lungs, 2);
        a->writeIfKilled("Killed by Suffocation .");
      }

      if (a->health > 2) {
        if (OVERHEAL_REPFILL)
          a->repcounter -= a->health - 2;
        a->health = 2;
      }
    }
  }

  if (modcounter % 2 == 0) {
    float    highestdist = max(max(max(FOOD_SHARING_DISTANCE, SEXTING_DISTANCE),
                                   GRABBING_DISTANCE),
                               SPIKELENGTH + MEANRADIUS * 3);
    for (int i           = 0; i < (int) units.size(); i++) {
      units[i].near  = false;
      units[i].dfood = 0;
      if (units[i].health <= 0)
        continue;

      for (int j = 0; j < i; j++) {
        if (units[i].pos.x < units[j].pos.x - highestdist
            || units[i].pos.x > units[j].pos.x + highestdist
            || units[i].pos.y > units[j].pos.y + highestdist
            || units[i].pos.y < units[j].pos.y - highestdist)
          continue;
        else {
          units[i].near = true;
          units[j].near = true;
          break;
          continue;
        }
      }
    }

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int) units.size(); i++) {
      if (units[i].health <= 0) continue;
      if (!units[i].near) continue;

      Unit *a = &units[i];

      for (int j = 0; j < (int) units.size(); j++) {
        if (i == j || !units[j].near) continue;
        if (units[j].health <= 0) {
          if (a->grabbing > 0.5 && a->grabID == units[j].id)
            a->grabbing = -1;
          if (units[j].health == 0)
            continue;
        }

        Unit *a2 = &units[j];

        float d = (a->pos - a2->pos).length();

        if (a->give > 0 && FOODTRANSFER > 0 && a2->health < 2) {

          float rd = a->give > 0.5 ? FOOD_SHARING_DISTANCE : a->radius +
                                                             a2->radius;
          if (d <= rd) {
            float healthrate = a->give > 0.5 ? FOODTRANSFER * a->give :
                               FOODTRANSFER * 2 * a->give;
            if (d <= a->radius + a2->radius && a->give > 0.5)
              healthrate = FOODTRANSFER;
            a2->health += healthrate;
            a->health -= healthrate;
            a2->dfood += healthrate;
            a->dfood -= healthrate;
            a->writeIfKilled("Killed by Excessive Generosity");
            if (a->health > 2) a->health = 2;
          }
        }

        float sumrad = a->radius + a2->radius;

        if (REACTPOWER > 0 && d < sumrad && a->jump <= 0 && a2->jump <= 0) {
          float ov = (sumrad - d);
          if (ov > 0 && d > 0.0001) {
            if (ov > TOOCLOSE && TOOCLOSE != -1) {
              float DMG = ov * HEALTHLOSS_BUMP;
              a->health -= DMG * sumrad / 2 / a->radius;
              a2->health -= DMG * sumrad / 2 / a2->radius;

              const char *fix = "Killed by a Collision";
              a->writeIfKilled(fix);
              a2->writeIfKilled(fix);

              if (a->health == 0) {
                break;
                continue;
              }
              if (a2->health == 0) {
                if (a->grabID == a2->id) a->grabbing = -1;
                a->killed++;
                continue;
              }

              a->initEvent(15, 0, 0.5, 1);
              a2->initEvent(15, 0, 0.5, 1);

              a->freshkill  = FRESHKILLTIME;
              a2->freshkill = FRESHKILLTIME;
            }
            float ff1 = capm(ov / d * a2->radius / a->radius * REACTPOWER, 0,
                             2);
            float ff2 = capm(ov / d * a->radius / a2->radius * REACTPOWER, 0,
                             2);
            a->pos.x -= (a2->pos.x - a->pos.x) * ff1;
            a->pos.y -= (a2->pos.y - a->pos.y) * ff1;
            a2->pos.x += (a2->pos.x - a->pos.x) * ff2;
            a2->pos.y += (a2->pos.y - a->pos.y) * ff2;

            a->borderRectify();
            a2->borderRectify();

//						printf("%f, %f, %f, %f, and %f\n", a->pos.x, a->pos.y, a2->pos.x, a2->pos.y, ov);
          }
        }

        const char *fix2 = "Killed by a Murder";

        if (a->isSpikey(SPIKELENGTH) && a->w1 > 0.1 && a->w2 > 0.1 &&
            d <= (sumrad + SPIKELENGTH * a->spikeLength) && a2->jump <= 0) {

          Vector2f v(1, 0);
          v.rotate(a->angle);
          float diff = v.angle_between(a2->pos - a->pos);
          if (fabs(diff) < M_PI / 8) {
            float DMG = SPIKEMULT * a->spikeLength * max(a->w1, a->w2);

            a2->health -= DMG;
            a2->writeIfKilled(fix2);
            a2->freshkill = FRESHKILLTIME;

            a->spikeLength = 0;

            a->initEvent(20 * DMG, 1, 0.5, 0);
            if (a2->health == 0) {
              a->initEvent(20 * DMG, 1, 0, 0);
              if (a->id == _selection)
                addEvent("The Selected Unit Killed Another!");
              a->killed++;
              if (a->grabID == a2->id) a->grabbing = -1;
              continue;
            } else if (a->id == _selection)
              addEvent("The Selected Unit Stabbed Another!");

            a->hits++;

            /*Vector2f v2(1,0);
            v2.rotate(a2->angle);
            float adiff= v.angle_between(v2);
            if (fabs(adiff)<M_PI/2) {
                //this was attack from the back. Retract spike of the other unit (startle!)
                //this is done so that the other unit cant right away "by accident" attack this unit
                a2->spikeLength= 0;
            }*/
          }
        }

        if (a->jawPosition > 0 && d <= (sumrad + 12.0)) {
          Vector2f v(1, 0);
          v.rotate(a->angle);
          float diff = v.angle_between(a2->pos - a->pos);
          if (fabs(diff) < M_PI / 6) {
            float DMG = HEALTHLOSS_JAWSNAP * a->jawPosition;

            a2->health -= DMG;
            a2->writeIfKilled(fix2);
            a2->freshkill = FRESHKILLTIME;
            a->hits++;

            a->initEvent(20 * a->jawPosition, 1, 1, 0);
            if (a2->health == 0) {
              a->initEvent(40 * DMG, 1, 0, 0);
              if (a->id == _selection)
                addEvent("The Selected Unit Killed Another!");
              a->killed++; //increment stat
              if (a->grabID == a2->id) a->grabbing = -1;
              continue;
            } else if (a->id == _selection)
              addEvent("The Selected Unit Bit Another!");
          }
        }

        if (a->grabbing > 0.5 && a2->health > 0) {
          if (d <= GRABBING_DISTANCE + a->radius) {
            if (a->grabID == -1) {
              Vector2f v(1, 0);
              v.rotate(a->angle);
              float diff = v.angle_between(a2->pos - a->pos);
              if (fabs(diff) < M_PI / 4 && randf(0, 1) <
                                           0.4) { //very wide AOE, 40% chance any one bot is picked
                a->grabID = a2->id;
              }

            } else if (a->grabID == a2->id && d > (sumrad + 1.0)) {
              float dpref = sumrad + 1.0;
              float ff1   = (dpref - d) * a2->radius / a->radius * a->grabbing *
                            0.005; //the radii come in here for inertia-like effect
              float ff2   = (dpref - d) * a->radius / a2->radius * a->grabbing *
                            0.005;
              a->pos.x -= (a2->pos.x - a->pos.x) * ff1;
              a->pos.y -= (a2->pos.y - a->pos.y) * ff1;
              a2->pos.x += (a2->pos.x - a->pos.x) * ff2;
              a2->pos.y += (a2->pos.y - a->pos.y) * ff2;

              a->borderRectify();
              a2->borderRectify();

              //"grab" (hehe) the coords of the other unit
              //a->grabx= a2->pos.x; a->graby= a2->pos.y;
            }
          } else if (a->grabID == a2->id) {
            a->grabID = -1;
          }
        } else {
          a->grabID = -1;
        }
      }
    }
  }
}

void Evolve::World::healthTick() {
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int) units.size(); i++) {
    if (units[i].health > 0) {
      float baseloss = HEALTHLOSS_WHEELS *
                       (fabs(units[i].w1) + fabs(units[i].w2)) / 2;

      baseloss += (float) units[i].age / MAXAGE * HEALTHLOSS_AGING;
//			baseloss *= units[i].metabolism;
      if (units[i].boost) {
        baseloss *= HEALTHLOSS_BOOSTMULT;
      }
      baseloss += HEALTHLOSS_BRAINUSE * units[i].brainact;

      units[i].health -= baseloss;
      units[i].writeIfKilled("Killed by Natural Causes");
      if (units[i].health == 0)
        continue;

      //remove health from lack of "air"
      //straight up penalty to all bots for large population
      units[i].health -= HEALTHLOSS_NOOXYGEN * units.size() / TOOMANYBOTS;
      units[i].writeIfKilled("Killed by LackOf Oxygen");
      if (units[i].health == 0) continue; //unit died, we must move on!

      //process temperature preferences
      //calculate temperature at the units spot. (based on distance from horizontal equator)
      float dd         = 2.0 * abs(units[i].pos.y / conf::HEIGHT - 0.5);
      float discomfort = sqrt(abs(dd - units[i].temperature_preference));
      if (discomfort < 0.005) discomfort = 0;
      units[i].health -= HEALTHLOSS_BADTEMP * discomfort; //subtract from health
      units[i].writeIfKilled("Killed by Temp Discomfort");
    }
  }
}

void Evolve::World::setSelection(int type) {
  int maxi = -1;

  if (modcounter % 5 == 0) {
    if (type == Select::NONE) {
      _selection = -1;
    } else if (type == Select::OLDEST) {
      int      maxage = -1;
      for (int i      = 0; i < (int) units.size(); i++) {
        if (units[i].health > 0 && units[i].age > maxage) {
          maxage = units[i].age;
          maxi   = i;
        }
      }
    } else if (type == Select::BEST_GEN) {
      int      maxgen = 0;
      for (int i      = 0; i < (int) units.size(); i++) {
        if (units[i].health > 0 && units[i].gencount > maxgen) {
          maxgen = units[i].gencount;
          maxi   = i;
        }
      }
    } else if (type == Select::HEALTHY) {
      float    maxhealth = -1;
      for (int i         = 0; i < (int) units.size(); i++) {
        if (units[i].health > 0 && units[i].health > maxhealth) {
          maxhealth = units[i].health;
          maxi      = i;
        }
      }
    } else if (type == Select::PRODUCTIVE) {
      float    maxprog = -1;
      for (int i       = 0; i < (int) units.size(); i++) {
        if (units[i].health > 0 && units[i].age != 0) {
          if (((float) units[i].children / (float) units[i].age) > maxprog) {
            maxprog = (float) units[i].children / (float) units[i].age;
            maxi    = i;
          }
        }
      }
    } else if (type == Select::AGGRESSIVE) {
      float    maxindex = 0;
      for (int i        = 0; i < (int) units.size(); i++) {
        float index = 0.25 * units[i].hits + units[i].killed;
        if (units[i].health > 0 && index > maxindex) {
          maxindex = index;
          maxi     = i;
        }
      }
    } else if (type == Select::RELATIVE) {
      int select = getSelectedUnit();
      if (select >= 0) {
        if (units[select].health <= 0) {
          int index = getClosestRelative(select);
          if (index >= 0) {
            maxi = index;
            printf("---> New relative selected.\n");
          } else {
            setSelectedUnit(randi(0, units.size()));
            addEvent("No More Living Relatives!");
          }
        }
      }
    }
  }

  if (maxi != -1 && units[maxi].id != _selection) {
    setSelectedUnit(maxi);
  }
}

void Evolve::World::setSelectedUnit(int idx) {
  if (units[idx].id != _selection)
    _selection = units[idx].id;

  if (units[idx].health > 0)
    units[idx].initEvent(10, 1.0, 1.0, 1.0);
  setControl(false);
}

int Evolve::World::getSelectedUnit() const {
  int idx = -1;
#pragma omp parallel for
  for (int i = 0; i < (int) units.size(); i++) {
    if (units[i].id == _selection) idx = i;
  }
  return idx;
}

int Evolve::World::getClosestRelative(int idx) const { (void) idx; }

int Evolve::World::getSelection() const { return _selection; }

void Evolve::World::selectedHeal() {
  int sidx = getSelectedUnit();
  if (sidx >= 0) {
    units[sidx].health = 2.0;
  }
}

void Evolve::World::selectedKill() {
  int sidx = getSelectedUnit();
  if (sidx >= 0) {
    units[sidx].health = -0.01;
    units[sidx].writeIfKilled("Killed by God (you)");
  }
}

void Evolve::World::selectedBabys() {
  int sidx = getSelectedUnit();
  if (sidx >= 0) {
    reproduce(sidx, sidx);
  }
}

void Evolve::World::selectedMutate() {
  int sidx = getSelectedUnit();
  if (sidx >= 0) {
    units[sidx].brain.liveMutate(units[sidx].MUTRATE1, units[sidx].MUTRATE2,
                                 units[sidx].out);
    units[sidx].initEvent(30, 0.5, 0, 1.0);
  }
}

void Evolve::World::getFollowLocation(float &xi, float &yi) {
  int sidx = getSelectedUnit();
  if (sidx >= 0) {
    xi = units[sidx].pos.x;
    yi = units[sidx].pos.y;
  }
}

void Evolve::World::draw(IView *view, int layer) {
  view->drawData();

  //draw cell layer
  if (layer != 0) {
    for (int x = 0; x < CW; x++) {
      for (int y = 0; y < CH; y++) {
        float val = 0;
        if (layer == Layer::PLANTS + 1) {
          //plant food
          val = 0.5 * cells[Layer::PLANTS][x][y] / conf::FOODMAX;
        } else if (layer == Layer::MEATS + 1) {
          //meat food
          val = 0.5 * cells[Layer::MEATS][x][y] / conf::MEATMAX;
        } else if (layer == Layer::HAZARDS + 1) {
          //hazards
          val = 0.5 * cells[Layer::HAZARDS][x][y] / conf::HAZARDMAX;
        } else if (layer == Layer::FRUITS + 1) {
          //fruit food
          val = 0.5 * cells[Layer::FRUITS][x][y] / conf::FRUITMAX;
        } else if (layer == Layer::LAND + 1) {
          //land
          val = cells[Layer::LAND][x][y];
        } else if (layer == Layer::LIGHT + 1) {
          //light
          val = cells[Layer::LIGHT][x][y];
        } else if (layer == Layer::TEMPLAYER) {
          //temperature
          val = cells[Layer::TEMPLAYER][x][y];
        }

        view->drawCell(x, y, val);
      }
    }
  }

  //draw all units
  vector<Unit>::const_iterator it;
  for (it = units.begin(); it != units.end(); ++it) {
    view->drawUnit(*it, it->pos.x, it->pos.y);
  }

  view->drawStatic();
}

void Evolve::World::addUnits(int num, int set_stomach, float nx, float ny,
                             bool set_lungs) {
  t_brain settings;
  settings.nbneuron    = conf::BRAINSIZE;
  settings.inputsize   = Input::INPUT_SIZE;
  settings.outputsize  = Output::OUTPUT_SIZE;
  settings.directinput = conf::BRAIN_DIRECTINPUT;
  settings.deadconns   = conf::BRAIN_DEADCONNS;
  settings.changeconn  = conf::BRAIN_CHANGECONNS;
  settings.memconn     = conf::BRAIN_MEMCONN;
  settings.learnrate   = conf::LEARNRATE;
  for (int i = 0; i < num; i++) {
    Unit a(settings, MEANRADIUS, REPRATE, MUTCHANCE, MUTSIZE);

    if (set_stomach == Stomach::PLANT)
      a.setHerbivore(); //if told to predetermine stomach type
    else if (set_stomach == Stomach::MEAT) a.setCarnivore();
    else if (set_stomach == Stomach::FRUIT) a.setFrugivore();

    if (set_lungs) { //if told to fix lungs for unit's position
      int scx = (int) a.pos.x / conf::CZ;
      int scy = (int) a.pos.y / conf::CZ;
      a.lungs = cap(randn((float) cells[Layer::LAND][scx][scy], 0.5));
    }

    if (nx != -1 || ny != -1) { //if custom location given
      a.pos.x = nx;
      a.pos.y = ny;
    }

    a.id = idcounter++;
    units.push_back(a);
  }
}

void Evolve::World::reproduce(int i1, int i2) {
  Unit mother = units[i1]; //mother is reproducing unit. her health, her repcounter, and her base stats are divided/reset/used
  Unit father = units[i2]; //father is a nearby unit that may have been sexting, or just the mother again. either way, repcounter is reset

  float healthloss = 0; //health lost by mother for assexual reproduction
  if (i1 == i2) { //if assexual rep, father is just the mother again
    father     = units[i1];
    healthloss = units[i1].radius / conf::MEANRADIUS * HEALTHLOSS_ASSEX;

    units[i1].initEvent(30, 0, 0.8,
                        0); //green event means unit asexually reproduced

  } else { //otherwise, it's sexual
    units[i1].initEvent(30, 0, 0, 0.8);
    units[i2].initEvent(30, 0, 0,
                        0.8); //blue events mean units sexually reproduced.
  }

  float newhealth = units[i1].health / (units[i1].numbabies + 1) -
                    mother.repcounter / conf::REPRATE;
  //repcounter should be negative or zero here, so its actually giving more health

  units[i1].health = newhealth - healthloss;
  units[i1].writeIfKilled("Killed by Child Birth");

  if (_selection == units[i1].id)
    printf("The Selected Unit has Reproduced and had %i Babies!\n",
           units[i1].numbabies);

#pragma omp parallel for ordered schedule(dynamic) //allow the creation of each baby to happen on a new thread
  for (int i = 0; i < units[i1].numbabies; i++) {
    Unit daughter = mother.reproduce(father, REPRATE);

#pragma omp ordered //restore order and collapse threads
    {
      if (i1 != i2) {
        daughter.hybrid = true; //if parents are not the same unit (sexual reproduction), then mark the child
        units[i2].children++;
      }

      daughter.health = newhealth;

      units[i1].children++;

      daughter.id = idcounter;
      idcounter++;
      units.push_back(daughter);
    }
  }
}

void Evolve::World::writeReport() {
  printf("Writing Report, Epoch: %i\n", current_epoch);
  //save all kinds of nice data stuff
  int   randgen   = 0;
  int   randspec  = 0;
  int   randseed  = 0;
  float randmetab = 0;
  float randred   = 0;
  float randgre   = 0;
  float randblu   = 0;
  //int happyspecies=0;
  //vector<int> species;
  //vector<int> members;
  //species.resize(1,0);
  //members.resize(1,0);

//	for (int i=0; i<(int)units.size(); i++) {
  /*for(int s=0;s<(int)species.size();s++){ //for every logged species...

      int speciesdiff= abs(species[s]-units[i].species);

      if(speciesdiff>MAXDEVIATION){ //is our bot not near any of them?...
          if(s==species.size()-1){ //if so, let's make sure we've checked all logged species...
              species.push_back(units[i].species); //and add this one if it's unique enough.
              members.push_back(1);
              break;
          }

      } else { //If our bot is actually a member of an already logged species, then we increment the member array...
          members[s]++;
          //and make the species num an average to better find other members
//				species[s]= (int) (species[s] + units[i].species) / members[s];
          //we will then make sure that only species with at least 3 members are counted
          break;
      }
  }*/
//	}
  randgen  = units[randi(0, units.size())].gencount;
  randspec = units[randi(0, units.size())].species;
  int      randunit           = randi(0, units.size());
  for (int i                  = 0;
       i < (int) units[randunit].brain._neurons.size(); i++) {
    if (units[randunit].brain._neurons[i].seed > randseed)
      randseed = units[randunit].brain._neurons[i].seed;
  }
  randmetab                   = units[randi(0, units.size())].metabolism;
  randred                     = units[randi(0, units.size())].red;
  randgre                     = units[randi(0, units.size())].gre;
  randblu                     = units[randi(0, units.size())].blu;

/*	for(int s=0;s<(int)species.size();s++){
		if(members[s]>=3) happyspecies++; //3 units with the same species id counts as a species
	}*/

  //death cause reporting
  std::vector<const char *> deathlog;
  std::vector<int>          deathcounts;
  for (int                  d = 0; d < (int) deaths.size(); d++) {
    bool     added = false;
    for (int e     = 0; e < (int) deathlog.size(); e++) {
      if (added) continue;
      if (deaths[d] == deathlog[e]) {
        deathcounts[e]++;
        added = true;
      }
    }
    if (!added) {
      deathlog.push_back(deaths[d]);
      deathcounts.push_back(1);
    }
  }

  FILE *fr = fopen("report.txt", "a");
  //print basics: Epoch and Unit counts
  fprintf(fr,
          "Epoch:\t%i\t#Units:\t%i\t#Herbi:\t%i\t#Frugi:\t%i\t#Carni:\t%i\t#Terra:\t%i\t#Aqua:\t%i\t#Hybrids:\t%i\t#Spikes:\t%i\t",
          current_epoch, getAlive(), getHerbivores(), getFrugivores(),
          getCarnivores(), getLungLand(), getLungWater(), getHybrids(),
          getSpiked());
  //print world stats: cell counts
  fprintf(fr,
          "#0.75Plant:\t%i\t#0.5Meat:\t%i\t#0.5Hazard:\t%i\t#0.5Fruit:\t%i\t",
          getFood(), getMeat(), getHazards(), getFruit());
  //print random selections: Genome, brain seeds, [[[generation]]]
  fprintf(fr,
          "RandGenome:\t%i\tRandSeed:\t%i\tRandGen:\t%i\tRandMetabolism:\t%f\tRandColorR:\t%f\tRandColorG:\t%f\tRandColorB:\t%f\t",
          randspec, randseed, randgen, randmetab, randred, randgre, randblu);
  //print generations: Top Gen counts
  fprintf(fr,
          "TopHGen:\t%i\tTopFGen:\t%i\tTopCGen:\t%i\tTopLGen:\t%i\tTopAGen:\t%i\t",
          stats.bestherbi, stats.bestfrugi, stats.bestcarni, stats.bestterran,
          stats.bestaquatic);
  //print deaths: Number and Causes
  fprintf(fr, "#Deaths:\t%i\tDEATHLOG:\t",
          deaths.size());
  for (int e = 0; e < (int) deathlog.size(); e++) {
    fprintf(fr, "%s:\t%i\t", deathlog[e], deathcounts[e]);
  }
  fprintf(fr, "\n");
  fclose(fr);
}

void Evolve::World::setClosed(bool close) {
  _closed = close;
}

bool Evolve::World::isClosed() const {
  return _closed;
}

int Evolve::World::getHerbivores() const {
  return stats.herbivores;
}

int Evolve::World::getFrugivores() const {
  return stats.frugivores;
}

int Evolve::World::getCarnivores() const {
  return stats.carnivores;
}

int Evolve::World::getLungLand() const {
  return stats.terrans;
}

int Evolve::World::getLungWater() const {
  return stats.aquatic;
}

int Evolve::World::getUnits() const {
  return units.size();
}

int Evolve::World::getHybrids() const {
  return stats.hybrids;
}

int Evolve::World::getSpiked() const {
  return stats.spiked;
}

int Evolve::World::getDead() const {
  return stats.dead;
}

int Evolve::World::getAlive() const {
  return stats.alive;
}

int Evolve::World::getFood() const //count plant cells with 75% max or more
{
  return stats.plants;
}

int Evolve::World::getFruit() const //count fruit cells with 50% max or more
{
  return stats.fruits;
}

int Evolve::World::getMeat() const //count meat cells with 50% max or more
{
  return stats.meats;
}

int Evolve::World::getHazards() const //count hazard cells with 50% max or more
{
  return stats.hazards;
}

float Evolve::World::getFoodSupp() const {
  return stats.allplant * FOODINTAKE / 2 *
         10;// Stat*INTAKE/maxhealth*statfrequency
}

float Evolve::World::getFruitSupp() const {
  return stats.allfruit * FRUITINTAKE / 2 * 10;
}

float Evolve::World::getMeatSupp() const {
  return stats.allmeat * MEATINTAKE / 2 * 10;
}

float Evolve::World::getHazardSupp() const {
  return stats.allhazard * HAZARDDAMAGE / 2 * 10;
}

void Evolve::World::findStats() {
  //clear old stats
  stats.herbivores  = 0;
  stats.frugivores  = 0;
  stats.carnivores  = 0;
  stats.terrans     = 0;
  stats.aquatic     = 0;
  stats.alive       = 0;
  stats.dead        = 0;
  stats.spiked      = 0;
  stats.hybrids     = 0;
  stats.bestherbi   = 0;
  stats.bestfrugi   = 0;
  stats.bestcarni   = 0;
  stats.bestterran  = 0;
  stats.bestaquatic = 0;
  stats.besthybrid  = 0;
  stats.plants      = 0;
  stats.fruits      = 0;
  stats.meats       = 0;
  stats.hazards     = 0;
  stats.allplant    = 0;
  stats.allfruit    = 0;
  stats.allmeat     = 0;
  stats.allhazard   = 0;

  //units
  for (int i = 0; i < (int) units.size(); i++) {
    if (units[i].health > 0) {
      stats.alive++;

      if (units[i].isHerbivore()) {
        stats.herbivores++;
        if (units[i].gencount > stats.bestherbi)
          stats.bestherbi = units[i].gencount;
      } else if (units[i].isFrugivore()) {
        stats.frugivores++;
        if (units[i].gencount > stats.bestfrugi)
          stats.bestfrugi = units[i].gencount;
      } else if (units[i].isCarnivore()) {
        stats.carnivores++;
        if (units[i].gencount > stats.bestcarni)
          stats.bestcarni = units[i].gencount;
      }

      if (units[i].isTerran()) {
        stats.terrans++;
        if (units[i].gencount > stats.bestterran)
          stats.bestterran = units[i].gencount;
      } else if (units[i].isAquatic()) {
        stats.aquatic++;
        if (units[i].gencount > stats.bestaquatic)
          stats.bestaquatic = units[i].gencount;
      }

      if (units[i].isSpikey(SPIKELENGTH)) stats.spiked++;

      if (units[i].hybrid) {
        stats.hybrids++;
        if (units[i].gencount > stats.besthybrid)
          stats.besthybrid = units[i].gencount;
      }
    } else stats.dead++;
  }

  //cell layers
  for (int i = 0; i < CW; i++) {
    for (int j = 0; j < CH; j++) {
      if (cells[Layer::PLANTS][i][j] > conf::FOODMAX * 0.75) stats.plants++;
      if (cells[Layer::FRUITS][i][j] > conf::FRUITMAX * 0.5) stats.fruits++;
      if (cells[Layer::MEATS][i][j] > conf::MEATMAX * 0.5) stats.meats++;
      if (cells[Layer::HAZARDS][i][j] > conf::HAZARDMAX * 0.5) stats.hazards++;
      stats.allplant += cells[Layer::PLANTS][i][j];
      stats.allfruit += cells[Layer::FRUITS][i][j];
      stats.allmeat += cells[Layer::MEATS][i][j];
      stats.allhazard += cells[Layer::HAZARDS][i][j];
    }
  }
}

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
  return _autoselect;
}

void Evolve::World::setAutoselect(bool state) {
  _autoselect = state;
}

void Evolve::World::cellsRandomFill(int layer, float amount, int number) {
  for (int i = 0; i < number; i++) {
    int cx = randi(0, CW);
    int cy = randi(0, CH);
    cells[layer][cx][cy] = amount;
  }
}

void Evolve::World::setDebug(bool state) {
  _debug = state;
}

bool Evolve::World::isDebug() const {
  return _debug;
}

void Evolve::World::addEvent(const char *text) {
  std::pair<const char *, int> data;
  data.first  = text;
  data.second = 2;
  events.push_back(data);
}

bool
Evolve::World::processMouse(int button, int state, int x, int y, float scale) {
  if (button == 0 && state == 1) {
    float mind = 1e10;
    int   mini = -1;
    float d;

    for (int i = 0; i < (int) units.size(); i++) {
      d = pow(x - units[i].pos.x, 2) + pow(y - units[i].pos.y, 2);
      if (d < mind) {
        mind = d;
        mini = i;
      }
    }
    if (mind < 3500 / scale) {
      setSelectedUnit(mini);
      return true;
    } else return false;
  } else return false;
}