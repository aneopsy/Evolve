#!/usr/bin/env python
import sys, os
sys.path.insert(1, os.path.join(sys.path[0], '..'))
from mlp import MLP, Layer

def main():
    xor = MLP()
    cnf = lambda: 0
    xor.add_layer(Layer(2))
    xor.add_layer(Layer(2, cnf))
    xor.add_layer(Layer(1))

    xor.add_bias()
    xor.init_network()

    xor.patterns = [
        ([0, 0], [0]),
        ([0, 1], [1]),
        ([1, 0], [1]),
        ([1, 1], [0]),
    ]

    print (xor.train_target(xor.patterns))
    for inp, target in xor.patterns:
        tolerance = 0.1
        computed = xor.run(inp)
        error = abs(computed[0] - target[0])
        print ('input: %s target: %s, output: %s, error: %.4f' % (inp,
            target, computed, error))

if __name__ == '__main__':
    main()
