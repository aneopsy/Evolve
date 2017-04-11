//
// Created by Aneosy on 02/04/2017.
//

#include <DWRAON.hpp>
#include <Utils.hpp>

DWRAON::Neuron::Neuron(t_brain const &settings){
        w.resize(CONNS,0);
        id.resize(CONNS,0);
        type.resize(CONNS,0);

        for(int i=0;i<CONNS;i++){
            w[i]= randf(0.1,2);
            id[i]= randi(0, settings.nbneuron);
            if(randf(0,1)<0.2)
                id[i]= randi(0, settings.inputsize);
            type[i]= randf(0,1)<0.5;
        }

        genre = (randf(0,1) > 0.5) ? (0) : (1);
        kp= randf(0.8, 1);
        bias= randf(-1, 1);

        out=0;
        target=0;
    }


DWRAON::Brain::Brain(t_brain const &settings) : _brain(settings){
    for (int i = 0; i < _brain.nbneuron; i++) {
        Neuron a(_brain);
        _neurons.push_back(a);
        _neurons[i].out = a.out;
        _neurons[i].target = a.target;
        _neurons[i].genre = a.genre;
        _neurons[i].bias = a.bias;
        for (int j = 0; j < CONNS; j++) {
            _neurons[i].type[j] = a.type[j];
            _neurons[i].w[j] = a.w[j];
            _neurons[i].id[j] = a.id[j];
            if (randf(0, 1) < 0.05)
                _neurons[i].id[j] = 0;
            if (randf(0, 1) < 0.05)
                _neurons[i].id[j] = 5;
            if (randf(0, 1) < 0.05)
                _neurons[i].id[j] = 12;
            if (randf(0, 1) < 0.05)
                _neurons[i].id[j] = 4;
            //boxes[i].id[j]= max(min(BRAINSIZE-1, randi(i-10,i+10)), 0);
            if (i < _brain.nbneuron / 2) {
                _neurons[i].id[j] = randi(0, _brain.inputsize);
            }
        }
    }
}

DWRAON::Brain::~Brain() {}

DWRAON::Brain                   &DWRAON::Brain::operator=(const DWRAON::Brain &other) {
    if (this != &other)
        _neurons = other._neurons;
    return (*this);
}

void                            DWRAON::Brain::tick(std::vector<float> &in, std::vector<float> &out) {
    for (int i = 0; i < _brain.inputsize; i++) {
        _neurons[i].out = in[i];
    }
    for (int i = _brain.inputsize; i < _brain.nbneuron; i++) {
        Neuron *abox = &_neurons[i];
        if (abox->genre == 0) {
            float res = 1;
            for (int j = 0; j < CONNS; j++) {
                int idx = abox->id[j];
                float val = _neurons[idx].out;
                if (abox->type[j]) val = 1 - val;
                //res= res * pow(val, abox->w[j]);
                res = res * val;
            }
            res *= abox->bias;
            abox->target = res;

        } else {
            float res = 0;
            for (int j = 0; j < CONNS; j++) {
                int idx = abox->id[j];
                float val = _neurons[idx].out;
                if (abox->type[j]) val = 1 - val;
                res = res + val * abox->w[j];
            }
            res += abox->bias;
            abox->target = res;
        }
        if (abox->target < 0)
            abox->target = 0;
        if (abox->target > 1)
            abox->target = 1;
    }
    for (int i = _brain.inputsize; i < _brain.nbneuron; i++) {
        Neuron *abox = &_neurons[i];
        abox->out = abox->out + (abox->target - abox->out) * abox->kp;
    }
    for (int i = 0; i < _brain.outputsize; i++) {
        out[i] = _neurons[_brain.nbneuron - 1 - i].out;
    }
}

float DWRAON::Brain::getActivity() {
    float sum = 0;

    for (int j = 0; j < (int) _neurons.size(); j++) {
        Neuron *abox = &_neurons[j];
        sum += fabs(abox->out - abox->oldout);
    }
    return (sum / _neurons.size());
}