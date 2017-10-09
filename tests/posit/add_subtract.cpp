// add_subtract.cpp: functional tests for addition and subtraction
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "stdafx.h"

#include "../../bitset/bitset_helpers.hpp"
#include "../../posit/posit_regime_lookup.hpp"
#include "../../posit/posit.hpp"
#include "../../posit/posit_operators.hpp"
#include "../../posit/posit_manipulators.hpp"

using namespace std;

// normalize creates a normalized number with the hidden bit installed: 1.bbbbbbbbb
template<size_t nbits>
void normalize(const std::bitset<nbits>& fraction, std::bitset<nbits>& number) {
	if (nbits == 3) return;
	number.set(nbits - 1); // set hidden bit
	int lb = nbits; lb -= 2;
	for (int i = lb; i >= 0; i--) {
		number.set(i, fraction[i + 1]);
	}
}
/*   h is hidden bit
*   h.bbbb_bbbb_bbbb_b...      fraction
*   0.000h_bbbb_bbbb_bbbb_b... number
*  >-.----<                    shift of 4
*/
template<size_t nbits>
void denormalize(const std::bitset<nbits>& fraction, int shift, std::bitset<nbits>& number) {
	if (nbits == 3) return;
	number.reset();
	if (shift <= nbits - 1) {
		number.set(nbits - 1 - shift); // set hidden bit
		for (int i = nbits - 2 - shift; i >= 0; i--) {
			number.set(i, fraction[i + 1 + shift]);
		}
	}
}

/*
	Testing the reciprocal nature of positive and negative posits
 */

/*
POSIT<4,0>
   #           Binary         Decoded         k-value            sign                        regime        exponent        fraction                         value
   0:             0000            0000               0               1                             1            -            ----                             0
   1:             0001            0001              -2               1                          0.25            -            ----                          0.25
   2:             0010            0010              -1               1                           0.5            -            0---                           0.5
   3:             0011            0011              -1               1                           0.5            -            1---                          0.75
   4:             0100            0100               0               1                             1            -            0---                             1
   5:             0101            0101               0               1                             1            -            1---                           1.5
   6:             0110            0110               1               1                             2            -            ----                             2
   7:             0111            0111               2               1                             4            -            ----                             4
   8:             1000            1000              -3              -1                         0.125            -            ----                           inf
   9:             1001            1111               2              -1                             4            -            ----                            -4
  10:             1010            1110               1              -1                             2            -            ----                            -2
  11:             1011            1101               0              -1                             1            -            1---                          -1.5
  12:             1100            1100               0              -1                             1            -            0---                            -1
  13:             1101            1011              -1              -1                           0.5            -            1---                         -0.75
  14:             1110            1010              -1              -1                           0.5            -            0---                          -0.5
  15:             1111            1001              -2              -1                          0.25            -            ----                         -0.25
*/
bool ValidateAdditionPosit_4_0() {
	float input_values[7] = {
		0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 4.0
	};
	float golden_value[7] = {
		0.50, 1.0, 1.50, 2.0, 4.0, 4.0, 4.0
	};

	bool bValid = true;
	posit<4, 0> pa, pb, psum;
	for (int i = 0; i < 7; i++) {
		pa = input_values[i];
		pb = pa;
		psum = pa + pb;
		if (fabs(psum.to_double() - golden_value[i]) > 0.0001) {
			cerr << "Posit<4,0> addition failed: " << pa << " + " << pb << " != " << golden_value[i] << " instead it yielded " << psum << " " << components_to_string(psum) << endl;
			bValid = false;
		}
	}

	return bValid;
}

bool ValidateNegationPosit_4_0() {
	float target_values[16] = {
		-4.0, -2.0, -1.5, -1.0, -0.75, -0.5, -0.25, 0.0, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 4.0, INFINITY
	};

	float golden_value[16] = {
		+4.0, +2.0, +1.5, +1.0, +0.75, +0.5, +0.25, 0.0, -0.25, -0.5, -0.75, -1.0, -1.5, -2.0, -4.0, INFINITY
	};

	bool bValid = true;
	posit<4, 0> pa, pb, pgolden;
	for (int i = 0; i < 16; i++) {
		pa = target_values[i];
		pb = -pa;
		pgolden = golden_value[i];
		if (pb != pgolden) {
			cerr << "Posit<4,0> negation failed: neg(" << pa << ") != " << pb << " golden_value is " << pgolden << " " << components_to_string(pb) << endl;
			bValid = false;
		}
	}

	return bValid;
}

bool ValidateNegAdditionPosit_4_0() {
	float target_values[7] = {
		0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 4.0
	};

	bool bValid = true;
	posit<4, 0> pa, pb, psum;
	for (int i = 0; i < 7; i++) {
		pa =  target_values[i];
		pb = -pa;
		psum = pa + pb;
		if (fabs(psum.to_double()) > 0.0001) {
			cerr << "Posit<4,0> negated add failed: " << pa << " + " << pb << " != 0 instead it yielded " << psum << " " << components_to_string(psum) << endl;
			bValid = false;
		}
	}

	return bValid;
}

void TestPositArithmeticOperators(bool bValid, string posit_cfg, string op)
{
	if (!bValid) {
		cout << posit_cfg << " " << op << " FAIL" << endl;
	}
	else {
		cout << posit_cfg << " " << op << " PASS" << endl;
	}
}

int main(int argc, char** argv)
{
	posit<4, 0> p, neg;
	p = 0.0f;
	cout << p << " " << components_to_string(p) << endl;
	neg = -p;
	cout << neg << " " << components_to_string(neg) << endl;
	bool test;
	test = p != neg;
	test = p == neg;
	//return 0;

	TestPositArithmeticOperators(ValidateAdditionPosit_4_0(), "posit<4,0>", "addition");
	TestPositArithmeticOperators(ValidateNegationPosit_4_0(), "posit<4,0>", "negation");
	TestPositArithmeticOperators(ValidateNegAdditionPosit_4_0(), "posit<4,0>", "neg addition");

	return 0;

	{

		cout << to_binary(6) << endl;
		cout << to_binary(16) << endl;
		cout << to_binary(24) << endl;
		const size_t nbits = 5;
		const size_t es = 1;
		posit<nbits, es> pa, pb, psum;
		int32_t va, vb;
		va = 16; vb = 6;
		pa = va; pb = vb;

		int lhs_scale = pa.scale();
		int rhs_scale = pb.scale();
		cout << "LHS " << va << " scale " << lhs_scale << endl;
		cout << "RHS " << vb << " scale " << rhs_scale << endl;
		int diff = lhs_scale - rhs_scale;
		cout << "diff " << diff << endl;
		if (diff < 0) {
			cerr << "Wrong" << endl;
		}
		else {
			bitset<nbits> r1, r2;

			cout << "frac1 " << pa.get_fraction() << endl;
			normalize<nbits>(pa.get_fraction().get(), r1);
			cout << "r1    " << r1 << endl;

			cout << "frac2 " << pb.get_fraction() << endl;
			denormalize<nbits>(pb.get_fraction().get(), diff, r2);
			cout << "r2    " << r2 << endl;
		}
		cout.flush();
		//psum = pa + pb;
		cout << pa << " + " << pb << " = " << psum << endl;
		cout << psum.get() << endl;
		psum = 21;
		cout << psum.get() << endl;
	}


	//diff = a - b;
	//cout << "Diff : " << diff << endl;

	//cout << "a " << a++ << " a+1 " << a << endl;

	return 0;
}
