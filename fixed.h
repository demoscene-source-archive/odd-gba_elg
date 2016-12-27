#ifndef _FIXED_H_
#define _FIXED_H_

#include <limits.h> // for INT_MAX / INT_MIN
#include "gba_assert.h"

// inline fixed16 fixed_rcp(fixed16 i);
static inline unsigned CLZ(unsigned Input);
extern int RSQ_Table[];
extern int RCP_Table[];

template <int frac_bits>
class fixed {
public:
	fixed() {}

	fixed(int val) : val(val) {}

	fixed operator-() {
		return fixed(-val);
	}

	fixed operator+(const fixed f) const {
		assert(((long long)val + f.val) <= INT_MAX);
		assert(((long long)val + f.val) >= INT_MIN);
		return fixed(val + f.val);
	}

	fixed operator-(const fixed f) const {
		assert(((long long)val - f.val) <= INT_MAX);
		assert(((long long)val - f.val) >= INT_MIN);
		return fixed(val - f.val);
	}

	fixed operator/(const fixed f) const{
		return *this * fixed_rcp(f);
	}

	fixed operator*(const fixed f) const {
		assert((((long long)val * f.val) >> frac_bits) <= INT_MAX);
		assert((((long long)val * f.val) >> frac_bits) >= INT_MIN);
		return fixed(((long long)val * f.val) >> frac_bits);
	}

	fixed operator*(const int i) const {
		assert(((long long)val * i) <= INT_MAX);
		assert(((long long)val * i) >= INT_MIN);
		return fixed(val * i);
	}

	fixed operator>>(const int i) const {
		return fixed(val >> i);
	}

	fixed operator<<(const int i) const {
		assert(((long long)val << i) <= INT_MAX);
		return fixed(val << i);
	}

	const fixed &operator+=(const fixed f) {
		assert(((long long)val + f.val) <= INT_MAX);
		assert(((long long)val + f.val) >= INT_MIN);
		val += f.val;
		return *this;
	}

	const fixed &operator-=(const fixed f) {
		assert(((long long)val - f.val) <= INT_MAX);
		assert(((long long)val - f.val) >= INT_MIN);
		val -= f.val;
		return *this;
	}

	const fixed &operator/=(const fixed f) {
		*this *= fixed_rcp(f);
		return *this;
	}

	const fixed &operator*=(const fixed f) {
		assert((((long long)val * f.val) >> frac_bits) <= INT_MAX);
		assert((((long long)val * f.val) >> frac_bits) >= INT_MIN);
		val = ((long long)val * f.val) >> frac_bits;
		return *this;
	}

	const fixed &operator*=(const int i) {
		assert(((long long)val * i) <= INT_MAX);
		assert(((long long)val * i) >= INT_MIN);
		val = val * i;
		return *this;
	}

	const fixed &operator>>=(const int i) {
		val = val >> i;
		return *this;
	}

	const fixed &operator<<=(const int i) {
		assert(((long long)val << i) <= INT_MAX);
		val = val << i;
		return *this;
	}

	fixed rcp(fixed i) {
		unsigned V, V1, V2, V3, V4;
		int ZeroBits;
		int f = i.to_int();
		int Neg = f < 0;

		f = abs(f);
		ZeroBits = CLZ(f);
		V = f << ZeroBits;
		V1 = RCP_Table[V >> 23];
		V2 = (V >> 14) & 511;

		//	V3 = V1 + Mult16x16(V1, V2);
		V3 = V1 + V1 * V2;

		V4 = V3 >> (31 - ZeroBits);
		if (Neg) V4 = -V4;
		return fixed(V4);
	}


	int to_int() {
		return val;
	}

	fixed abs() {
		if (val > 0) return *this;
		return -(*this);
	}

	int val;
};

typedef fixed<16> fixed16;

#include "math.h"

#endif /* _FIXED_H_ */
