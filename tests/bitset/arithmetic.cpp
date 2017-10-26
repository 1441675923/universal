﻿//  arithmetic.cpp : bitset-based arithmetic tests
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "stdafx.h"
#include <sstream>
#include "../../bitset/bitset_helpers.hpp"

using namespace std;

void BinaryConversions() {
	const size_t nbits = 33;
		std::bitset<nbits> a, b, sum;
	bool carry = 0;

	cout << "Binary conversions" << endl;

	a = flip_sign_bit(convert_to_bitset<nbits,uint64_t>(uint64_t(0x55555555)));
	b = convert_to_bitset<nbits,uint64_t>(uint64_t(0x5));

	carry = add_signed_magnitude(a, b, sum);

	cout << "a   = " << a << endl;
	cout << "b   = " << to_binary(b) << endl;
	cout << "sum = " << to_binary(sum) << " carry = " << carry << endl;

	cout << "1's complement of a = " << to_binary(ones_complement(a)) << endl;
	cout << "1's complement of b = " << to_binary(ones_complement(b)) << endl;

	std::bitset<9> c;
	c = convert_to_bitset<9,int8_t>(int8_t(-128));
	cout << "c                = " << to_binary(c) << endl;
	c = twos_complement(c);
	cout << "2's Complement   = " << to_binary(c) << endl;

	std::bitset<9> d;
	d = convert_to_bitset<9,int64_t>(int64_t(int8_t(-128)));
	cout << "d                = " << to_binary(d) << endl;
	d = twos_complement(d);
	cout << "2's complement   = " << to_binary(d) << endl;
	cout << endl;
}



template<size_t nbits>
void add_fractions(int f1_scale, std::bitset<nbits> f1, int f2_scale, std::bitset<nbits> f2, int& sum_scale, std::bitset<nbits>& sum) {
	// fraction operations that are part of adding posits
	cout << "add fractions" << endl;
	cout << "f1 scale " << f1_scale << " value " << to_hex(f1) << endl;
	cout << "f2 scale " << f2_scale << " value " << to_hex(f2) << endl;

	// scale the smaller number to the bigger number by right shifting the difference
	// fractions are right extended, and the hidden bit becomes the msb
	f1.set(nbits - 1);
	f2.set(nbits - 1);
	int diff = f1_scale - f2_scale;
	cout << "scale difference is " << diff << endl;

	if (diff < 0) {
		f1 >>= -diff;	// denormalize
	}
	else {
		f2 >>= diff;	// denormalize
	}
	cout << "f1  : " << f1 << endl;
	cout << "f2  : " << f2 << endl;

	bool carry = add_unsigned(f1, f2, sum);
	cout << "sum : " << sum << " carry : " << (carry ? 1 : 0) << endl; 

	sum_scale = (f1_scale > f2_scale ? f1_scale : f2_scale);
	sum_scale = f1_scale + (carry ? 1 : 0);
	sum >>= (carry ? 1 : 0);
}

bool FractionManipulation()
{
	bool bValid = true;
	const size_t posit_nbits = 16;
	const size_t fraction_nbits = posit_nbits - 2;
	int f1_scale = 5;

	cout << "Fraction manipulation" << endl;
	std::bitset<fraction_nbits> f1 = convert_to_bitset<fraction_nbits,uint64_t>(uint64_t(0x1fff));
	int f2_scale = 3;
	std::bitset<fraction_nbits> f2 = convert_to_bitset<fraction_nbits,uint64_t>(uint64_t(0xf));
	int sum_scale = 0;
	std::bitset<fraction_nbits> sum;
	add_fractions<fraction_nbits>(f1_scale, f1, f2_scale, f2, sum_scale, sum);
	cout << "sum : " << sum << " sum_scale : " << sum_scale << endl;
	cout << endl;
	return bValid;
}

bool IncrementRightAdjustedBitset()
{
	bool bValid = true;
	const size_t size = 5;
	std::bitset<size> r1, ref;
	bool carry;

	cout << "Increments" << endl;
	for (int i = 0; i < size; i++) {
		r1.reset();
		r1.set(size - 1 - i, true);
		carry = false;
		cout << "carry " << (carry ? "1" : "0") << " r1 " << r1 << " <-- input" << endl;
		carry = increment_unsigned(r1, i);
		cout << "carry " << (carry ? "1" : "0") << " r1 " << r1 << " <-- result" << endl;
	}
	cout << endl;
	return bValid;
}

int main()
try
{

	cout << "Arithmetic experiments on bitsets" << endl;

	BinaryConversions();

	IncrementRightAdjustedBitset();

	FractionManipulation();


	return 0;
}
catch (char* msg) {
	cerr << msg << endl;
	return 1;
}
