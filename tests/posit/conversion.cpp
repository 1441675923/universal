// positive_regime.cpp : conversion operators for positive regime of posit numbers
//

#include "stdafx.h"

#include "../../posit/posit.hpp"
#include "../../posit/posit_operators.hpp"

using namespace std;

template<size_t nbits, size_t es>
void checkSpecialCases(posit<nbits, es> p) {
	cout << "posit is " << (p.isZero() ? "zero " : "non-zero ") << (p.isPositive() ? "positive " : "negative ") << (p.isInfinite() ? "+-infinite" : "not infinite") << endl;
}

void ConversionOperatorsPositiveRegime() {
	posit<5, 1> p0, p1, p2, p3, p4, p5, p6;
	double minpos, maxpos;

	cout << "Minpos = " << setprecision(7) << p0.minpos() << endl;
	cout << "Maxpos = " << p0.maxpos() << setprecision(0) << endl;

	int64_t number = 1;
	for (int i = 0; i < 8; i++) {
		p0 = number;
		cout << p0 << endl;
		number <<= 1;
	}

	return;

	p0 = int(0);  checkSpecialCases(p0);
	p1 = char(1);  cout << "P1 " << p1 << endl;
	p2 = long(2);  cout << "P2 " << p2 << endl;
	p3 =  4;  cout << "P3 " << p3 << endl;
	p4 =  8;  cout << "P4 " << p4 << endl;
	p5 = 16;  cout << "P5 " << p5 << endl;
	p6 = 32;  cout << "P6 " << p6 << endl;
}

void ConversionOperatorsNegativeRegime() {
	posit<5, 1> p0, p1, p2, p3, p4, p5, p6;
	double minpos, maxpos;

	cout << "Minpos = " << setprecision(7) << p0.minpos() << endl;
	cout << "Maxpos = " << p0.maxpos() << setprecision(0) << endl;

	p0 = 0;  checkSpecialCases(p0);
	p1 = -1;  checkSpecialCases(p1);
	p2 = -2;  checkSpecialCases(p2);
	p3 = -4;  checkSpecialCases(p3);
	p4 = -8;  checkSpecialCases(p4);
	p5 = -16;  checkSpecialCases(p5);
	p6 = -32;  checkSpecialCases(p6);
}

// what are you trying to capture with this method? TODO
// return the position of the msb of the largest binary number representable by this posit?
// this would be the scale of maxpos
template<size_t nbits, size_t es>
unsigned int maxpos_scale_f()  {
	return (nbits-2) * (1 << es);
}

unsigned int scale(unsigned int max_k, unsigned int es) {
	return max_k * (1 << es);
}

unsigned int base_regime(int64_t value, unsigned int es) {
	return (findMostSignificantBit(value) - 1) >> es;
}

unsigned int exponent(unsigned int msb, unsigned int es) {
	unsigned int value = 0;
	if (es > 0) {
		value = msb % (1 << es);
	}
	return value;
}

unsigned int fraction(int64_t value) {
	unsigned int hidden_bit_at = findMostSignificantBit(value) - 1;
	uint64_t mask = ~(1 << hidden_bit_at);
	return value & mask;
}

void EnumerationTests() {
	// set the max es we want to evaluate. useed grows very quickly as a function of es
	int max_es = 4;

	// cycle through the k values to test the scale calculation
	// since useed^k grows so quickly, we can't print the value, 
	// so instead we just print the scale of the number as measured in the binary exponent of useed^k = k*2^es
	cout << setw(10) << "posit size" << setw(6) << "max_k" << "   scale of max regime" << endl;
	cout << setw(16) << "           ";
	for (int i = 0; i < max_es; i++) {
		cout << setw(5) << "es@" << i;
	}
	cout << endl;
	for (int max_k = 1; max_k < 14; max_k++) {
		cout << setw(10) << max_k + 2 << setw(6) << max_k;
		for (int es = 0; es < max_es; es++) {
			cout << setw(6) << scale(max_k, es);
		}
		cout << endl;
	}

	// cycle through scales to test the regime determination
	cout << setw(10) << "Value";
	for (int i = 0; i < max_es; i++) {
		cout << setw(7) << "k";
	}
	cout << endl;
	unsigned int value = 1;
	for (int i = 0; i < 16; i++) {
		cout << setw(10) << value;
		for (int es = 0; es < max_es; es++) {
			cout << setw(7) << base_regime(value, es);
		}
		cout << endl;
		value <<= 1;
	}

	// cycle through a range to test the exponent extraction
	for (int i = 0; i < 32; i++) {
		cout << setw(10) << i;
		for (int es = 0; es < max_es; es++) {
			cout << setw(5) << exponent(i, es);
		}
		cout << endl;
	}

	// cycle through a range to test the faction extraction
	for (int i = 0; i < 32; i++) {
		cout << setw(10) << hex << i << setw(5) << fraction(i) << endl;
	}
}

void basic_algorithm_for_conversion() {
	const size_t nbits = 5;
	const size_t es = 1;

	int64_t value;
	unsigned int msb;
	unsigned int maxpos_scale = maxpos_scale_f<nbits, es>();

	value = 8;
	msb = findMostSignificantBit(value) - 1;
	if (msb > maxpos_scale) {
		cerr << "msb = " << msb << " and maxpos_scale() = " << maxpos_scale << endl;
		cerr << "Can't represent " << value << " with posit<" << nbits << "," << es << ">: maxpos = " << (1 << maxpos_scale) << endl;
	}
	// we need to find the regime for this rhs
	// regime represents a scale factor of useed ^ k, where k ranges from [1-nbits, nbits-2]
	// regime @ k = 0 -> 1
	// regime @ k = 1 -> (1 << (1 << es) ^ 1 = 2
	// regime @ k = 2 -> (1 << (1 << es) ^ 2 = 4
	// regime @ k = 3 -> (1 << (1 << es) ^ 3 = 8
	// the left shift of the regime is simply k * 2^es
	// which means that the msb of the regime is simply k*2^es

	// a posit has the form: useed^k * 2^exp * 1.fraction
	// useed^k is the regime and is encoded by the run length m of:
	//   - a string of 0's for numbers [0,1), and 
	//   - a string of 1's for numbers [1,inf)

	// The value k ranges from [1-nbits,nbits-2]
	//  m  s-regime   k  
	//  ...
	//  4  0-00001   -4
	//  3  0-0001    -3
	//  2  0-001     -2
	//  1  0-01      -1
	//  1  0-10       0
	//  2  0-110      1
	//  3  0-1110     2
	//  4  0-11110    3
	//  ...
	//
	// We'll be using m to convert from posit to integer/float.
	// We'll be using k to convert from integer/float to posit

	// The first step to convert an integer to a posit is to find the base regime scale
	// The base is defined as the biggest k where useed^k < integer
	// => k*2^es < msb && (k+1)*2^es > msb
	// => k < msb/2^es && k > msb/2^es - 1
	// => k = (msb >> es)

	// algorithm: convert int64 to posit<nbits,es>
	// step 1: find base regime
	//         if int64 is positive
	//            base regime = useed ^ k, where k = msb_of_int64 >> es
	//         else
	//            negate int64
	//            base regime = useed ^ k, where k = msb_of_negated_int64 >> es
	// step 2: find exponent
	//         exp = msb % 2^es
	// step 3: extract remaining fraction
	//         remove hidden bit
	// step 4: if int64 is negative, take 2's complement the posit of positive int64 calculated above
	//
	value = 33;
	cout << hex << "0x" << value << dec << setw(12) << value << endl;
	msb = findMostSignificantBit(value) - 1;
	cout << "MSB      = " << msb << endl;
	cout << "Regime   = " << base_regime(value, es) << endl;
	cout << "Exponent = " << exponent(msb, es) << endl;
	cout << "Fraction = 0x" << hex << fraction(value) << dec << endl;
}

/*
POSIT<3,0>
   #           Binary         k-value            sign          regime        exponent        fraction           value
   0:              000              -2               1            0.25               -               -               0
   1:              001              -1               1             0.5               -               -             0.5
   2:              010               0               1               1               -               -               1
   3:              011               1               1               2               -               -               2
   4:              100               2              -1               4               -               -             inf
   5:              101               1              -1               2               -               -              -2
   6:              110               0              -1               1               -               -              -1
   7:              111              -1              -1             0.5               -               -            -0.5
   */
bool ValidatePosit_3_0()
{
	float golden_answer[16] = {
		0.0, 0.5, 1.0, 2.0, INFINITY, -2.0, -1.0, -0.5
	};

	bool bValid = true;
	for (int i = 0; i < 16; i++) {
		posit<3, 0> p = golden_answer[i];
		if (fabs(p.to_double() - golden_answer[i]) > 0.00000001) {
			cerr << "Posit conversion failed: golden value = " << golden_answer[i] << " != posit<3,0> " << p << endl;
			bValid = false;
		}
	}
	return bValid;
}

/*
POSIT<4,0>
   #           Binary         k-value            sign          regime        exponent        fraction           value
   0:             0000              -3               1           0.125               -               0               0
   1:             0001              -2               1            0.25               -               0            0.25
   2:             0010              -1               1             0.5               -               0             0.5
   3:             0011              -1               1             0.5               -               1            0.75
   4:             0100               0               1               1               -               0               1
   5:             0101               0               1               1               -               1             1.5
   6:             0110               1               1               2               -               0               2
   7:             0111               2               1               4               -               0               4
   8:             1000               3              -1               8               -               0             inf
   9:             1001               2              -1               4               -               0              -4
  10:             1010               1              -1               2               -               0              -2
  11:             1011               0              -1               1               -               1            -1.5
  12:             1100               0              -1               1               -               0              -1
  13:             1101              -1              -1             0.5               -               1           -0.75
  14:             1110              -1              -1             0.5               -               0            -0.5
  15:             1111              -2              -1            0.25               -               0           -0.25
*/
bool ValidatePosit_4_0()
{
	float golden_answer[16] = {
		0.0, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 4.0, INFINITY, -4.0, -2.0, -1.5, -1.0, -0.75, -0.5, -0.25
	};

	bool bValid = true;
	for (int i = 0; i < 9; i++) {
		posit<4, 0> p = golden_answer[i];
		if (fabs(p.to_double() - golden_answer[i]) > 0.0001) {
			cerr << "Posit conversion failed: golden value = " << golden_answer[i] << " != posit<4,0> " << components_to_string(p) << endl;
			bValid = false;
		}
	}
	return bValid;
}

void PositIntegerConversion() {
	cout << "Conversion examples: (notice the rounding errors)" << endl;
	posit<5, 1> p1;
	int64_t value;
	cout << "Rounding mode : " << p1.RoundingMode() << endl;
	value =  1;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value =  2;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value =  3;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value =  4;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value =  5;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value =  7;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value =  8;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 15;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	cout << endl;
}

void ReportPositScales() {
	// print scales of different posit configurations
	// useed = 2^(2^es) and thus is just a function of the exponent configuration
	// maxpos = useed^(nbits-2)
	// minpos = useed^(2-nbits)
	posit<4, 0> p4_0;
	posit<8, 0> p8_0;
	posit<16, 1> p16_1;
	posit<32, 2> p32_2;
	posit<64, 3> p64_3;
	cout << "Posit specificiation examples and their ranges:" << endl;
	cout << spec_to_string(p4_0) << endl;
	cout << spec_to_string(p8_0) << endl;
	cout << spec_to_string(p16_1) << endl;
	cout << spec_to_string(p32_2) << endl;
	cout << spec_to_string(p64_3) << endl;
	cout << endl;
}

bool TestPositInitialization() {
	// posit initialization and assignment
	bool bValid = false;
	cout << "Posit copy constructor, assignment, and test" << endl;
	posit<16, 1> p16_1_1;
	p16_1_1 = (1 << 16);
	posit<16, 1> p16_1_2(p16_1_1);
	if (p16_1_1 != p16_1_2) {
		cerr << "Copy constructor failed" << endl;
		cerr << "value: " << (1 << 16) << " posits " << p16_1_1 << " == " << p16_1_2 << endl;
	}
	else {
		cout << "PASS" << endl;
		bValid = true;
	}
	cout << endl;
	return bValid;
}

void PositFloatConversion()
// posit float conversion
{
	cout << "Posit float conversion" << endl;
	float value;
	posit<4, 0> p1;
	cout << "Rounding mode : " << p1.RoundingMode() << endl;
	value = 0.0;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 0.25;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 0.5;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 0.75;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 1.0;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 1.5;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 2.0;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = 4.0;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	value = INFINITY;  p1 = value;	cout << "value: " << setw(2) << value << " -> posit: " << p1 << endl;
	
	cout << endl;
}

int main()
{
	/*
	ReportPositScales();
	PositIntegerConversion();
	PositFloatConversion();
	if (!TestPositInitialization()) {
		cerr << "initialization failed" << endl;
	}
	ConversionOperatorsPositiveRegime();
	*/

	posit<4, 0> p;
	p = 0; cout << p << endl;
	p = 0.25f; cout << p << endl;
	p = 1; cout << p << endl;
	p = 1.5; cout << p << endl;  // <- error
	p = 2; cout << p << endl;
	p = 4; cout << p << endl;

	//p = 0.25f; cout << p << endl;
	p = -1; cout << p << endl;
	//p = 1.5; cout << p << endl;  // <- error
	p = -2; cout << p << endl;
	p = -4; cout << p << endl;
	return 0;

	{
		cout << "Posit Configuration validation" << endl;
		if (!ValidatePosit_3_0()) {
			cout << "posit<3,0> is incorrect" << endl;
		}	
		else {
			cout << "posit<3,0> float conversions are valid" << endl;
		}
		if (!ValidatePosit_4_0()) {
			cout << "posit<4,0> is incorrect" << endl;
		}
		else {
			cout << "posit<4,0> float conversions are valid" << endl;
		}

		cout << endl;
	}

	return 0;
}


// posit<5,0> useed = 2
//  k  regime   exp   fraction regime scale   exponent scale
// -4  0-0000    -       -     0                 1
// -3  0-0001    -       -     0.125             1
// -2  0-001     -       0     0.25              1
// -1  0-01      -      00     0.5               1
//  0  0-10      -      00     1                 1
//  1  0-110     -       0     2                 1
//  2  0-1110    -       -     4                 1
//  3  0-1111    -       -     8                 1

// posit<5,1>, useed = 4
//  k  regime   exp   fraction regime scale   exponent scale
// -4  0-0000    -       -     0                 1
// -3  0-0001    -       -     0.015625          1
// -2  0-001     0       -     0.0625            2
// -1  0-01      0       0     0.25              2
//  0  0-10      0       0     1                 2
//  1  0-110     0       -     4                 2
//  2  0-1110    -       -     16                1
//  3  0-1111    -       -     64                1

// posit<5,2>, useed = 16
//  k  regime   exp   fraction regime scale   exponent scale
// -4  0-0000    -       -     0                 1
// -3  0-0001    -       -     0.0002441406      1
// -2  0-001     0       -     0.00390625        2
// -1  0-01     00       -     0.0625            4
//  0  0-10     00       -     1                 4
//  1  0-110     0       -     16                2
//  2  0-1110    -       -     256               1
//  3  0-1111    -       -     4096              1

