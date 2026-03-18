#ifndef __RNG_HPP__
#define __RNG_HPP__

#include <random>

class RNG{
public:
    RNG(size_t seed){ setSeed(seed); }
    RNG() : RNG(0) {};

    void setSeed(size_t seed){ gen.seed(seed); }
    float uniform()const { return uniform_distribution(gen); }
private:
    mutable std::mt19937 gen;
    mutable std::uniform_real_distribution<float> uniform_distribution {0, 1};
};

#endif // __RNG_HPP__