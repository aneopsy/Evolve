//
// Created by Aneosy on 31/03/2017.
//
#include <iostream>

#include <Utils.hpp>
#include <DRAWS.hpp>

DRAWS::Neuron::Neuron(t_brain const &settings) {
    w.resize(CONNS, 0);
    id.resize(CONNS, 0);
    type.resize(CONNS, 0);

    for (int i = 0; i < CONNS; i++) {
        w[i] = randf(-10, 10);
        if (randf(0, 1) < settings.deadconns)
            w[i] = 0;

        id[i] = randi(0, settings.nbneuron);
        if (randf(0, 1) < settings.directinput)
            id[i] = randi(0, settings.inputsize);

        type[i] = 0;
        if (randf(0, 1) < settings.changeconn)
            type[i] = 1;
        if (randf(0, 1) <settings.memconn)
            type[i] = 2;
    }

    seed = (randf(0,1)>0.5)?(0):(1);
    kp = randf(0.01, 1);
    gw = randf(-2, 2);
    bias = randf(-1, 1);

    out = 0;
    oldout = 0;
    target = 0;
}

DRAWS::Brain::Brain(t_brain const &settings) : _brain(settings){
    for (int i = 0; i < _brain.nbneuron; i++) {
        Neuron neuron = Neuron(_brain);
        _neurons.push_back(neuron);
    }
}

DRAWS::Brain::~Brain() {}

DRAWS::Brain::Brain(const DRAWS::Brain &other) {
    _neurons = other._neurons;
}

DRAWS::Brain                &DRAWS::Brain::operator=(const DRAWS::Brain &other) {
    if (this != &other)
        _neurons = other._neurons;
    return (*this);
}

void                        DRAWS::Brain::tick(std::vector<float> &in, std::vector<float> &out) {
    for (int j=0; j<(int)_neurons.size(); j++){
      DRAWS::Neuron* abox= &_neurons[j];

      if (j<Input::INPUT_SIZE) { //take first few boxes and set their out to in[]. (no need to do these separately, since thay are first)
        abox->out= in[j];
      } else { //then do a dynamics tick and set all targets
        float acc=abox->bias;

        for (int k=0;k<CONNS;k++) {
          int idx=abox->id[k];
          int type = abox->type[k];
          float val= _neurons[idx].out;

          if(type==2){ //switch conn
            if(val>0.5){
              break;
              continue;
            }
            continue;
          }

          if(type==1){ //change sensitive conn
            val-= _neurons[idx].oldout;
            val*=10;
          }

          acc+= val*abox->w[k];
        }

        acc*= abox->gw;

        //put through sigmoid
        acc= 1.0/(1.0+exp(-acc));

        abox->target= cap(acc);
      }
    }


    for (int j=0; j<(int)_neurons.size(); j++){
      DRAWS::Neuron* abox= &_neurons[j];

      //back up current out for each box
      abox->oldout = abox->out;

      //make all boxes go a bit toward target
      if (j>=Input::INPUT_SIZE) abox->out+= (abox->target-abox->out)*abox->kp;
    }

    //finally set out[] to the last few boxes output
    for (int j=0;j<Output::OUTPUT_SIZE;j++) {
      //jump has different responce because we've made it into a change sensitive output
      if (j==Output::JUMP) out[j]= cap(10*(_neurons[_neurons.size()-1-j].out-_neurons[_neurons.size()-1-j].oldout));
      else out[j]= _neurons[_neurons.size()-1-j].out;
    }
  }

float DRAWS::Brain::getActivity() {
    float sum = 0;

    for (int j = 0; j < (int) _neurons.size(); j++) {
        Neuron *abox = &_neurons[j];
        sum += fabs(abox->out - abox->oldout);
    }
    return (sum / _neurons.size());
}

void DRAWS::Brain::initMutate(float mr, float mr2) {
    for (int j = 0; j < (int) _neurons.size(); j++) {
        Neuron *abox = &_neurons[j];
        if (randf(0, 1) < mr / 50) {
            int rc = randi(0, CONNS);
            abox->type[rc] = randi(0, 2);
            abox->seed = 0;
        }
        if (randf(0, 1) < mr / 40) {
            int k = randi(0, _neurons.size());
            if (k != j) {
                abox->type = _neurons[k].type;
                abox->id = _neurons[k].id;
                abox->bias = _neurons[k].bias;
                abox->kp = _neurons[k].kp;
                abox->type = _neurons[k].type;
                abox->w = _neurons[k].w;
                abox->seed = 0;
            }
        }
        if (randf(0, 1) < mr / 20) {
            int rc = randi(0, CONNS);
            int ri = randi(0, _neurons.size());
            abox->id[rc] = ri;
            abox->seed = 0;
        }
        if (randf(0, 1) < mr / 10) {
            int rc1 = randi(0, CONNS);
            int rc2 = randi(0, CONNS);
            int temp = abox->id[rc1];
            abox->id[rc1] = abox->id[rc2];
            abox->id[rc2] = temp;
            abox->seed = 0;
        }
        if (randf(0, 1) < mr / 2) {
            abox->gw += randn(0, mr2);
        }
        if (randf(0, 1) < mr) {
            abox->bias += randn(0, mr2 * 5);
        }
        if (randf(0, 1) < mr) {
            abox->kp += randn(0, mr2);
            if (abox->kp < 0.01) abox->kp = 0.01;
            if (abox->kp > 1) abox->kp = 1;
        }
        if (randf(0, 1) < mr) {
            int rc = randi(0, CONNS);
            abox->w[rc] += randn(0, mr2 * 5);
        }
    }
}

void DRAWS::Brain::liveMutate(float mr, float mr2, std::vector<float> &out) {
    int j = randi(0, _neurons.size());
    Neuron *abox = &_neurons[j];

    if (randf(0, 1) < mr) {
        int rc = randi(0, CONNS);
        int b = -1;
        while (b <= -1) {
            b -= 1;
            int rb = randi(0,_neurons.size());
            if (abs(_neurons[rb].oldout - abox->out) <= 0.01) b = rb;
            if (b <= -100) break;
        }
        if (b >= 0) {
            abox->id[rc] = b;
            abox->seed = 0;
        }
    }

    if (randf(0, 1) < mr) {
        float stim = out[Output::STIMULANT];
        if (stim > 0.5) {
            for (int k = 0; k < CONNS; k++) {
                float val = _neurons[abox->id[k]].out;
                abox->w[k] += _brain.learnrate * stim * (abox->oldout - (1 - val));
            }
        }
//		abox->seed= 0;
    }

    if (randf(0, 1) < mr * 5) {
        abox->bias += randn(0, mr2 * 10);
    }

    if (randf(0, 1) < mr * 5) {
        abox->kp += randn(0, mr2 * 10);
        if (abox->kp < 0.01) abox->kp = 0.01;
        if (abox->kp > 1) abox->kp = 1;
    }

    if (randf(0, 1) < mr * 10) {
        int rc = randi(0, CONNS);
        abox->w[rc] += randn(0, mr2 * 10);
    }
}

DRAWS::Brain DRAWS::Brain::crossover(const DRAWS::Brain &other) {
    DRAWS::Brain newbrain(*this);

#pragma omp parallel for
    for (size_t i = 0; i < newbrain._neurons.size(); i++) {
        if (i >= other._neurons.size())
            continue;
        int s1 = this->_neurons[i].seed;
        int s2 = other._neurons[i].seed;
        float threshold = ((s1 - s2) / (1 + abs(s1 - s2)) + 1) / 2;

        if (randf(0, 1) < threshold) {
            newbrain._neurons[i].bias = this->_neurons[i].bias;
            newbrain._neurons[i].gw = this->_neurons[i].gw;
            newbrain._neurons[i].kp = this->_neurons[i].kp;
            newbrain._neurons[i].seed = this->_neurons[i].seed + 1;
//			this->neurons[i].seed += 1; //reward the copied box
            for (int j = 0; j < CONNS; j++) {
                newbrain._neurons[i].id[j] = this->_neurons[i].id[j];
                newbrain._neurons[i].w[j] = this->_neurons[i].w[j];
                newbrain._neurons[i].type[j] = this->_neurons[i].type[j];
            }

        } else {
            newbrain._neurons[i].bias = other._neurons[i].bias;
            newbrain._neurons[i].gw = other._neurons[i].gw;
            newbrain._neurons[i].kp = other._neurons[i].kp;
            newbrain._neurons[i].seed = other._neurons[i].seed + 1;
//			other.neurons[i].seed += 1;
            for (int j = 0; j < CONNS; j++) {
                newbrain._neurons[i].id[j] = other._neurons[i].id[j];
                newbrain._neurons[i].w[j] = other._neurons[i].w[j];
                newbrain._neurons[i].type[j] = other._neurons[i].type[j];
            }
        }
    }
    return newbrain;
}

t_brain                 DRAWS::Brain::getBrain() const {
    return _brain;
}