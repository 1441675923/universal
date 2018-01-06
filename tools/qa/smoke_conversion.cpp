// smoke_conversion.cpp: generate smoke tests for posit conversion/rounding
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "stdafx.h"
#include <chrono>
#include <ctime>

#include <posit>
#include "../tests/posit_test_helpers.hpp"

using namespace std;

/*
smoke tests focus on the boundary cases of posit arithmetic.

There are four regions where the number of exponent bits vary
 */

#include <limits>

namespace sw {
	namespace qa {

		template<size_t nbits, size_t es>
		int Compare(double input, const sw::unum::posit<nbits, es>& presult, double reference, bool bReportIndividualTestCases) {
			int fail = 0;
			double result = presult.to_double();
			if (fabs(result - reference) > 0.000000001) {
				fail++;
				if (bReportIndividualTestCases)	ReportConversionError("FAIL", "=", input, reference, presult);
			}

				//if (bReportIndividualTestCases) ReportConversionSuccess("PASS", "=", input, reference, presult);
				// report test cases: input operand -> posit bit pattern
				sw::unum::value<std::numeric_limits< double >::digits> vi(input), vr(reference);
				cout.precision(std::numeric_limits< double >::max_digits10);
				cout << input << ", " << sw::unum::to_binary(input) << ", " << components(vi) << "\n" << reference << ", " << sw::unum::to_binary(reference) << ", " << components(vr) << "," << presult.get() << endl;

			return fail;
		}


		template<size_t nbits, size_t es>
		int SmokeTestConversion(std::string tag, bool bReportIndividualTestCases) {
			//static_assert(nbits >= 16, "Use exhaustive testing for posits smaller than 16");
			static_assert(nbits < 64, "TODO: smoke test algorithm only works for nibts < 64");
			// we are going to generate a test set that consists of all edge case posit configs and their midpoints
			// we do this by enumerating a posit that is 1-bit larger than the test posit configuration

			// there are four quadrants, each with two endpoints
			// south-east  -> [minpos -   1.0)
			// north-east  -> (1.0    -   maxpos)
			// north-west  -> [-maxpos - -1.0)
			// south-west  -> (-1.0    - -minpos)

			// on each minpos/maxpos side there are 2^(es+1) patterns that carry special rounding behavior
			// es = 0:   0/minpos                            ->  2 special cases
			// es = 1:   0/minpos, 2 exponent configs        ->  4 special cases
			// es = 2:   0/minpos, 2, 4 exponent configs     ->  8 special cases
			// es = 3:   0/minpos, 2, 4, 8 exponent configs  -> 16 special cases
			// es = 4:   0/minpos, 2, 4, 8, 16 exp configs   -> 32 special cases
			// -> 2^(es+1) special cases
			//
			// plus the region around 1 that puts the most pressure on the conversion algorithm's precision
			// --1, 1, and 1++, so three extra cases per half.
			// Because we need to recognize the -minpos case, which happens to be all 1's, and is the last
			// test case in exhaustive testing, we need to have that test case end up in the last entry
			// of the test case array.
			constexpr size_t single_quadrant_cases = size_t(1) << (es + 2);
			constexpr size_t cases_around_plusminus_one = 6;
			constexpr size_t cases = cases_around_plusminus_one + 4 * single_quadrant_cases;
			const int64_t STATE_SPACE = uint64_t(1) << (nbits+1);
			const int64_t HALF = uint64_t(1) << nbits;  // <--- raw bit value of infinite for a posit<nbits+1,es>
			// generate the special patterns
			uint64_t test_patterns[cases];
			// first patterns around +/- 1
			std::bitset<nbits+1> raw_bits;
			sw::unum::posit<nbits+1, es> p;  // need to generate them in the context of the posit that is nbits+1
			// around 1.0
			p = 1.0; p--; raw_bits = p.get();
			cout << "raw bits for  1.0-eps: " << raw_bits << " ull " << raw_bits.to_ullong() << endl;
			test_patterns[0] = raw_bits.to_ullong();
			p = 1.0; raw_bits = p.get();
			cout << "raw bits for  1.00000: " << raw_bits << " ull " << raw_bits.to_ullong() << endl;
			test_patterns[1] = raw_bits.to_ullong();
			p = 1.0; p++; raw_bits = p.get(); 
			cout << "raw bits for  1.0+eps: " << raw_bits << " ull " << raw_bits.to_ullong() << endl;
			test_patterns[2] = raw_bits.to_ullong();
			// around -1.0
			p = -1.0; p--; raw_bits = p.get();
			cout << "raw bits for -1.0-eps: " << raw_bits << " ull " << raw_bits.to_ullong() << " posit : " << p << endl;
			test_patterns[3] = raw_bits.to_ullong();
			p = -1.0; raw_bits = p.get();
			cout << "raw bits for -1.00000: " << raw_bits << " ull " << raw_bits.to_ullong() << " posit : " << p << endl;
			test_patterns[4] = raw_bits.to_ullong();
			p = -1.0; p++; raw_bits = p.get();
			cout << "raw bits for -1.0+eps: " << raw_bits << " ull " << raw_bits.to_ullong() << " posit : " << p << endl;
			test_patterns[5] = raw_bits.to_ullong();

			// second are the exponentiol ranges from/to minpos/maxpos
			// south-east region
			int index = 6;
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = i;
			}
			// north-east region
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = HALF - single_quadrant_cases + i;
			}
			// north-west region
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = HALF + i;
			}
			// south-east region
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = STATE_SPACE - single_quadrant_cases + i;
			}
#if 0
			cout << "Generated test patterns" << endl;
			for (int i = 0; i < single_quadrant_cases + cases_around_plusminus_one; i++) {
				cout << "[" << setw(3) << i << "] = " << test_patterns[i] << endl;
			}
#endif
			const int64_t NR_TEST_CASES = cases_around_plusminus_one + 4*single_quadrant_cases;

			sw::unum::posit<nbits + 1, es> pref, pprev, pnext;

			// execute the test
			int nrOfFailedTests = 0;
			double minpos = sw::unum::minpos_value<nbits + 1, es>();
			double eps;
			double da, input;
			sw::unum::posit<nbits, es> pa;
			for (int64_t index = 0; index < NR_TEST_CASES; index++) {
				uint64_t i = test_patterns[index];
				pref.set_raw_bits(i);
				cout << "Test case [" << index << "] = " << i << " b" << sw::unum::to_binary(pref.get().to_ullong()) << "  >>>>>>>>>>>>>>>  Reference Seed value: " << pref << endl;

				da = pref.to_double();
				if (i == 0) {
					eps = minpos / 2.0;
				}
				else {
					eps = da > 0 ? da * 1.0e-9 : da * -1.0e-9;
				}
				if (i % 2) {
					if (i == 1) {
						// special case of projecting to +minpos
						// even the -delta goes to +minpos
						input = da - eps;
						pa = input;
						pnext.set_raw_bits(i + 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, pnext.to_double(), bReportIndividualTestCases);
						input = da + eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, pnext.to_double(), bReportIndividualTestCases);

					}
					else if (i == HALF - 1) {
						// special case of projecting to +maxpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(HALF - 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, pprev.to_double(), bReportIndividualTestCases);
					}
					else if (i == HALF + 1) {
						// special case of projecting to -maxpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(HALF + 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, pprev.to_double(), bReportIndividualTestCases);
					}
					else if (i == STATE_SPACE - 1) {
						// special case of projecting to -minpos
						// even the +delta goes to -minpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(i - 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, pprev.to_double(), bReportIndividualTestCases);
						input = da + eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, pprev.to_double(), bReportIndividualTestCases);
					}
					else {
						// for odd values, we are between posit values, so we create the round-up and round-down cases
						// round-down
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(i - 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, pprev.to_double(), bReportIndividualTestCases);
						// round-up
						input = da + eps;
						pa = input;
						pnext.set_raw_bits(i + 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, pnext.to_double(), bReportIndividualTestCases);
					}
				}
				else {
					// for the even values, we generate the round-to-actual cases
					if (i == 0) {
						// special case of projecting to +minpos
						input = da + eps;
						pa = input;
						pnext.set_raw_bits(i + 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, pnext.to_double(), bReportIndividualTestCases);
					}
					else if (i == STATE_SPACE - 2) {
						// special case of projecting to -minpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(STATE_SPACE - 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, pprev.to_double(), bReportIndividualTestCases);
					}
					else {
						// round-up
						input = da - eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, da, bReportIndividualTestCases);
						// round-down
						input = da + eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, da, bReportIndividualTestCases);
					}
				}
			}
			return nrOfFailedTests;
		}

	}
}


int main(int argc, char** argv)
try {
	typedef std::numeric_limits< double > dbl;
	cout << "double max digits " << dbl::max_digits10 << endl;

	cout << "Generating smoke tests for posit conversion/rounding" << endl;

	bool bReportIndividualTestCases = true;
	int nrOfFailedTestCases = 0;

	float upper_limit = int64_t(1) << 17;
	using namespace std::chrono;
	steady_clock::time_point t1 = steady_clock::now();
	nrOfFailedTestCases = sw::qa::SmokeTestConversion<32, 2>("smoke testing", bReportIndividualTestCases);
	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	double elapsed = time_span.count();
	std::cout << "It took " << elapsed << " seconds." << std::endl;
	std::cout << "Performance " << (uint32_t)(upper_limit / (1000 * elapsed)) << " Ksamples/s" << std::endl;
	std::cout << std::endl;

	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (char* msg) {
	cerr << msg << endl;
	return EXIT_FAILURE;
}
