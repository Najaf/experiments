#include <stdint.h>
#include <stdio.h>
#define RNG_STATE_MAX 624

typedef struct RNG {
  int index;
  uint32_t state[RNG_STATE_MAX];
} RNG;

void initRNG(RNG *rng, uint32_t seed) {
  rng->index = 0;
  rng->state[0] = seed;
  for (int i = 1; i < RNG_STATE_MAX - 1; i++) {
    //since we only have 32 bits in our uinut32_t, assume the 'last 32 bits of' part is superfluous
    rng->state[i] = 0x6c078965 * (rng->state[i-1] ^ ((rng->state[i-1])  >> 30) + 1);
  }
}

void generateNumbers(RNG *rng) {
  for (int i = 0; i < RNG_STATE_MAX - 1; i++) {
    uint32_t x = (rng->state[i] & 0x80000000) + (rng->state[(i+1) % RNG_STATE_MAX] & 0x7fffffff);
    rng->state[i] = rng->state[(i + 397) % 624] ^ (x >> 1);
    if ((x % 2) != 0) {
      rng->state[i] = rng->state[i] ^ 0x9908b0df;
    }
  }
}

uint32_t temperNumber(uint32_t x) {
  x ^=  x >> 11;
  x ^= (x << 7)  & 0x9d2c5680;
  x ^= (x << 15) & 0xefc60000;
  x ^=  x >> 18;
  return x;
}

uint32_t extractNumber(RNG *rng) {
  if (rng->index == 0) {
    generateNumbers(rng);
  }

  uint32_t x = rng->state[rng->index];
  x = temperNumber(x);
  rng->index = (rng->index + 1) % RNG_STATE_MAX;
  return x;
}



int main() {
  RNG rng;
  initRNG(&rng, 123123123);
  printf("%u\n", extractNumber(&rng));
  printf("%u\n", extractNumber(&rng));
  printf("%u\n", extractNumber(&rng));

}
