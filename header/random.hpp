#ifndef C28EF1A2_07FD_41E5_9520_967B0802DC40
#define C28EF1A2_07FD_41E5_9520_967B0802DC40
#include <stdint.h>

typedef uint64_t (*rdm_func)(uint64_t seed, uint64_t cursor);

/// @brief genererique random function
class Random {
    private:
        uint64_t seed;
        uint64_t cursor;
        rdm_func random;
    public:
        Random(uint64_t seed, rdm_func random);
        ~Random();
        uint64_t next();
};

#endif /* C28EF1A2_07FD_41E5_9520_967B0802DC40 */
