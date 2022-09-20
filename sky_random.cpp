// SECTION: PRNG

namespace lcg {
    static uint64_t rand_seed = (uint64_t)rand();
    uint64_t a = 1664525;
    uint64_t c = 1013904223;
    uint64_t m = (uint64_t)1 << 32;
    static void UNUSED_FUNCTION(sky_srand)(uint64_t seed) {
        rand_seed = seed;
    }
    static uint64_t UNUSED_FUNCTION(sky_rand)() {
        rand_seed = ((a * rand_seed) + c) % m;
        return rand_seed;
    }
    static float UNUSED_FUNCTION(sky_rand_float)() {
        return (float)rand() / m; 
    
    }
}
namespace xorshift128plus {
    static uint64_t s[2] = { (uint64_t)rand(), (uint64_t)rand() };
    
    static void UNUSED_FUNCTION(sky_srand)(uint64_t seed) {
        s[0] = seed;
        s[1] = 0;
    }
        
    static uint64_t sky_rand() {
        uint64_t a = s[0];
        uint64_t b = s[1];

        s[0] = b;
        a ^= a << 23;
        a ^= a >> 18;
        a ^= b;
        a ^= b >> 5;
        s[1] = a;

        return a + b;
    }
}



