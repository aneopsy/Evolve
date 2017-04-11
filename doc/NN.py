import numpy as np

alphas = [1]
hiddenSize = 150

def sigmoid(x, deriv=False):
    if (deriv):
        return x*(1-x)
    return (1 / (1 + np.exp(-x)))

X = np.array([[0,0],
              [0,1],
              [1,0],
              [1,1]])

y = np.array([[0],
              [1],
              [1],
              [0]])

for alpha in alphas:
    print ("\nTraining With Alpha:" + str(alpha))
    np.random.seed(2)

    synapse_0 = 2 * np.random.random((2, hiddenSize)) - 1
    synapse_1 = 2 * np.random.random((hiddenSize, 1)) - 1

    for j in range(50000):
        layer_0 = X
        layer_1 = sigmoid(np.dot(layer_0,synapse_0))
        layer_2 = sigmoid(np.dot(layer_1,synapse_1))

        layer_2_error = layer_2 - y

        if (j% 10000) == 0:
            print ("Error after "+str(j)+" iterations:" + str(np.mean(np.abs(layer_2_error))))

        layer_2_delta = layer_2_error * sigmoid(layer_2, True)
        layer_1_error = layer_2_delta.dot(synapse_1.T)

        layer_1_delta = layer_1_error * sigmoid(layer_1, True)

        synapse_1 -= alpha * (layer_1.T.dot(layer_2_delta))
        synapse_0 -= alpha * (layer_0.T.dot(layer_1_delta))
    print ("Error after "+str(j)+" iterations:" + str(np.mean(np.abs(layer_2_error))))
    print (layer_2)
