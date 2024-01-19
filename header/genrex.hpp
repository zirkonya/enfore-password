#ifndef ACECE6C4_FDDF_41F2_BB4C_A6CB8096A83F
#define ACECE6C4_FDDF_41F2_BB4C_A6CB8096A83F
#include <vector>
#include <stdint.h>
#include <string>
#define UNIT_MAXLEN 100
#include "random.hpp"

namespace genrex
{
    /// @brief a regex unit represent characteres and length
    class RegUnit {
        public:
            std::vector<char> chars;
            int32_t min, max;
            RegUnit();
            ~RegUnit();
            
    };

    std::vector<RegUnit*> compile(char* pattern);
    void free_regunits(std::vector<RegUnit*> units);
    
    /// @brief the string generator by regex
    class GenRex {
        private:
            std::vector<RegUnit*> pattern;
            Random random;
        public:
            GenRex(char* pattern, Random random);
            ~GenRex();
            std::string generate_string();
    };    
} // namespace genrex

#endif /* ACECE6C4_FDDF_41F2_BB4C_A6CB8096A83F */
