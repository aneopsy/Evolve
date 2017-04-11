//
// Created by Aneosy on 31/03/2017.
//

#ifndef EVOLVE_UTILS_HPP
#define EVOLVE_UTILS_HPP

#include <stdlib.h>
#include <math.h>
#include <vmath.hpp>

inline float                    randf(float a, float b) {
    return ((b - a) * ((float) rand() / RAND_MAX)) + a;
}

inline int randi(int a, int b) {
    return (rand() % (b - a)) + a;
}

inline double                   randn(double mu, double sigma) {
    static bool                 is_deviate = false;
    static float                tmp_deviate;
    double                      polar;
    double                      rsquared;
    double                      var1;
    double                      var2;

    if (!is_deviate) {
        do {
            var1 = 2.0 * (double(rand()) / double(RAND_MAX)) - 1.0;
            var2 = 2.0 * (double(rand()) / double(RAND_MAX)) - 1.0;
            rsquared = ((var1 * var1) + (var2 * var2));
        } while ((rsquared >= 1.0) || (rsquared == 0.0));
        polar = sqrt(-2.0 * log(rsquared) / rsquared);
        tmp_deviate = (var1 * polar);
        is_deviate = true;
        return ((var2 * polar * sigma) + mu);
    } else {
        is_deviate = false;
        return ((tmp_deviate * sigma) + mu);
    }
}

inline float                    cap(float a) {
    if (a < 0x00)
        return (0x00);
    else if (a > 0x01)
        return (0x01);
    else
        return (a);
}

inline float                    capm(float a, float low, float hi) {
    if (a < low)
        return (low);
    else if (a > hi)
        return (hi);
    else
        return (a);
}

inline int                      fround(float a) {
    if (a - ((int) (abs(a))) >= 0.5)
        return ((int) (a) + 1);
    else
        return ((int) (a));
}

inline float                    pointline(Vector2f posA, Vector2f posB, Vector2f posC) {
    float                       normalLength = (posA - posB).length();
    return (fabs((posC.x - posA.x) * (posB.y - posA.y) - (posC.y - posA.y) * (posB.x - posA.x)) / normalLength);
}

#endif //EVOLVE_UTILS_HPP
