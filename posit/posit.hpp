#pragma once
// posit.hpp: definition of arbitrary posit number configurations
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include <cmath>
#include <iostream>

#include "../bitset/bitset_helpers.hpp"
#include "posit_regime_lookup.hpp"
#include "posit_helpers.hpp"

const uint8_t POSIT_ROUND_DOWN = 0;
const uint8_t POSIT_ROUND_TO_NEAREST = 1;

template<size_t nbits, size_t es>
double useed() {
	return double(uint64_t(1) << (uint64_t(1) << es));
};

template<size_t nbits, size_t es>
class regime {
public:
	regime() {
		_Bits.reset();
		_k = 0;
		_RegimeBits = 0;
	}
	regime(const regime& r) {
		_Bits = r._Bits;
		_k = r._k;
		_RegimeBits = r._RegimeBits;
	}
	regime& operator= (const regime& r) {
		_Bits = r._Bits;
		_k = r._k;
		_RegimeBits = r._RegimeBits;
		return *this;
	}
	void reset() {
		_k = 0;
		_RegimeBits = 0;
		_Bits.reset();
	}
	unsigned int nrBits() const {
		return _RegimeBits;
	}
	int scale() const {
		return (int(_k) << es);
	}
	// return the k-value of the regime: useed ^ k
	int regime_k() const {
		return _k;
	}
	double value() const {
		double scale;
		int e2 = (1 << es) * _k;
		if (e2 < -63 || e2 > 63) {
			scale = pow(2.0, e2);
		}
		else {
			if (e2 >= 0) {
				scale = double((uint64_t(1) << e2));
			}
			else {
				scale = double(1.0) / double(uint64_t(1) << -e2);
			}
		}
		return scale;
	}
	bool isZero() const {
		return _Bits.none();
	}
	std::bitset<nbits - 1> get() const {
		return _Bits;
	}
	void set(const std::bitset<nbits - 1>& raw, unsigned int nrOfRegimeBits) {
		_Bits = raw;
		_RegimeBits = nrOfRegimeBits;
	}
	void setZero() {
		_Bits.reset();
		_RegimeBits = nbits - 1;
	}
	void setInf() {
		_Bits.reset();
		_RegimeBits = nbits - 1;
	}
	// construct the regime bit pattern given a number's scale and returning the number of regime bits
	unsigned int assign_regime_pattern(bool sign, int k) {
		_Bits.reset();
		_k = k;
		if (k < 0) {
			k = -k - 1;
			uint64_t regime = REGIME_BITS[k];
			uint64_t mask = REGIME_BITS[0];
			_RegimeBits = (k < nbits - 2 ? k + 2 : nbits - 1);
			for (unsigned int i = 0; i < _RegimeBits; i++) {
				_Bits[nbits - 2 - i] = !(regime & mask);
				mask >>= 1;
			}
		}
		else {
			uint64_t regime = REGIME_BITS[k];
			uint64_t mask = REGIME_BITS[0];
			_RegimeBits = (k < nbits - 2 ? k + 2 : nbits - 1);
			for (unsigned int i = 0; i < _RegimeBits; i++) {
				_Bits[nbits - 2 - i] = regime & mask;
				mask >>= 1;
			}
		}
		return _RegimeBits;
	}
private:
	std::bitset<nbits - 1>	_Bits;
	int8_t					_k;
	unsigned int			_RegimeBits;

	// template parameters need names different from class template parameters (for gcc and clang)
	template<size_t nnbits, size_t ees>
	friend std::ostream& operator<< (std::ostream& ostr, const regime<nnbits, ees>& r);
	template<size_t nnbits, size_t ees>
	friend std::istream& operator>> (std::istream& istr, regime<nnbits, ees>& r);

	template<size_t nnbits, size_t ees>
	friend bool operator==(const regime<nnbits, ees>& lhs, const regime<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator!=(const regime<nnbits, ees>& lhs, const regime<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator< (const regime<nnbits, ees>& lhs, const regime<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator> (const regime<nnbits, ees>& lhs, const regime<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator<=(const regime<nnbits, ees>& lhs, const regime<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator>=(const regime<nnbits, ees>& lhs, const regime<nnbits, ees>& rhs);
};

template<size_t nbits, size_t es>
class exponent {
public:
	exponent() {
		reset();
	}
	exponent(const exponent& e) {
		_Bits = e._Bits;
		_NrOfBits = e._NrOfBits;
	}
	exponent& operator=(const exponent& e) {
		_Bits = e._Bits;
		_NrOfBits = e._NrOfBits;
		return *this;
	}
	void reset() {
		_NrOfBits = 0;
		_Bits.reset();
	}
	unsigned int nrBits() const {
		return _NrOfBits;
	}
	int scale() const {
		return _Bits.to_ulong();
	}
	double value() const {
		return double(uint64_t(1) << scale());
	}
	std::bitset<es> get() const {
		return _Bits;
	}
	void set(const std::bitset<es>& raw, int nrOfExponentBits) {
		_Bits = raw;
		_NrOfBits = nrOfExponentBits;
	}
	// calculate the exponent given a number's scale and the number of regime bits, returning the number of exponent bits assigned
	unsigned int assign_exponent_bits(unsigned int msb, unsigned int nr_of_regime_bits) {
		_Bits.reset();
		_NrOfBits = (nbits - 1 - nr_of_regime_bits > es ? es : nbits - 1 - nr_of_regime_bits);
		if (_NrOfBits > 0) {
			unsigned int exponent = (es > 0 ? msb % (1 << es) : 0);
			uint64_t mask = (uint64_t(1) << (_NrOfBits - 1));
			for (unsigned int i = 0; i < _NrOfBits; i++) {
				_Bits[es - 1 - i] = exponent & mask;
				mask >>= 1;
			}
		}
		return _NrOfBits;
	}
private:
	std::bitset<es> _Bits;
	unsigned int	_NrOfBits;

	// template parameters need names different from class template parameters (for gcc and clang)
	template<size_t nnbits, size_t ees>
	friend std::ostream& operator<< (std::ostream& ostr, const exponent<nnbits, ees>& e);
	template<size_t nnbits, size_t ees>
	friend std::istream& operator>> (std::istream& istr, exponent<nnbits, ees>& e);

	template<size_t nnbits, size_t ees>
	friend bool operator==(const exponent<nnbits, ees>& lhs, const exponent<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator!=(const exponent<nnbits, ees>& lhs, const exponent<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator< (const exponent<nnbits, ees>& lhs, const exponent<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator> (const exponent<nnbits, ees>& lhs, const exponent<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator<=(const exponent<nnbits, ees>& lhs, const exponent<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator>=(const exponent<nnbits, ees>& lhs, const exponent<nnbits, ees>& rhs);
};

template<size_t nbits, size_t es>
class fraction {
public:
	fraction() {
		_Bits.reset();
	}
	fraction(const fraction& f) {
		_Bits = f._Bits;
		_FractionBits = f._FractionBits;
	}
	fraction& operator=(const fraction& f) {
		_Bits = f._Bits;
		_FractionBits = f._FractionBits;
		return *this;
	}
	void reset() {
		_FractionBits = 0;
		_Bits.reset();
	}
	unsigned int nrBits() const {
		return _FractionBits;
	}
	double value() const {
		return double(_Bits.to_ullong()) / double(uint64_t(1) << nbits);
	}
	std::bitset<nbits> get() const {
		return _Bits;
	}
	void set(const std::bitset<nbits>& raw, int nrOfFractionBits) {
		_Bits = raw;
		_FractionBits = nrOfFractionBits;
	}
	// given a number and the fraction's cut-off point, assign the fraction bits in a right-extended format, returning the number of fraction bits assigned
	unsigned int assign_fraction_bits(uint64_t number, unsigned int nrOfFractionBits) {
		std::bitset<nbits> _frac;
		unsigned int _fractionBits = nrOfFractionBits;
		_Bits.set(_frac, _fractionBits);
	}
	void assign_fraction(unsigned int remaining_bits, std::bitset<nbits>& _fraction) {
		if (remaining_bits > 0 && nbits > 3) {
			for (unsigned int i = 0; i < remaining_bits; i++) {
				_Bits[nbits - 1 - i] = _fraction[nbits - 1 - i];
			}
		}
	}
private:
	// maximum size fraction is <nbits - one sign bit - minimum two regime bits>
	// the conditional length of the exponent field creates a situation where we need to use the maximum size constant.
	// this is too big and not precise, but is an outcome of using a template specification that needs to be const
	// at time of compilation.
	std::bitset<nbits> _Bits;
	unsigned int _FractionBits;

	// template parameters need names different from class template parameters (for gcc and clang)
	template<size_t nnbits, size_t ees>
	friend std::ostream& operator<< (std::ostream& ostr, const fraction<nnbits, ees>& f);
	template<size_t nnbits, size_t ees>
	friend std::istream& operator>> (std::istream& istr, fraction<nnbits, ees>& f);

	template<size_t nnbits, size_t ees>
	friend bool operator==(const fraction<nnbits, ees>& lhs, const fraction<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator!=(const fraction<nnbits, ees>& lhs, const fraction<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator< (const fraction<nnbits, ees>& lhs, const fraction<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator> (const fraction<nnbits, ees>& lhs, const fraction<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator<=(const fraction<nnbits, ees>& lhs, const fraction<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator>=(const fraction<nnbits, ees>& lhs, const fraction<nnbits, ees>& rhs);
};

/*
 class posit represents arbitrary configuration posits and their basic arithmetic operations (add/sub, mul/div)
 */
template<size_t nbits, size_t es> class posit {
public:
	posit<nbits, es>() {
		bRoundingMode = POSIT_ROUND_DOWN;
		reset();
		validate();
	}
	posit<nbits, es>(int64_t initial_value) {
		*this = initial_value;
	}
	posit<nbits, es>(const posit& p) {
		*this = p;
	}
	posit<nbits, es>& operator=(int8_t rhs) {
		*this = int64_t(rhs);
		return *this;
	}
	posit<nbits, es>& operator=(int16_t rhs) {
		*this = int64_t(rhs);
		return *this;
	}
	posit<nbits, es>& operator=(int32_t rhs) {
		*this = int64_t(rhs);
		return *this;
	}
	posit<nbits, es>& operator=(int64_t rhs) {
		reset();
		bool _sign = (0x8000000000000000 & rhs);  // 1 is negative, 0 is positive
		if (_sign) {
			// process negative number: process 2's complement of the input
			unsigned int _scale = findMostSignificantBit(-rhs) - 1;
			uint64_t _fraction_without_hidden_bit = (-rhs << (64 - _scale));
			std::bitset<nbits> _fraction = copy_integer_fraction<nbits>(_fraction_without_hidden_bit);
			convert_to_posit(_sign, _scale, _fraction);
			take_2s_complement();
		}
		else {
			// process positive number
			if (rhs != 0) {
				unsigned int _scale = findMostSignificantBit(rhs) - 1;
				uint64_t _fraction_without_hidden_bit = (rhs << (64 - _scale));
				std::bitset<nbits> _fraction = copy_integer_fraction<nbits>(_fraction_without_hidden_bit);
				convert_to_posit(_sign, _scale, _fraction);
			}
		}
		return *this;
	}
	posit<nbits, es>& operator=(uint64_t rhs) {
		reset();
		if (rhs != 0) {
			unsigned int _scale = findMostSignificantBit(rhs) - 1;
			uint64_t _fraction_without_hidden_bit = (rhs << (64 - _scale));
			std::bitset<nbits> _fraction = copy_integer_fraction<nbits>(_fraction_without_hidden_bit);
			convert_to_posit(false, _scale, _fraction);
		}
		decode();
		return *this;
	}
	posit<nbits, es>& operator=(const float rhs) {
		reset();
		switch (std::fpclassify(rhs)) {
		case FP_INFINITE:
			_regime.setInf();
			break;
		case FP_NAN:
			std::cerr << "float is NAN" << std::endl;
			break;
		case FP_SUBNORMAL:
			std::cerr << "TODO: subnormal number" << std::endl;
			break;
		case FP_NORMAL:
			{
				if (rhs == 0.0) break;  // 0 is a special case
				bool _sign = extract_sign(rhs); 
				int _scale = extract_exponent(rhs) - 1;
				uint32_t _23b_fraction_without_hidden_bit = extract_fraction(rhs);
				std::bitset<nbits> _fraction = copy_float_fraction<nbits>(_23b_fraction_without_hidden_bit);
				//std::cout << "sign " << _sign << " scale " << _scale << " 23b fraction " << std::hex << _23b_fraction_without_hidden_bit << " _fraction " << _fraction << std::dec << std::endl;
				convert_to_posit(_sign, _scale, _fraction);
				if (_sign) {
					// transform back through 2's complement
					take_2s_complement();
				}
			}
			break;
		}
		return *this;
	}
	posit<nbits, es>& operator=(const double rhs) {
		reset();
		switch (std::fpclassify(rhs)) {
		case FP_INFINITE:
			_regime.setInf();
			break;
		case FP_NAN:
			std::cerr << "float is NAN" << std::endl;
			break;
		case FP_SUBNORMAL:
			std::cerr << "TODO: subnormal number" << std::endl;
			break;
		case FP_NORMAL:
			{
				if (rhs == 0.0) break;  // 0 is a special case
				bool _sign = extract_sign(rhs);
				int _scale = extract_exponent(rhs) - 1;
				uint64_t _52b_fraction_without_hidden_bit = extract_fraction(rhs);
				std::bitset<nbits> _fraction = copy_double_fraction<nbits>(_52b_fraction_without_hidden_bit);
				//std::cout << "sign " << _sign << " scale " << _scale << " 52b fraction " << std::hex << _52b_fraction_without_hidden_bit << " _fraction " << _fraction << std::dec << std::endl;
				convert_to_posit(_sign, _scale, _fraction);
				if (_sign) {
					// transform back through 2's complement
					take_2s_complement();
				}
			}
			break;
		}
		return *this;
	}
	posit<nbits, es>& operator=(const posit& rhs) {
		_raw_bits = rhs._raw_bits;
		_sign     = rhs._sign;
		_regime   = rhs._regime;
		_exponent = rhs._exponent;
		_fraction = rhs._fraction;
		return *this;
	}
	posit<nbits, es> operator-() {
		posit<nbits, es> negated(*this);
		negated.decode(twos_complement(_raw_bits));
		return negated;
	}
	posit<nbits, es>& operator+=(const posit& rhs) {
		if (isZero()) {
			return *this;
		}
		else {
			if (rhs.isZero()) {
				return *this;
			}
			else if (isInfinite()) {
				return *this;
			}
			else if (rhs.isInfinite()) {
				*this = rhs;
				return *this;
			}
		}
		bool _sign = false;
		std::bitset<nbits> r1, r2, sum; // fraction is at most nbits-3 bits, + 1 for the hidden bit
		int _scale;
		align_numbers(scale(), _fraction.get(), rhs.scale(), rhs._fraction.get(), _scale, r1, r2);

		/*
		std::cout << "lhs " << *this << " scale " << scale() << std::endl;
		std::cout << "rhs " << rhs <<   " scale " << rhs.scale() << std::endl;
		std::cout << "lhs_f " << _fraction << std::endl;
		std::cout << "rhs_f " << rhs._fraction << std::endl;
		std::cout << "r1    " << r1 << std::endl;
		std::cout << "r2    " << r2 << std::endl;
		std::cout << "scale " << _scale << std::endl;
		*/

		bool carry = add_unsigned<nbits>(r1, r2, sum);
		//std::cout << "sum " << sum << " carry " << (carry ? "1" : "0") << std::endl;
		if (carry) {
			_scale++;
			sum >>= 1;  // hide the msb
		}
		else {
			// find the msb that will become the hidden bit
			unsigned int msb = 0;
			for (int i = nbits - 1; i >= 0; i--) {
				if (sum.test(i)) {
					msb = i;
					break;
				}
			}
			_scale += msb - (nbits - 1);
			sum <<= 1; // the msb becomes the hidden bit
		}
		/*
		std::cout << "scale " << _scale << std::endl;
		std::cout << "sum " << sum << std::endl;
		*/
		convert_to_posit(_sign, _scale, sum);
		return *this;
	}
	posit<nbits, es>& operator-=(const posit& rhs) {
		return *this;
	}
	posit<nbits, es>& operator*=(const posit& rhs) {
		return *this;
	}
	posit<nbits, es>& operator/=(const posit& rhs) {
		return *this;
	}
	posit<nbits, es>& operator++() {
		*this = *this + posit<nbits, es>(1);
		return *this;
	}
	posit<nbits, es> operator++(int) {
		posit tmp(*this);
		operator++();
		return tmp;
	}
	posit<nbits, es>& operator--() {
		return *this;
	}
	posit<nbits, es> operator--(int) {
		posit tmp(*this);
		operator--();
		return tmp;
	}

	// SELECTORS
	bool isInfinite() const {
		return (_sign & _regime.isZero());
	}
	bool isZero() const {
		return (!_sign & _regime.isZero());
	}
	bool isNegative() const {
		return _sign;
	}
	bool isPositive() const {
		return !_sign;
	}
	std::string RoundingMode() {
		switch (bRoundingMode) {
		case POSIT_ROUND_DOWN:
			return std::string("ROUND_DOWN");
			break;
		case POSIT_ROUND_TO_NEAREST:
			return std::string("ROUND_TO_NEAREST");
			break;
		default:
			return std::string("UNKNOWN");
		}
	}
	double useed_value() const {
		return double(uint64_t(1) << useed_scale());
	}
	double maxpos_value() {
		return pow(double(useed_value()), double(nbits-2));
	}
	double minpos_value() {
		return pow(double(useed_value()), double(static_cast<int>(2-nbits)));
	}
	uint32_t useed_scale() const {
		return (uint32_t(1) << es);
	}
	uint32_t maxpos_scale() {
		return (nbits - 2) * (1 << es);
	}
	uint32_t minpos_scale() {
		return static_cast<int>(2 - nbits) * (1 << es);
	}

	int	   sign_value() const {
		return (_sign ? -1 : 1);
	}
	double regime_value() const {
		return _regime.value();
	}
	double exponent_value() const {
		return _exponent.value();
	}
	double fraction_value() const {
		return _fraction.value();
	}

	int				   regime_k() const {
		return _regime.regime_k();
	}
	regime<nbits, es>  get_regime() const {
		return _regime;
	}
	exponent<nbits,es> get_exponent() const {
		return _exponent;
	}
	fraction<nbits,es> get_fraction() const {
		return _fraction;
	}
	std::bitset<nbits> get() const {
		return _raw_bits;
	}
	std::bitset<nbits> get_decoded() const {
		std::bitset<nbits-1> r = _regime.get();
		unsigned int nrRegimeBits = _regime.nrBits();
		std::bitset<es> e = _exponent.get();
		unsigned int nrExponentBits = _exponent.nrBits();
		std::bitset<nbits> f = _fraction.get();
		unsigned int nrFractionBits = _fraction.nrBits();

		std::bitset<nbits> _Bits;
		_Bits.set(nbits - 1, _sign);
		int msb = nbits - 2;
		for (unsigned int i = 0; i < nrRegimeBits; i++) {
			_Bits.set(msb--, r[nbits - 2 - i]);
		}
		if (msb < 0) return _Bits;
		for (unsigned int i = 0; i < nrExponentBits; i++) {
			_Bits.set(msb--, e[es - 1 - i]);
		}
		if (msb < 0) return _Bits;
		for (unsigned int i = 0; i < nrFractionBits; i++) {
			_Bits.set(msb--, f[nbits - 1 - i]);
		}
		return _Bits;
	}
	void validate() {
		if (nbits < es + 3) {
			throw "Requested es is too large for nbits";
		}
	}

	// MODIFIERS
	void reset() {
		bRoundingMode = POSIT_ROUND_TO_NEAREST;
		_sign = false;
		_regime.reset();
		_exponent.reset();
		_fraction.reset();
	}
	posit<nbits, es>&  set(const std::bitset<nbits>& raw_bits) {
		decode(raw_bits);
		return *this;
	}
	// Set the raw bits of the posit given a binary pattern
	posit<nbits,es>& set_raw_bits(uint64_t value) {
		reset();
		std::bitset<nbits> raw_bits;
		unsigned long mask = 1;
		for ( int i = 0; i < nbits; i++ ) {
			raw_bits.set(i,(value & mask));
			mask = mask << 1;
		}
		// decode to cache the posit number interpretation
		decode(raw_bits);
		return *this;
	}
	int decode_regime(std::bitset<nbits>& raw_bits) {
		// let m be the number of identical bits in the regime
		int m = 0;   // regime runlength counter
		int k = 0;   // converted regime scale
		if (raw_bits[nbits - 2] == 1) {   // run length of 1's
			m = 1;   // if a run of 1's k = m - 1
			for (int i = nbits - 3; i >= 0; --i) {
				if (raw_bits[i] == 1) {
					m++;
				}
				else {
					break;
				}
			}
			k = m - 1;
		}
		else {
			m = 1;  // if a run of 0's k = -m
			for (int i = nbits - 3; i >= 0; --i) {
				if (raw_bits[i] == 0) {
					m++;
				}
				else {
					break;
				}
			}
			k = -m;
		}
		return k;
	}
	// decode takes the raw bits representing a posit coming from memory
	// and decodes the regime, the exponent, and the fraction.
	// This function has the functionality of the posit register-file load.
	void decode(const std::bitset<nbits>& raw_bits) {
		bool bVerbose = false;
		_raw_bits = raw_bits;	// store the raw bits for reference
		// check special cases
		std::bitset<nbits> tmp(raw_bits);
		// special case = 0
		if (tmp.none()) {  // special case = 0
			// that is reset state
			_regime.setZero();
			_exponent.reset();
			_fraction.reset();
			return;
		}
		_sign = raw_bits.test(nbits - 1);

		// special case = +-inf
		if (_sign) {
			tmp.reset(nbits - 1);
			if (raw_bits.none()) {
				_regime.setInf();
				_exponent.reset();
				_fraction.reset();
				return;
			}
			tmp.set(nbits - 1);  // set the sign bit again
		}

		if (_sign) tmp = twos_complement(tmp);
		unsigned int nrRegimeBits = _regime.assign_regime_pattern(_sign, decode_regime(tmp));
		if (bVerbose) std::cout << " _regimeBits = " << nrRegimeBits << " ";

		// get the exponent bits
		// start of exponent is nbits - (sign_bit + regime_bits)
		int32_t msb = nbits - 1 - (1 + nrRegimeBits);
		if (bVerbose) std::cout << " msb = " << msb << " ";
		unsigned int nrExponentBits = 0;
		if (es > 0) {
			std::bitset<es> _exp;
			if (msb >= 0 && es > 0) {
				nrExponentBits = (msb >= es - 1 ? es : msb + 1);
				if (bVerbose) std::cout << " _exponentBits " << nrExponentBits << " ";
				for (unsigned int i = 0; i < nrExponentBits; i++) {
					_exp[es - 1 - i] = tmp[msb - i];
				}
			}
			_exponent.set(_exp, nrExponentBits);
		}
		else {
			if (bVerbose) std::cout << " _exponentBits 0 msb " << msb << " ";
		}

		// finally, set the fraction bits
		// we do this so that the fraction is right extended with 0;
		// The max fraction is <nbits - 3 - es>, but we are setting it to <nbits - 3> and right-extent
		// The msb bit of the fraction represents 2^-1, the next 2^-2, etc.
		// If the fraction is empty, we have a fraction of nbits-3 0 bits
		// If the fraction is one bit, we have still have fraction of nbits-3, with the msb representing 2^-1, and the rest are right extended 0's
		std::bitset<nbits> _frac;
		msb = msb - nrExponentBits;
		unsigned int nrFractionBits = (msb < 0 ? 0 : msb + 1);
		if (bVerbose) std::cout << "fraction bits " << (msb < 0 ? 0 : msb + 1) << std::endl;
		if (msb >= 0) {
			for (int i = msb; i >= 0; --i) {
				_frac[nbits - 1 - (msb - i)] = tmp[i];
			}
		}
		_fraction.set(_frac, nrFractionBits);

		// we are storing both the raw bit representation and the decoded form
		// so no need to transform back via 2's complement of regime/exponent/fraction
	}
	int64_t to_int64() const {
		if (isZero()) return 0;
		if (isInfinite()) throw "inf";
		// returning the integer representation of a posit only works for [1,inf) and is approximate
		int64_t value;
		int s = scale();
		if (s < 0) {
			value = (_fraction.get().to_ullong() >> -s);
		}
		else {
			value = (_fraction.get().to_ullong() << s);
		}	
		return value;
	}
	double to_double() const {
		if (isZero())
			return 0.0;
		if (isInfinite())
			return INFINITY;
		return sign_value() * regime_value() * exponent_value() * (1.0 + fraction_value());
	}
	// collect the posit components into a bitset
	std::bitset<nbits> collect() {
		std::bitset<nbits - 1> r = _regime.get();
		unsigned int nrRegimeBits = _regime.nrBits();
		std::bitset<es> e = _exponent.get();
		unsigned int nrExponentBits = _exponent.nrBits();
		std::bitset<nbits> f = _fraction.get();
		unsigned int nrFractionBits = _fraction.nrBits();
		std::bitset<nbits> raw_bits;
		// collect
		raw_bits.set(nbits - 1, _sign);
		int msb = nbits - 2;
		for (unsigned int i = 0; i < nrRegimeBits; i++) {
			raw_bits.set(msb--, r[nbits - 2 - i]);
		}
		if (msb >= 0) {
			for (unsigned int i = 0; i < nrExponentBits; i++) {
				raw_bits.set(msb--, e[es - 1 - i]);
			}
		}
		if (msb >= 0) {
			for (unsigned int i = 0; i < nrFractionBits; i++) {
				raw_bits.set(msb--, f[nbits - 1 - i]);
			}
		}
		return raw_bits;
	}
	// given a decoded posit, take its 2's complement
	void take_2s_complement() {
		// transform back through 2's complement
		std::bitset<nbits - 1> r = _regime.get();
		unsigned int nrRegimeBits = _regime.nrBits();
		std::bitset<es> e = _exponent.get();
		unsigned int nrExponentBits = _exponent.nrBits();
		std::bitset<nbits> f = _fraction.get();
		unsigned int nrFractionBits = _fraction.nrBits();
		std::bitset<nbits> raw_bits;
		// collect
		raw_bits.set(nbits - 1, _sign);
		int msb = nbits - 2;
		for (unsigned int i = 0; i < nrRegimeBits; i++) {
			raw_bits.set(msb--, r[nbits - 2 - i]);
		}
		if (msb >= 0) {
			for (unsigned int i = 0; i < nrExponentBits; i++) {
				raw_bits.set(msb--, e[es - 1 - i]);
			}
		}
		if (msb >= 0) {
			for (unsigned int i = 0; i < nrFractionBits; i++) {
				raw_bits.set(msb--, f[nbits - 1 - i]);
			}
		}
		// transform
		raw_bits = twos_complement(raw_bits);
		// distribute
		std::bitset<nbits - 1> regime_bits;
		for (unsigned int i = 0; i < nrRegimeBits; i++) {
			regime_bits.set(nbits - 2 - i, raw_bits[nbits - 2 - i]);
		}
		_regime.set(regime_bits, nrRegimeBits);
		if (es > 0 && nrExponentBits > 0) {
			std::bitset<es> exponent_bits;
			for (unsigned int i = 0; i < nrExponentBits; i++) {
				exponent_bits.set(es - 1 - i, raw_bits[nbits - 2 - nrRegimeBits - i]);
			}
			_exponent.set(exponent_bits, nrExponentBits);
		}
		if (nrFractionBits > 0) {
			std::bitset<nbits> fraction_bits;
			for (unsigned int i = 0; i < nrFractionBits; i++) {
				fraction_bits.set(nbits - 1 - i, raw_bits[nbits - 2 - nrRegimeBits - nrExponentBits - i]);
			}
			_fraction.set(fraction_bits, nrFractionBits);
		}
	}
	// scale returns the shifts to normalize the number =  regime + exponent shifts
	int scale() const {
		// how many shifts represent the regime?
		// regime = useed ^ k = 2 ^ (k*(2 ^ e))
		// scale = useed ^ k * 2^e 
		return _regime.scale() + _exponent.scale();
	}
	unsigned int estimate_nr_fraction_bits(int k) {
		unsigned int nr_of_regime_bits;
		if (k < 0) {
			k = -k - 1;
			nr_of_regime_bits = (k < nbits - 2 ? k + 2 : nbits - 1);
		}
		else {
			nr_of_regime_bits = (k < nbits - 2 ? k + 2 : nbits - 1);
		}
		unsigned int nr_of_exp_bits = (nbits - 1 - nr_of_regime_bits > es ? es : nbits - 1 - nr_of_regime_bits);
		return (nbits - 1 - nr_of_regime_bits - nr_of_exp_bits > 0 ? nbits - 1 - nr_of_regime_bits - nr_of_exp_bits : 0);
	}

	// -1 -> round-down, 0 -> no rounding, +1 -> round-up
	// _fraction contains the fraction without the hidden bit
	int rounding_decision(const std::bitset<nbits>& _fraction, unsigned int nr_of_fraction_bits) {
		bool bVerbose = false;
		if (bVerbose) std::cout << "_fraction bits to process: " << nr_of_fraction_bits << " " << _fraction << std::endl;
		// check if there are any bits set past the cut-off
		int rounding_direction = 0;
		if (nr_of_fraction_bits == 0) {
			if (_fraction.test(nbits - 1)) {
				rounding_direction = 1;
				if (bVerbose) std::cout << "Rounding up" << std::endl;
			}
			else {
				if (bVerbose) std::cout << "Rounding down" << std::endl;
			}		
			return rounding_direction;
		}
		// first bit after the cut-off is at nbits - 1 - nr_of_fraction_bits
		if (nbits >= 4 + nr_of_fraction_bits) {
			if (bVerbose) std::cout << "_fraction bits to process: " << nr_of_fraction_bits << " " << _fraction << std::endl;
			rounding_direction = -1;
			for (int i = nbits - 1 - nr_of_fraction_bits; i >= 0; --i) {
				if (_fraction.test(i)) {
					rounding_direction = 1;
					if (bVerbose) std::cout << "Fraction bit set: round up" << std::endl;
					break;
				}
			}
			if (rounding_direction == -1) {
				if (bVerbose) std::cout << "Fraction bits not set: round down" << std::endl;
			}
		}
		else {
			if (bVerbose) std::cout << "No bits left: no rounding" << std::endl;
		}
		return rounding_direction;
	}	
	int round(bool _sign, int _scale, std::bitset<nbits>& _fraction) {
		bool bVerbose = false;
		switch (bRoundingMode) {
		case POSIT_ROUND_DOWN:
			if (bVerbose) std::cout << "Rounding Mode: round down" << std::endl;
			break;
		default:
		case POSIT_ROUND_TO_NEAREST:
			if (nbits > 3) {
				switch (rounding_decision(_fraction, estimate_nr_fraction_bits(_scale >> es))) {
				case -1:
					if (bVerbose) std::cout << "Rounding Mode: Rounding down to nearest" << std::endl;
					break;
				case 0:
					if (bVerbose) std::cout << "Rounding Mode: No Rounding required" << std::endl;
					break;
				case 1:
					if (bVerbose) std::cout << "Rounding Mode: Rounding up to nearest" << std::endl;
					_scale += 1;
					break;
				}
			}
			break;
		}
		return _scale;
	}
	// this routine will not allocate 0 or infinity due to the test on (0,minpos], and [maxpos,inf)
	// TODO: is that the right functionality? right now the special cases are deal with in the
	// assignment operators for integer/float/double. I don't like that distribution of knowledge.
	void convert_to_posit(bool _sign, int _scale, std::bitset<nbits>& _frac) {
		reset();
		bool bVerbose = false;
		// deal with minpos/maxpos special cases
		int k = (_scale >> es); 
		if (bVerbose) std::cout << "scale = " << _scale << " es = " << es << " k = " << k << std::endl;
		if (k < 0) {
			// minpos is at k = -(nbits-1)
			if (k <= 1 - nbits) { // <= minpos  0 is dealt with in special case
				if (bVerbose) std::cout << "value between 0 and minpos: round up" << std::endl;
				_regime.assign_regime_pattern(_sign, 2-int(nbits));  // assign minpos
				return;
			}
			else {
				if (bVerbose) std::cout << "value > minpos: round depending on _fraction" << std::endl;
				_scale = round(_sign, _scale, _frac);
			}
		}
		else {
			// maxpos is at k = nbits-1
			if (k >= nbits-1) { // maxpos   INFINITY is dealt with in special case
				if (bVerbose) std::cout << "value between maxpos and INFINITY: round down" << std::endl;
				_regime.assign_regime_pattern(_sign, nbits-2);	// assign maxpos
				return;
			}
			else {
				if (bVerbose) std::cout << "value < maxpos: round depending on _fraction" << std::endl;
				_scale = round(_sign, _scale, _frac);
			}
		}
		// construct the posit
		unsigned int nr_of_regime_bits = _regime.assign_regime_pattern(_sign, _scale >> es);
		if (bVerbose) std::cout << "Regime      " << _regime << std::endl;
		unsigned int nr_of_exp_bits = _exponent.assign_exponent_bits(_scale, nr_of_regime_bits);
		if (bVerbose) std::cout << "Exponent    " << _exponent << std::endl;
		if (bVerbose) std::cout << "Fraction in " << _frac << std::endl;
		unsigned int remaining_bits = (nbits - 1 - nr_of_regime_bits - nr_of_exp_bits > 0 ? nbits - 1 - nr_of_regime_bits - nr_of_exp_bits : 0);
		if (bVerbose) std::cout << "Regime bits " << nr_of_regime_bits << "  exponent bits " << nr_of_exp_bits << " remaining bits " << remaining_bits << " fraction " << _frac << std::endl;
		_fraction.assign_fraction(remaining_bits, _frac);
		// store raw bit representation
		_raw_bits = collect();
		if (bVerbose) std::cout << "Posit    " << *this << std::endl;
	}

private:
	bool				   _sign;
	std::bitset<nbits>     _raw_bits;	// raw bit representation
	regime<nbits, es>	   _regime;		// decoded posit representation
	exponent<nbits, es>    _exponent;	// decoded posit representation
	fraction<nbits, es>	   _fraction;	// decoded posit representation

	int8_t bRoundingMode;

	// HELPER methods
	void align_numbers(int lhs_scale, const std::bitset<nbits>& lhs, int rhs_scale, const std::bitset<nbits>& rhs, int& scale, std::bitset<nbits>& r1, std::bitset<nbits>& r2) {
		int diff = lhs_scale - rhs_scale;
		if (diff < 0) {
			scale = rhs_scale;
			denormalize(lhs, diff, r1);
			normalize(rhs, r2);
		}
		else {
			scale = lhs_scale;
			normalize(lhs, r1);
			denormalize(rhs, diff, r2);
		}
	}
	// normalize the fraction by adding the hidden bit into the value
	void normalize(const std::bitset<nbits>& fraction, std::bitset<nbits>& number) {
		if (nbits == 3) return;
		number.set(nbits - 1); // set hidden bit
		for (int i = nbits - 2; i >= 0; i--) {
			number.set(i, fraction[i+1]);
		}
	}
	/*   h is hidden bit
	 *   h.bbbb_bbbb_bbbb_b...      fraction
	 *   0.000h_bbbb_bbbb_bbbb_b... number
	 *  >-.----<                    shift of 4
	 */
	void denormalize(const std::bitset<nbits>& fraction, unsigned int shift, std::bitset<nbits>& number) {
		if (nbits == 3) return;
		number.reset();
		if (shift <= nbits - 1) {
			number.set(nbits - 1 - shift); // set hidden bit
			for (int i = nbits - 2 - shift; i >= 0; i--) {
				number.set(i, fraction[i + 1 + shift]);
			}
		}
	}

    // template parameters need names different from class template parameters (for gcc and clang)
	template<size_t nnbits, size_t ees>
	friend std::ostream& operator<< (std::ostream& ostr, const posit<nnbits, ees>& p);
	template<size_t nnbits, size_t ees>
	friend std::istream& operator>> (std::istream& istr, posit<nnbits, ees>& p);

	template<size_t nnbits, size_t ees>
	friend bool operator==(const posit<nnbits, ees>& lhs, const posit<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator!=(const posit<nnbits, ees>& lhs, const posit<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator< (const posit<nnbits, ees>& lhs, const posit<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator> (const posit<nnbits, ees>& lhs, const posit<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator<=(const posit<nnbits, ees>& lhs, const posit<nnbits, ees>& rhs);
	template<size_t nnbits, size_t ees>
	friend bool operator>=(const posit<nnbits, ees>& lhs, const posit<nnbits, ees>& rhs);
};
