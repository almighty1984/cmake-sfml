module;
#include <random>
#include <chrono>

export module random;
import types;

export namespace random {
    export class Generator {
    public:
        static void init() {
            s_rand_engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
        }
        static void seed(u32c s) {
            s_rand_engine.seed(s);
        }
        static u32 number(u32c min, u32c max) {
            std::uniform_int_distribution<u32> dist(min, max);
            return dist(s_rand_engine);
        }
        static f64 real_number(f64c min, f64c max) {
            std::uniform_real_distribution<f64> dist(min, max);
            return dist(s_rand_engine);
        }
    private:
        static inline std::default_random_engine s_rand_engine;
    };
}