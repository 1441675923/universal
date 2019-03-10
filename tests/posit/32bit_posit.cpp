// 32bit_posit.cpp: Functionality tests for standard 32-bit posits
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include "common.hpp"
// Configure the posit template environment
// first: enable fast specialized posit<32,2>
#define POSIT_FAST_SPECIALIZATION   // turns on all fast specializations
//#define POSIT_FAST_POSIT_32_2 1
// second: enable posit arithmetic exceptions
#define POSIT_THROW_ARITHMETIC_EXCEPTION 1
#include <posit>
#include "../test_helpers.hpp"
#include "../posit_test_helpers.hpp"
#include "softposit32_ref.hpp"

/*
Standard posit with nbits = 32 have es = 2 exponent bits.
*/

int main(int argc, char** argv)
try {
	using namespace std;
	using namespace sw::unum;

	const size_t RND_TEST_CASES = 2000000;

	const size_t nbits = 32;
	const size_t es = 2;

	int nrOfFailedTestCases = 0;
	bool bReportIndividualTestCases = false;
	std::string tag = " posit<32,2>";

#if POSIT_FAST_POSIT_32_2
	cout << "Fast specialization posit<32,2> configuration tests" << endl;
#else
	cout << "Standard posit<32,2> configuration tests" << endl;
#endif

#ifdef later
	//FAIL 00000000000000000000000011100101 + 00000000000000000000000000011110 != 00000000000000000000000000010001 instead it yielded 00000000000000000000000011100101 s0 r000000000000000000000001 e11 f00101 qSE v + 1.8680226235633559552e-27
	//FAIL 00000000000000000000000000000001 + 00000000000000000000000000001101 != 00000000000000000000000001111101 instead it yielded 00000000000000000000000000001101 s0 r0000000000000000000000000001 e10 f1 qSE v + 1.8488927466117464189e-32
	//FAIL 00000000000000000000000010001010 + 00000000000000000000000000000001 != 00000000000000000000000011110110 instead it yielded 00000000000000000000000010001010 s0 r000000000000000000000001 e00 f01010 qSE v + 2.6505726415425996662e-28
		F
	posit32_t a, b, c;
	a = 0x0000'00B2;
	b = 0x0000'00D3;
	c = p32_sub(a, b);
	cout << hex;
	cout << "a = 32.2x" << a << endl;
	cout << "b = 32.2x" << b << endl;
	cout << "c = 32.2x" << c << endl;
	cout << dec;

	posit<nbits, es> x, y, z;
	x.set_raw_bits(0x00B2);
	y.set_raw_bits(0x00D3);
	z = x - y;
	cout << "x = " << posit_format(x) << endl;
	cout << "y = " << posit_format(y) << endl;
	cout << "z = " << posit_format(z) << endl;

	return 1;
#endif

	posit<nbits, es> p;
	cout << dynamic_range(p) << endl << endl;
#define now_
#ifdef now
	// logic tests
	nrOfFailedTestCases += ReportTestResult( ValidatePositLogicEqual             <nbits, es>(), tag, "    ==         ");
	nrOfFailedTestCases += ReportTestResult( ValidatePositLogicNotEqual          <nbits, es>(), tag, "    !=         ");
	nrOfFailedTestCases += ReportTestResult( ValidatePositLogicLessThan          <nbits, es>(), tag, "    <          ");
	nrOfFailedTestCases += ReportTestResult( ValidatePositLogicLessOrEqualThan   <nbits, es>(), tag, "    <=         ");
	nrOfFailedTestCases += ReportTestResult( ValidatePositLogicGreaterThan       <nbits, es>(), tag, "    >          ");
	nrOfFailedTestCases += ReportTestResult( ValidatePositLogicGreaterOrEqualThan<nbits, es>(), tag, "    >=         ");
	// conversion tests
	// disabled until we can constrain the state space 2^33 is too big
	nrOfFailedTestCases += ReportTestResult( ValidateIntegerConversion<nbits, es>(tag, bReportIndividualTestCases), tag, "sint32 assign  ");
	nrOfFailedTestCases += ReportTestResult( ValidateUintConversion<nbits, es>(tag, bReportIndividualTestCases), tag, "uint32 assign  ");
	nrOfFailedTestCases += ReportTestResult( ValidateConversion <nbits, es>(tag, bReportIndividualTestCases), tag, "float assign   ");
//	nrOfFailedTestCases += ReportTestResult( ValidateConversionThroughRandoms <nbits, es>(tag, true, 100), tag, "float assign   ");
#endif
	cout << "Arithmetic tests " << RND_TEST_CASES << " randoms each" << endl;
	nrOfFailedTestCases += ReportTestResult( ValidateThroughRandoms<nbits, es>(tag, bReportIndividualTestCases, OPCODE_ADD, RND_TEST_CASES),  tag, "addition       ");
	nrOfFailedTestCases += ReportTestResult( ValidateThroughRandoms<nbits, es>(tag, bReportIndividualTestCases, OPCODE_SUB, RND_TEST_CASES),  tag, "subtraction    ");
	nrOfFailedTestCases += ReportTestResult( ValidateThroughRandoms<nbits, es>(tag, true, OPCODE_MUL, RND_TEST_CASES),  tag, "multiplication ");
//	nrOfFailedTestCases += ReportTestResult( ValidateThroughRandoms<nbits, es>(tag, bReportIndividualTestCases, OPCODE_DIV, RND_TEST_CASES),  tag, "division       ");
//	nrOfFailedTestCases += ReportTestResult( ValidateThroughRandoms<nbits, es>(tag, bReportIndividualTestCases, OPCODE_SQRT, RND_TEST_CASES), tag, "sqrt           ");

	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const posit_arithmetic_exception& err) {
	std::cerr << "Uncaught posit arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const quire_exception& err) {
	std::cerr << "Uncaught quire exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const posit_internal_exception& err) {
	std::cerr << "Uncaught posit internal exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::runtime_error& err) {
	std::cerr << "Uncaught runtime exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}

