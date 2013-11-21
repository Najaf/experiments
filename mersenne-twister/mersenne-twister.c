#include <stdint.h>
#include <stdio.h>
#define RNG_STATE_MAX 624
#define INT_MAX_32 4294967295


typedef struct RNG {
  int index;
  uint32_t state[RNG_STATE_MAX];
} RNG;

void initRNGWithSeed(RNG *rng, uint32_t seed) {
  rng->index = 0;
  rng->state[0] = seed;
  for (int i = 1; i < RNG_STATE_MAX; i++) {
    //since we only have 32 bits in our uinut32_t, assume the 'last 32 bits of' part is superfluous
    rng->state[i] = 0x6c078965 * (rng->state[i-1] ^ ((rng->state[i-1])  >> 30) + 1);
  }
}

void generateNumbers(RNG *rng) {
  for (int i = 0; i < RNG_STATE_MAX; i++) {
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

uint32_t reverseBitshiftRightXOR(uint32_t x, int shift) {
  uint32_t y = 0;

  for (int i = 0; (i * shift) < 32; i++) {
    uint32_t mask = ((uint32_t)(INT_MAX_32 << (32 - shift))) >> (i * shift);
    uint32_t segment = x & mask;
    x ^= segment >> shift;
    y |= segment;
  }
  return y;
}

uint32_t reverseBitshiftLeftXOR(uint32_t x, uint32_t mask, int shift) {
  uint32_t y = 0;
  for (int i = 0; (i * shift) < 32; i++) {
    uint32_t segmentMask = ((uint32_t)(INT_MAX_32 >> (32 - shift))) << (i * shift);
    uint32_t segment = x & segmentMask;
    x ^= (segment << shift) & mask;
    y |= segment;
  }
  return y;
}


uint32_t untemperNumber(uint32_t x) {
  x = reverseBitshiftRightXOR(x, 18);
  x = reverseBitshiftLeftXOR(x, 0xefc60000, 15);
  x = reverseBitshiftLeftXOR(x, 0x9d2c5680, 7);
  x = reverseBitshiftRightXOR(x, 11);
  return x;
}

void initRNGWithOutput(RNG *rng, uint32_t output[RNG_STATE_MAX]) {
  rng->index = 0;
  for (int i = 0; i < RNG_STATE_MAX; i++) {
    rng->state[i] = untemperNumber(output[i]);
  }
}

void cloneRNG(RNG *rng1, RNG *rng2) {
  uint32_t output[RNG_STATE_MAX];
  for (int i = 0; i < RNG_STATE_MAX; i++) {
    output[i] = extractNumber(rng1);
  }
  initRNGWithOutput(rng2, output);
  rng2->index = rng1->index;
}

int main() {
  RNG rng1;
  RNG rng2;
  initRNGWithSeed(&rng1, 123123123);
  cloneRNG(&rng1, &rng2);
  printf(
    "rng1: %X %X %X %X\n",
    extractNumber(&rng1),
    extractNumber(&rng1),
    extractNumber(&rng1),
    extractNumber(&rng1)
  );
  printf(
    "rng2: %X %X %X %X\n",
    extractNumber(&rng2),
    extractNumber(&rng2),
    extractNumber(&rng2),
    extractNumber(&rng2)
  );
}
