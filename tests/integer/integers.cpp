//  integers.cpp : test suite for abitrary precision integers
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <iostream>
#include <string>
#include <universal/integer/integer.hpp>
#include <universal/integer/numeric_limits.hpp>
// test helpers
#include "../test_helpers.hpp"

/*
   The goal of the arbitrary integers is to provide a constrained big integer type
   that enables fast computation with exceptions for overflow, so that the type
   can be used for forward error analysis studies.
*/

namespace sw {
namespace unum {

#define INTEGER_THROW_ARITHMETIC_EXCEPTION 1

#define FLOAT_TABLE_WIDTH 20
	template<size_t nbits>
	void ReportBinaryArithmeticError(std::string test_case, std::string op, const integer<nbits>& lhs, const integer<nbits>& rhs, const integer<nbits>& pref, const integer<nbits>& presult) {
		std::cerr << test_case << " "
			<< std::setprecision(20)
			<< std::setw(FLOAT_TABLE_WIDTH) << lhs
			<< " " << op << " "
			<< std::setw(FLOAT_TABLE_WIDTH) << rhs
			<< " != "
			<< std::setw(FLOAT_TABLE_WIDTH) << pref << " instead it yielded "
			<< std::setw(FLOAT_TABLE_WIDTH) << presult
			<< " " << to_binary(pref) << " vs " << to_binary(presult)
			<< std::setprecision(5)
			<< std::endl;
	}

	// enumerate all addition cases for an integer<nbits> configuration
	template<size_t nbits>
	int VerifyAddition(std::string tag, bool bReportIndividualTestCases) {
		constexpr size_t NR_INTEGERS = (size_t(1) << nbits);
		int nrOfFailedTests = 0;
		integer<nbits> ia, ib, isum, iref;

		int64_t i64a, i64b;
		for (size_t i = 0; i < NR_INTEGERS; i++) {
			ia.set_raw_bits(i);
			i64a = int64_t(ia);
			for (size_t j = 0; j < NR_INTEGERS; j++) {
				ib.set_raw_bits(j);
				i64b = int64_t(ib);
				iref = i64a + i64b;
#if INTEGER_THROW_ARITHMETIC_EXCEPTION
				try {
					isum = ia + ib;
				}
				catch (...) {
					if (iref > max_int<nbits>() || iref < min_int<nbits>()) {
						// correctly caught the exception
	
					}
					else {
						throw "unknown exception";
					}
				}

#else
				isum = ia + ib;
#endif
				if (isum != iref) {
					nrOfFailedTests++;
					if (bReportIndividualTestCases)	ReportBinaryArithmeticError("FAIL", "+", ia, ib, iref, isum);
				}
				else {
					//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "+", ia, ib, iref, isum);
				}
			}
		}

		return nrOfFailedTests;
	}
}
}


#define MANUAL_TESTING 1
#define STRESS_TESTING 0

int main()
try {
	using namespace std;
	using namespace sw::unum;


	std::string tag = "Integer Arithmetic tests failed";

#if MANUAL_TESTING
	using int8 = integer<8>;
	using int64 = integer<64>;
	using int128 = integer<128>;

	int8 a;
	int64 k;
	int128 m;
	cout << "Nr of bytes\n";
	cout << typeid(a).name() << "  size in bytes " << a.nrBytes << endl;
	cout << typeid(k).name() << "  size in bytes " << k.nrBytes << endl;
	cout << typeid(m).name() << "  size in bytes " << m.nrBytes << endl;

	integer<128> i1, i2, i3;
	i1 = 0; 
	cout << i1 << endl;

	i1 = 123456789;
//	i2 = 1.23456789e8;
//	i3.parse("123456789");

	cout << i1 << endl;

	return EXIT_SUCCESS;
#else
	std::cout << "Integer Arithmetic verfication" << std::endl;

	bool bReportIndividualTestCases = false;
	int nrOfFailedTestCases = 0;

	// allocation is the only functionality of integer<N> at this time

	// TODO: implement parsing, assigment, conversion, arithmetic
	// manual exhaustive test
	nrOfFailedTestCases += ReportTestResult(VerifyAddition<8>("Manual Testing", true), "integer<8>", "addition");

#ifdef STRESS_TESTING


#endif // STRESS_TESTING
	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);

#endif // MANUAL_TESTING
}
catch (char const* msg) {
	std::cerr << msg << '\n';
	return EXIT_FAILURE;
}
catch (const std::runtime_error& err) {
	std::cerr << "Uncaught runtime exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << '\n';
	return EXIT_FAILURE;
}
