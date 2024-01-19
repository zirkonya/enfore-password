#include "random.hpp"

/// @brief generic random function, build with seed and the random function to use
/// @param seed the seed used to generate random
/// @param random the random function to use
Random::Random(uint64_t seed, rdm_func random) :
    seed(seed), cursor(0), random(random)
{}

/// @brief destructor of Random
Random::~Random()
{
    this->seed = 0;
    this->cursor = 0;
}

/// @brief calculate a random value using the function, and increment the cursor
/// @return the next random value
uint64_t Random::next()
{
    return this->random(Random::seed, Random::cursor++);
}