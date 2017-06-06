#include "xorshift.h"

static unsigned long long state0 = 1;
static unsigned long long state1 = 2;

unsigned long long xorshift128plus() {
	unsigned long long s1 = state0;
	unsigned long long s0 = state1;
	state0 = s0;
	s1 ^= s1 << 23;
	s1 ^= s1 >> 17;
	s1 ^= s0;
	s1 ^= s0 >> 26;
	state1 = s1;
	return state0 + state1;
}
