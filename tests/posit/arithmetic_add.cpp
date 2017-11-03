// arithmetic_add.cpp: functional tests for addition
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "stdafx.h"

#include <vector>

#define POSIT_THROW_FOR_INCORRECT_CALCULATION               // to stop at first addition bugs
// #define POSIT_VERBOSE_OUTPUT                                // trace everything
// #define POSIT_ROUND_TIES_TO_ZERO                            // In tie case round toward zero (for consistency with Theo's tests)
#define POSIT_USE_LONG_VALUE_IN_CONVERSION                  // Use all mantissa bit in value and round later

#include "../../bitset/bitset_helpers.hpp"
#include "../../posit/posit_regime_lookup.hpp"
#include "../../posit/posit.hpp"
#include "../../posit/posit_operators.hpp"
#include "../../posit/posit_manipulators.hpp"
#include "../tests/test_helpers.hpp"
#include "../tests/posit_test_helpers.hpp"

using namespace std;

// generate specific test case that you can trace with the trace conditions in posit.h
// for most bugs they are traceable with _trace_conversion and _trace_add
template<size_t nbits, size_t es>
void GenerateTestCase(float fa, float fb) {
	posit<nbits, es> pa, pb, pref, psum;
	pa = fa;
	pb = fb;
	pref = fa + fb;
	psum = pa + pb;
	cout << "reference " << pref << " result " << psum << endl << endl;
}

template<size_t nbits, size_t es>
void GenerateTestCase(double da, double db) {
	posit<nbits, es> pa, pb, pref, psum;
	pa = da;
	pb = db;
	pref = da + db;
	psum = pa + pb;
	cout << "reference " << pref << " result " << psum << endl << endl;
}

int main(int argc, char** argv)
try
{
	int nrOfFailedTests = 0;
	bool bReportIndividualTestCases = false;

	// generate individual testcases to hand trace/debug
// 	GenerateTestCase<5, 0>(-0.625f,  4.000f);
// 	GenerateTestCase<5, 0>(-0.500f,  4.000f);

// 	GenerateTestCase<3, 0>(1.0f,  1.0f);
//      GenerateTestCase<4, 0>(0.25f,  0.75f);
//         GenerateTestCase<4, 0>(0.25f,  1.5f);
//         GenerateTestCase<4, 0>(0.75f,  0.75f);
//         posit<4, 0> p = 2.75;
//         GenerateTestCase<4, 0>(0.75f,  2.0f);
        
        posit<3, 0> p2 = 1.5;
        GenerateTestCase<3, 0>(0.5f,  1.0f);

	/*
	// previous bugs that where hand traced
	GenerateTestCase<5, 0>( 0.125f,  0.250f);
	GenerateTestCase<5, 0>( 0.125f,  1.000f);
	GenerateTestCase<5, 0>( 0.250f, -1.000f);
	GenerateTestCase<5, 0>( 0.250f,  0.375f);
	*/

#if 1
	nrOfFailedTests += ReportTestResult(ValidateAddition<3, 0>("Posit<3,0> addition failed: ", bReportIndividualTestCases), "posit<3,0>", "addition");

	nrOfFailedTests += ReportTestResult(ValidateAddition<4, 0>("Posit<4,0> addition failed: ", bReportIndividualTestCases), "posit<4,0>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<4, 1>("Posit<4,1> addition failed: ", bReportIndividualTestCases), "posit<4,1>", "addition");

	nrOfFailedTests += ReportTestResult(ValidateAddition<5, 0>("Posit<5,0> addition failed: ", bReportIndividualTestCases), "posit<5,0>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<5, 1>("Posit<5,1> addition failed: ", bReportIndividualTestCases), "posit<5,1>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<5, 2>("Posit<5,2> addition failed: ", bReportIndividualTestCases), "posit<5,2>", "addition");

	nrOfFailedTests += ReportTestResult(ValidateAddition<6, 0>("Posit<6,0> addition failed: ", bReportIndividualTestCases), "posit<6,0>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<6, 1>("Posit<6,1> addition failed: ", bReportIndividualTestCases), "posit<6,1>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<6, 2>("Posit<6,2> addition failed: ", bReportIndividualTestCases), "posit<6,2>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<6, 3>("Posit<6,3> addition failed: ", bReportIndividualTestCases), "posit<6,3>", "addition");

	nrOfFailedTests += ReportTestResult(ValidateAddition<7, 0>("Posit<7,0> addition failed: ", bReportIndividualTestCases), "posit<7,0>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<7, 1>("Posit<7,1> addition failed: ", bReportIndividualTestCases), "posit<7,1>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<7, 2>("Posit<7,2> addition failed: ", bReportIndividualTestCases), "posit<7,2>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<7, 3>("Posit<7,3> addition failed: ", bReportIndividualTestCases), "posit<7,3>", "addition");

	nrOfFailedTests += ReportTestResult(ValidateAddition<8, 0>("Posit<8,0> addition failed: ", bReportIndividualTestCases), "posit<8,0>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<8, 1>("Posit<8,1> addition failed: ", bReportIndividualTestCases), "posit<8,1>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<8, 2>("Posit<8,2> addition failed: ", bReportIndividualTestCases), "posit<8,2>", "addition");
	nrOfFailedTests += ReportTestResult(ValidateAddition<8, 3>("Posit<8,3> addition failed: ", bReportIndividualTestCases), "posit<8,3>", "addition");
#endif
	// very long running, so disabled
	//ReportTestResult(ValidateAddition<16, 1>("Posit<16,1> addition failed: ", bReportIndividualTestCases), "posit<16,1>", "addition");
	return nrOfFailedTests;
}
catch (char* e) {
	cerr << e << endl;
	return -1;
}
