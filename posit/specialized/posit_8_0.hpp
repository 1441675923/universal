#pragma once
// posit_8_0.hpp: specialized 8-bit posit using fast compute specialized for posit<8,0>
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

namespace sw {
	namespace unum {

		// set the fast specialization variable to indicate that we are running a special template specialization
#ifdef POSIT_FAST_SPECIALIZATION
#define POSIT_FAST_POSIT_8_0 1

			template<>
			class posit<NBITS_IS_8, ES_IS_0> {
			public:
				static constexpr size_t nbits = NBITS_IS_8;
				static constexpr size_t es = ES_IS_0;
				static constexpr size_t sbits = 1;
				static constexpr size_t rbits = nbits - sbits;
				static constexpr size_t ebits = es;
				static constexpr size_t fbits = nbits - 3;
				static constexpr size_t fhbits = fbits + 1;
				static constexpr uint8_t index_shift = 4;

				posit() { _bits = 0; }
				posit(const posit&) = default;
				posit(posit&&) = default;
				posit& operator=(const posit&) = default;
				posit& operator=(posit&&) = default;

				// initializers for native types
				posit(const signed char initial_value)        { *this = initial_value; }
				posit(const short initial_value)              { *this = initial_value; }
				posit(const int initial_value)                { *this = initial_value; }
				posit(const long initial_value)               { *this = initial_value; }
				posit(const long long initial_value)          { *this = initial_value; }
				posit(const char initial_value)               { *this = initial_value; }
				posit(const unsigned short initial_value)     { *this = initial_value; }
				posit(const unsigned int initial_value)       { *this = initial_value; }
				posit(const unsigned long initial_value)      { *this = initial_value; }
				posit(const unsigned long long initial_value) { *this = initial_value; }
				posit(const float initial_value)              { *this = initial_value; }
				posit(const double initial_value)             { *this = initial_value; }
				posit(const long double initial_value)        { *this = initial_value; }

				// assignment operators for native types
				posit& operator=(const signed char rhs)       { 
					// special case for speed as this is a common initialization
					if (rhs == 0) {
						_bits = 0x00;
						return *this;
					}

					bool sign = bool(rhs & 0x80);
					int8_t v = sign ? -rhs : rhs; // project to positve side of the projective reals
					uint8_t raw;
					if (v > 48 || v == -128) { // +-maxpos, 0x80 is special in int8 arithmetic as it is its own negation
						raw = 0x7F;
					}
					else {
						uint8_t mask = 0x40;
						int8_t k = 6;
						uint8_t fraction_bits = v;
						while (!(fraction_bits & mask)) {
							k--;
							fraction_bits <<= 1;
						}
						fraction_bits = (fraction_bits ^ mask);
						raw = (0x7F ^ (0x3F >> k)) | (fraction_bits >> (k + 1));

						mask = 0x1 << k; //bitNPlusOne
						if (mask & fraction_bits) {
							if (((mask - 1) & fraction_bits) | ((mask << 1) & fraction_bits)) raw++;
						}
					}
					_bits = sign ? -raw : raw;
					return *this;
				}
				posit& operator=(const short rhs)             { return operator=((signed char)(rhs)); }
				posit& operator=(const int rhs)               { return operator=((signed char)(rhs)); }
				posit& operator=(const long rhs)              { return operator=((signed char)(rhs)); }
				posit& operator=(const long long rhs)         { return operator=((signed char)(rhs)); }
				posit& operator=(const char rhs)              { return operator=((signed char)(rhs)); }
				posit& operator=(const unsigned short rhs)    { return operator=((signed char)(rhs)); }
				posit& operator=(const unsigned int rhs)      { return operator=((signed char)(rhs)); }
				posit& operator=(const unsigned long rhs)     { return operator=((signed char)(rhs)); }
				posit& operator=(const unsigned long long rhs){ return operator=((signed char)(rhs)); }
				posit& operator=(const float rhs)             { return float_assign(rhs); }
				posit& operator=(const double rhs)            { return float_assign(rhs); }
				posit& operator=(const long double rhs)       { return float_assign(rhs); }

				explicit operator long double() const { return to_long_double(); }
				explicit operator double() const { return to_double(); }
				explicit operator float() const { return to_float(); }
				explicit operator long long() const { return to_long_long(); }
				explicit operator long() const { return to_long(); }
				explicit operator int() const { return to_int(); }
				explicit operator unsigned long long() const { return to_long_long(); }
				explicit operator unsigned long() const { return to_long(); }
				explicit operator unsigned int() const { return to_int(); }

				posit& set(sw::unum::bitblock<NBITS_IS_8>& raw) {
					_bits = uint8_t(raw.to_ulong());
					return *this;
				}
				posit& set_raw_bits(uint64_t value) {
					_bits = uint8_t(value & 0xff);
					return *this;
				}
				posit operator-() const {
					if (iszero()) {
						return *this;
					}
					if (isnar()) {
						return *this;
					}
					posit p;
					return p.set_raw_bits((~_bits) + 1);
				}
				posit& operator+=(const posit& b) { // derived from SoftPosit
					uint8_t lhs = _bits;
					uint8_t rhs = b._bits;
					// process special cases
					if (isnar() || b.isnar()) {  // infinity
						_bits = 0x80;
						return *this;
					}
					if (iszero() || b.iszero()) { // zero
						_bits = lhs | rhs;
						return *this;
					}
					bool sign = bool(_bits & 0x80);
					if (sign) {
						lhs = -lhs & 0xFF;
						rhs = -rhs & 0xFF;
					}
					if (lhs < rhs) std::swap(lhs, rhs);
					
					// decode the regime of lhs
					int8_t m = 0; // pattern length
					uint8_t remaining = 0;
					decode_regime(lhs, m, remaining);
					uint16_t frac16A = (0x80 | remaining) << 7;
					int8_t shiftRight = m;
					// adjust shift and extract fraction bits of rhs
					adjust(rhs, shiftRight, remaining);
					uint16_t frac16B = (0x80 | remaining) << 7;

					// Work-around CLANG (LLVM) compiler when shifting right more than number of bits
					(shiftRight>7) ? (frac16B = 0) : (frac16B >>= shiftRight); 

					frac16A += frac16B;

					bool rcarry = bool(0x8000 & frac16A); // is MSB set
					if (rcarry) {
						m++;
						frac16A >>= 1;
					}

					_bits = round(m, frac16A);
					if (sign) _bits = -_bits & 0xFF;
					return *this;
				}
				posit& operator-=(const posit& b) {  // derived from SoftPosit
					uint8_t lhs = _bits;
					uint8_t rhs = b._bits;
					// process special cases
					if (isnar() || b.isnar()) {
						_bits = 0x80;
						return *this;
					}
					if (iszero() || b.iszero()) {
						_bits = lhs | rhs;
						return *this;
					}
					// Both operands are actually the same sign if rhs inherits sign of sub: Make both positive
					bool sign = bool(lhs & 0x80);
					(sign) ? (lhs = (-lhs & 0xFF)) : (rhs = (-rhs & 0xFF));

					if (lhs == rhs) {
						_bits = 0x00;
						return *this;
					}
					if (lhs < rhs) {
						std::swap(lhs, rhs);
						sign = !sign;
					}

					// decode the regime of lhs
					int8_t m = 0; // pattern length
					uint8_t remaining = 0;
					decode_regime(lhs, m, remaining);
					uint16_t frac16A = (0x80 | remaining) << 7;
					int8_t shiftRight = m;
					// adjust shift and extract fraction bits of rhs
					adjust(rhs, shiftRight, remaining);
					uint16_t frac16B = (0x80 | remaining) << 7;

					// do the subtraction of the fractions
					if (shiftRight >= 14) {
						_bits = lhs;
						if (sign) _bits = -_bits & 0xFFFF;
						return *this;
					}
					else {
						frac16B >>= shiftRight;
					}
					frac16A -= frac16B;

					while ((frac16A >> 14) == 0) {
						m--;
						frac16A <<= 1;
					}
					bool ecarry = bool (0x4000 & frac16A);
					if (!ecarry) {
						m--;
						frac16A <<= 1;
					}

					_bits = round(m, frac16A);
					if (sign) _bits = -_bits & 0xFF;
					return *this;
				}
				posit& operator*=(const posit& b) {
					uint8_t lhs = _bits;
					uint8_t rhs = b._bits;
					// process special cases
					if (isnar() || b.isnar()) {
						_bits = 0x80;
						return *this;
					}
					if (iszero() || b.iszero()) {
						_bits = 0x00;
						return *this;
					}

					// calculate the sign of the result
					bool sign = bool(lhs & 0x80) ^ bool(rhs & 0x80);
					lhs = lhs & 0x80 ? -lhs : lhs;
					rhs = rhs & 0x80 ? -rhs : rhs;

					// decode the regime of lhs
					int8_t m = 0; // pattern length
					uint8_t remaining = 0;
					decode_regime(lhs, m, remaining);
					uint8_t lhs_fraction = (0x80 | remaining);
					// adjust shift and extract fraction bits of rhs
					extract(rhs, m, remaining);
					uint8_t rhs_fraction = (0x80 | remaining);
					uint16_t result_fraction = uint16_t(lhs_fraction) * uint16_t(rhs_fraction);

					bool rcarry = bool(result_fraction & 0x8000);
					if (rcarry) {
						m++;
						result_fraction >>= 1;
					}

					_bits = round(m, result_fraction);
					if (sign) _bits = -_bits & 0xFF;
					return *this;
				}
				posit& operator/=(const posit& b) {

					return *this;
				}
				posit& operator++() {
					++_bits;
					return *this;
				}
				posit operator++(int) {
					posit tmp(*this);
					operator++();
					return tmp;
				}
				posit& operator--() {
					--_bits;
					return *this;
				}
				posit operator--(int) {
					posit tmp(*this);
					operator--();
					return tmp;
				}
				posit reciprocate() const {
					posit p;

					return p;
				}
				// SELECTORS
				inline bool isnar() const      { return (_bits == 0x80); }
				inline bool iszero() const     { return (_bits == 0x00); }
				inline bool isone() const      { return (_bits == 0x40); } // pattern 010000...
				inline bool isminusone() const { return (_bits == 0xC0); } // pattern 110000...
				inline bool isneg() const      { return (_bits & 0x80); }
				inline bool ispos() const      { return !isneg(); }
				inline bool ispowerof2() const { return !(_bits & 0x1); }

				inline int sign_value() const  { return (_bits & 0x8 ? -1 : 1); }

				bitblock<NBITS_IS_8> get() const { bitblock<NBITS_IS_8> bb; bb = int(_bits); return bb; }
				unsigned long long encoding() const { return (unsigned long long)(_bits); }

				inline void clear() { _bits = 0; }
				inline void setzero() { clear(); }
				inline void setnar() { _bits = 0x80; }
				inline posit twosComplement() const {
					posit<NBITS_IS_8, ES_IS_0> p;
					int8_t v = -*(int8_t*)&_bits;
					p.set_raw_bits(v);
					return p;
				}
			private:
				uint8_t _bits;

				// Conversion functions
#if POSIT_THROW_ARITHMETIC_EXCEPTION
				int         to_int() const {
					if (iszero()) return 0;
					if (isnar()) throw not_a_real{};
					return int(to_float());
				}
				long        to_long() const {
					if (iszero()) return 0;
					if (isnar()) throw not_a_real{};
					return long(to_double());
				}
				long long   to_long_long() const {
					if (iszero()) return 0;
					if (isnar()) throw not_a_real{};
					return long(to_long_double());
				}
#else
				int         to_int() const {
					if (iszero()) return 0;
					if (isnar())  return int(INFINITY);
					return int(to_float());
				}
				long        to_long() const {
					if (iszero()) return 0;
					if (isnar())  return long(INFINITY);
					return long(to_double());
				}
				long long   to_long_long() const {
					if (iszero()) return 0;
					if (isnar())  return (long long)(INFINITY);
					return long(to_long_double());
				}
#endif
				float       to_float() const {
					return (float)to_double();
				}
				double      to_double() const {
					if (iszero())	return 0.0;
					if (isnar())	return NAN;
					bool		     	 _sign;
					regime<nbits, es>    _regime;
					exponent<nbits, es>  _exponent;
					fraction<fbits>      _fraction;
					bitblock<nbits>		 _raw_bits;
					_raw_bits.reset();
					uint64_t mask = 1;
					for (size_t i = 0; i < nbits; i++) {
						_raw_bits.set(i, (_bits & mask));
						mask <<= 1;
					}
					decode(_raw_bits, _sign, _regime, _exponent, _fraction);
					double s = (_sign ? -1.0 : 1.0);
					double r = _regime.value();
					double e = _exponent.value();
					double f = (1.0 + _fraction.value());
					return s * r * e * f;
				}
				long double to_long_double() const {
					if (iszero())  return 0.0;
					if (isnar())   return NAN;
					bool		     	 _sign;
					regime<nbits, es>    _regime;
					exponent<nbits, es>  _exponent;
					fraction<fbits>      _fraction;
					bitblock<nbits>		 _raw_bits;
					_raw_bits.reset();
					uint64_t mask = 1;
					for (size_t i = 0; i < nbits; i++) {
						_raw_bits.set(i, (_bits & mask));
						mask <<= 1;
					}
					decode(_raw_bits, _sign, _regime, _exponent, _fraction);
					long double s = (_sign ? -1.0 : 1.0);
					long double r = _regime.value();
					long double e = _exponent.value();
					long double f = (1.0 + _fraction.value());
					return s * r * e * f;
				}

				template <typename T>
				posit& float_assign(const T& rhs) {
					constexpr int dfbits = std::numeric_limits<T>::digits - 1;
					value<dfbits> v((T)rhs);

					// special case processing
					if (v.iszero()) {
						setzero();
						return *this;
					}
					if (v.isinf() || v.isnan()) {  // posit encode for FP_INFINITE and NaN as NaR (Not a Real)
						setnar();
						return *this;
					}

					bitblock<NBITS_IS_8> ptt;
					convert_to_bb<NBITS_IS_8, ES_IS_0, dfbits>(v.sign(), v.scale(), v.fraction(), ptt); // TODO: needs to be faster
					_bits = uint8_t(ptt.to_ulong());
					return *this;
				}

				// helper method
				// decode_regime takes the raw bits of the posit, and returns the regime run-length, m, and the remaining fraction bits in remainder
				inline void decode_regime(const uint8_t bits, int8_t& m, uint8_t& remaining) const {
					remaining = (bits << 2) & 0xFF;
					if (bits & 0x40) {  // positive regimes
						while (remaining >> 7) {
							m++;
							remaining = (remaining << 1) & 0xFF;
						}
					}
					else {              // negative regimes
						m = -1;
						while (!(remaining >> 7)) {
							m--;
							remaining = (remaining << 1) & 0xFF;
						}
						remaining &= 0x7F;
					}
				}
				inline void adjust(const uint8_t bits, int8_t& shiftRight, uint8_t& remaining) const {
					remaining = (bits << 2) & 0xFF;
					if (bits & 0x40) {  // positive regimes
						while (remaining >> 7) {
							shiftRight--;
							remaining = (remaining << 1) & 0xFF;
						}
					}
					else {              // negative regimes
						shiftRight++;
						while (!(remaining >> 7)) {
							shiftRight++;
							remaining = (remaining << 1) & 0xFF;
						}
						remaining &= 0x7F;
					}
				}
				inline void extract(const uint8_t bits, int8_t& m, uint8_t& remaining) const {
					remaining = (bits << 2) & 0xFF;
					if (bits & 0x40) {  // positive regimes
						while (remaining >> 7) {
							m++;
							remaining = (remaining << 1) & 0xFF;
						}
					}
					else {              // negative regimes
						m--;
						while (!(remaining >> 7)) {
							m--;
							remaining = (remaining << 1) & 0xFF;
						}
						remaining &= 0x7F;
					}
				}
				inline uint8_t round(const int8_t m, uint16_t fraction) const {
					uint8_t scale, regime, bits;
					if (m < 0) {
						scale = (-m & 0xFF);
						regime = 0x40 >> scale;
					}
					else {
						scale = m + 1;
						regime = 0x7F - (0x7F >> scale);
					}

					if (scale > 6) {
						bits = m<0 ? 0x1 : 0x7F;  // minpos and maxpos
					}
					else {
						fraction = (fraction & 0x3FFF) >> scale;
						uint8_t final_fbits = uint8_t(fraction >> 8);
						bool bitNPlusOne = bool(0x80 & fraction);
						bits = uint8_t(regime) + uint8_t(final_fbits);
						// n+1 frac bit is 1. Need to check if another bit is 1 too if not round to even
						if (bitNPlusOne) {
							uint8_t moreBits = (0x7F & fraction) ? 0x01 : 0x00;
							bits += (bits & 0x01) | moreBits;
						}
					}
					return bits;
				}
				// I/O operators
				friend std::ostream& operator<< (std::ostream& ostr, const posit<NBITS_IS_8, ES_IS_0>& p);
				friend std::istream& operator>> (std::istream& istr, posit<NBITS_IS_8, ES_IS_0>& p);

				// posit - posit logic functions
				friend bool operator==(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs);
				friend bool operator!=(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs);
				friend bool operator< (const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs);
				friend bool operator> (const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs);
				friend bool operator<=(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs);
				friend bool operator>=(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs);

			};

			// posit I/O operators
			// generate a posit format ASCII format nbits.esxNN...NNp
			inline std::ostream& operator<<(std::ostream& ostr, const posit<NBITS_IS_8, ES_IS_0>& p) {
				// to make certain that setw and left/right operators work properly
				// we need to transform the posit into a string
				std::stringstream ss;
#if POSIT_ROUNDING_ERROR_FREE_IO_FORMAT
				ss << NBITS_IS_8 << '.' << ES_IS_0 << 'x' << to_hex(p.get()) << 'p';
#else
				std::streamsize prec = ostr.precision();
				std::streamsize width = ostr.width();
				std::ios_base::fmtflags ff;
				ff = ostr.flags();
				ss.flags(ff);
				ss << std::showpos << std::setw(width) << std::setprecision(prec) << (long double)p;
#endif
				return ostr << ss.str();
			}

			// read an ASCII float or posit format: nbits.esxNN...NNp, for example: 32.2x80000000p
			inline std::istream& operator>> (std::istream& istr, posit<NBITS_IS_8, ES_IS_0>& p) {
				std::string txt;
				istr >> txt;
				if (!parse(txt, p)) {
					std::cerr << "unable to parse -" << txt << "- into a posit value\n";
				}
				return istr;
			}

			// convert a posit value to a string using "nar" as designation of NaR
			std::string to_string(const posit<NBITS_IS_8, ES_IS_0>& p, std::streamsize precision) {
				if (p.isnar()) {
					return std::string("nar");
				}
				std::stringstream ss;
				ss << std::setprecision(precision) << float(p);
				return ss.str();
			}

			// posit - posit binary logic operators
			inline bool operator==(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return lhs._bits == rhs._bits;
			}
			inline bool operator!=(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return !operator==(lhs, rhs);
			}
			inline bool operator< (const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return *(signed char*)(&lhs._bits) < *(signed char*)(&rhs._bits);
			}
			inline bool operator> (const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return operator< (rhs, lhs);
			}
			inline bool operator<=(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return operator< (lhs, rhs) || operator==(lhs, rhs);
			}
			inline bool operator>=(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return !operator< (lhs, rhs);
			}

			inline posit<NBITS_IS_8, ES_IS_0> operator+(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				posit<NBITS_IS_8, ES_IS_0> result = lhs;
				if (lhs.isneg() == rhs.isneg()) {  // are the posits the same sign?
					result += rhs;
				} 
				else {
					result -= rhs;
				}
				return result;
			}
			inline posit<NBITS_IS_8, ES_IS_0> operator-(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				posit<NBITS_IS_8, ES_IS_0> result = lhs;
				if (lhs.isneg() == rhs.isneg()) {  // are the posits the same sign?
					result -= rhs.twosComplement();
				}
				else {
					result += rhs.twosComplement();
				}
				return result;

			}
			// binary operator*() is provided by generic class

#if POSIT_ENABLE_LITERALS
			// posit - literal logic functions

			// posit - int logic operators
			inline bool operator==(const posit<NBITS_IS_8, ES_IS_0>& lhs, int rhs) {
				return operator==(lhs, posit<NBITS_IS_8, ES_IS_0>(rhs));
			}
			inline bool operator!=(const posit<NBITS_IS_8, ES_IS_0>& lhs, int rhs) {
				return !operator==(lhs, posit<NBITS_IS_8, ES_IS_0>(rhs));
			}
			inline bool operator< (const posit<NBITS_IS_8, ES_IS_0>& lhs, int rhs) {
				return operator<(lhs, posit<NBITS_IS_8, ES_IS_0>(rhs));
			}
			inline bool operator> (const posit<NBITS_IS_8, ES_IS_0>& lhs, int rhs) {
				return operator< (posit<NBITS_IS_8, ES_IS_0>(rhs), lhs);
			}
			inline bool operator<=(const posit<NBITS_IS_8, ES_IS_0>& lhs, int rhs) {
				return operator< (lhs, posit<NBITS_IS_8, ES_IS_0>(rhs)) || operator==(lhs, posit<NBITS_IS_8, ES_IS_0>(rhs));
			}
			inline bool operator>=(const posit<NBITS_IS_8, ES_IS_0>& lhs, int rhs) {
				return !operator<(lhs, posit<NBITS_IS_8, ES_IS_0>(rhs));
			}

			// int - posit logic operators
			inline bool operator==(int lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return posit<NBITS_IS_8, ES_IS_0>(lhs) == rhs;
			}
			inline bool operator!=(int lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return !operator==(posit<NBITS_IS_8, ES_IS_0>(lhs), rhs);
			}
			inline bool operator< (int lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return operator<(posit<NBITS_IS_8, ES_IS_0>(lhs), rhs);
			}
			inline bool operator> (int lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return operator< (posit<NBITS_IS_8, ES_IS_0>(rhs), lhs);
			}
			inline bool operator<=(int lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return operator< (posit<NBITS_IS_8, ES_IS_0>(lhs), rhs) || operator==(posit<NBITS_IS_8, ES_IS_0>(lhs), rhs);
			}
			inline bool operator>=(int lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
				return !operator<(posit<NBITS_IS_8, ES_IS_0>(lhs), rhs);
			}

#endif
	}

#else 
#define POSIT_FAST_POSIT_8_0 0
#endif

}
