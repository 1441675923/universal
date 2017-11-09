// conversion.cpp : functional tests for conversion operators to posit numbers
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "stdafx.h"

// if you want to trace the posit intermediate results
#define POSIT_VERBOSE_OUTPUT

#include "../../posit/posit.hpp"
#include "../../posit/posit_operators.hpp"
#include "../../posit/posit_manipulators.hpp"
#include "../tests/test_helpers.hpp"
#include "../tests/posit_test_helpers.hpp"

using namespace std;

// generate specific test case that you can trace with the trace conditions in posit.h
// for most bugs they are traceable with _trace_conversion and _trace_add
template<size_t nbits, size_t es>
void GenerateTestCase(float input, float reference, const posit<nbits, es>& presult) {
	if (fabs(presult.to_double() - reference) > 0.000000001) 
		ReportConversionError("test_case", "=", input, reference, presult);
	else
		ReportConversionSuccess("test_case", "=", input, reference, presult);
	cout << endl;
}

template<size_t nbits, size_t es>
void GenerateTestCase(double input, double reference, const posit<nbits, es>& presult) {
	if (fabs(presult.to_double() - reference) > 0.000000001)
		ReportConversionError("test_case", "=", input, reference, presult);
	else
		ReportConversionSuccess("test_case", "=", input, reference, presult);
	cout << endl;
}

#define MANUAL_TESTING 1
#define STRESS_TESTING 0

int main(int argc, char** argv)
try {
	bool bReportIndividualTestCases = false;
	int nrOfFailedTestCases = 0;

	std::string tag = "Conversion failed";

#if MANUAL_TESTING
	// generate individual testcases to hand trace/debug
	double input, reference;
	
#if 0
	cout << "-1 >> 2 =" << (-1 >> 2) << endl;
	cout << "-2 >> 2 =" << (-2 >> 2) << endl;
	cout << "-3 >> 2 =" << (-3 >> 2) << endl;
	cout << "-4 >> 2 =" << (-4 >> 2) << endl;
	cout << "-5 >> 2 =" << (-5 >> 2) << endl;
	cout << "-6 >> 2 =" << (-6 >> 2) << endl;
	cout << "-7 >> 2 =" << (-7 >> 2) << endl;
	cout << "-8 >> 2 =" << (-8 >> 2) << endl;
	cout << "-9 >> 2 =" << (-9 >> 2) << endl;
	cout << "-10 >> 2 =" << (-10 >> 2) << endl;
	cout << " 1 >> 2 =" << ( 1 >> 2) << endl;
	cout << " 2 >> 2 =" << ( 2 >> 2) << endl;
	cout << " 3 >> 2 =" << ( 3 >> 2) << endl;
	cout << " 4 >> 2 =" << ( 4 >> 2) << endl;
	cout << " 5 >> 2 =" << ( 5 >> 2) << endl;
	cout << " 6 >> 2 =" << ( 6 >> 2) << endl;
	cout << " 7 >> 2 =" << ( 7 >> 2) << endl;
	cout << " 8 >> 2 =" << ( 8 >> 2) << endl;
	cout << " 9 >> 2 =" << ( 9 >> 2) << endl;
	cout << "10 >> 2 =" << (10 >> 2) << endl;
#endif

#if PREVIOUS_FAILURE_INPUTS
	posit<4, 1> p;
	input = 0.0625f; reference = 0.0625f; 
	input = 0.1249f; reference = 0.0625f;
	input = 0.1251f; reference = 0.25f;
	input = 0.249999999f; reference = 0.25f;
	input = 4.000001f; reference = 4.0f;
	posit<5, 2> p;
	input = 32.0001; reference = 64; 
	input = 63.9999; reference = 64; 
	input = 128.0001; reference = 256;
	input = 255.9999; reference = 256;
	input = 256.0001; reference = 256;
	input = 1023.9999; reference = 256;
#endif
	posit<5, 2> p;
	input = 8.0; reference = 8.0;
	p = input;
	GenerateTestCase(input, reference, p);
	input = 0.5; reference = 0.5;
	p = input;
	GenerateTestCase(input, reference, p);
	input = 0.50001; reference = 0.5;
	p = input;
	GenerateTestCase(input, reference, p);
	input = 0.74999; reference = 0.5;
	p = input;	
	GenerateTestCase(input, reference, p);
	return 0;
	// manual exhaustive testing
	tag = "Manual Testing";
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<5, 2>(tag, true), "posit<5,2>", "conversion");
	//nrOfFailedTestCases += ReportTestResult(ValidateConversion<6, 3>(tag, true), "posit<6,3>", "conversion");
	return 0;

	nrOfFailedTestCases += ReportTestResult(ValidateConversion<4, 0>(tag, true), "posit<4,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<4, 1>(tag, true), "posit<4,1>", "conversion"); 
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<5, 0>(tag, true), "posit<5,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<5, 1>(tag, true), "posit<5,1>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<5, 2>(tag, true), "posit<5,2>", "conversion");
	return 0;
	nrOfFailedTestCases += ReportTestResult(ValidateAddition<6, 0>("Posit<6,0> addition failed: ", bReportIndividualTestCases), "posit<6,0>", "addition");
	nrOfFailedTestCases += ReportTestResult(ValidateAddition<6, 1>("Posit<6,1> addition failed: ", bReportIndividualTestCases), "posit<6,1>", "addition");
	nrOfFailedTestCases += ReportTestResult(ValidateAddition<6, 2>("Posit<6,2> addition failed: ", bReportIndividualTestCases), "posit<6,2>", "addition");
	nrOfFailedTestCases += ReportTestResult(ValidateAddition<6, 3>("Posit<6,3> addition failed: ", bReportIndividualTestCases), "posit<6,3>", "addition");
	return 0;

#else

	cout << "Posit conversion validation" << endl;

	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 3, 0>(tag, bReportIndividualTestCases), "posit<3,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 4, 0>(tag, bReportIndividualTestCases), "posit<4,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 5, 0>(tag, bReportIndividualTestCases), "posit<5,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 6, 0>(tag, bReportIndividualTestCases), "posit<6,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 7, 0>(tag, bReportIndividualTestCases), "posit<7,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 8, 0>(tag, bReportIndividualTestCases), "posit<8,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<10, 0>(tag, bReportIndividualTestCases), "posit<10,0>", "conversion");

	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 4, 1>(tag, bReportIndividualTestCases), "posit<4,1>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 5, 1>(tag, bReportIndividualTestCases), "posit<5,1>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 6, 1>(tag, bReportIndividualTestCases), "posit<6,1>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 7, 1>(tag, bReportIndividualTestCases), "posit<7,1>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 8, 1>(tag, bReportIndividualTestCases), "posit<8,1>", "conversion");

	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 5, 2>(tag, bReportIndividualTestCases), "posit<5,2>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 6, 2>(tag, bReportIndividualTestCases), "posit<6,2>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 7, 2>(tag, bReportIndividualTestCases), "posit<7,2>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 8, 2>(tag, bReportIndividualTestCases), "posit<8,2>", "conversion");

	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 6, 3>(tag, bReportIndividualTestCases), "posit<6,3>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 7, 3>(tag, bReportIndividualTestCases), "posit<7,3>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion< 8, 3>(tag, bReportIndividualTestCases), "posit<8,3>", "conversion");


#ifdef STRESS_TEST

	nrOfFailedTestCases += ReportTestResult(ValidateConversion<16, 0>(tag, bReportIndividualTestCases), "posit<16,0>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<16, 1>(tag, bReportIndividualTestCases), "posit<16,1>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<16, 2>(tag, bReportIndividualTestCases), "posit<16,2>", "conversion");
	nrOfFailedTestCases += ReportTestResult(ValidateConversion<16, 3>(tag, bReportIndividualTestCases), "posit<16,3>", "conversion");

#endif // STRESS_TESTING


#endif // MANUAL_TESTING

	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (char* msg) {
	cerr << msg << endl;
	return EXIT_FAILURE;
}

