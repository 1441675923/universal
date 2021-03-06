#pragma once
//  posit_test_randoms.hpp : posit verification functions based on random operand generation testing
// Needs to be included after posit type is declared.
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <vector>
#include <iostream>
#include <typeinfo>
#include <random>
#include <limits>

// include the base test helpers
#include "posit_test_helpers.hpp"

namespace sw {
	namespace unum {

		//////////////////////////////////// RANDOMIZED TEST SUITE FOR LARGE POSITS ////////////////////////

		// for testing posit configs that are nbits > 14-15, we need a more efficient approach.
		// One simple, brute force approach is to generate randoms.
		//
		// A more white box approach is to focus on the testcases 
		// where something special happens in the posit arithmetic, such as rounding,
		// or the geometric rounding and inward projections.

		// operation opcodes
		const int OPCODE_NOP   =  0;
		const int OPCODE_ADD   =  1;
		const int OPCODE_SUB   =  2;
		const int OPCODE_MUL   =  3;
		const int OPCODE_DIV   =  4;
		// elementary functions with one operand
		const int OPCODE_SQRT  =  5;
		const int OPCODE_EXP   =  6;
		const int OPCODE_EXP2  =  7;
		const int OPCODE_LOG   =  8;
		const int OPCODE_LOG2  =  9;
		const int OPCODE_LOG10 = 10;
		const int OPCODE_SIN   = 11;
		const int OPCODE_COS   = 12;
		const int OPCODE_TAN   = 13;
		const int OPCODE_ASIN  = 14;
		const int OPCODE_ACOS  = 15;
		const int OPCODE_ATAN  = 16;
		const int OPCODE_SINH  = 17;
		const int OPCODE_COSH  = 18;
		const int OPCODE_TANH  = 19;
		const int OPCODE_ASINH = 20;
		const int OPCODE_ACOSH = 21;
		const int OPCODE_ATANH = 22;
		// elementary functions with two operands
		const int OPCODE_POW   = 30;
		const int OPCODE_RAN   = 40;

		// Execute a binary operator
		template<size_t nbits, size_t es>
		void executeBinary(int opcode, double da, double db, const posit<nbits, es>& pa, const posit<nbits, es>& pb, posit<nbits, es>& preference, posit<nbits, es>& presult) {
			double reference = 0.0;
			switch (opcode) {
			case OPCODE_ADD:
				presult = pa + pb;
				reference = da + db;
				break;
			case OPCODE_SUB:
				presult = pa - pb;
				reference = da - db;
				break;
			case OPCODE_MUL:
				presult = pa * pb;
				reference = da * db;
				break;
			case OPCODE_DIV:
				presult = pa / pb;
				reference = da / db;
				break;
			case OPCODE_POW:
				presult = sw::unum::pow(pa, pb);
				reference = std::pow(da, db);
				break;
			case OPCODE_NOP:
			default:
				std::cerr << "Unsupported unary operator: operation ignored\n";
				break;
			}
			preference = reference;
		}

		// Execute a unary operator
		template<size_t nbits, size_t es>
		void executeUnary(int opcode, double da, const posit<nbits, es>& pa, posit<nbits, es>& preference, posit<nbits, es>& presult) {
			double reference = 0.0;
			switch (opcode) {
			case OPCODE_SQRT:
				presult = sw::unum::sqrt(pa);
				reference = std::sqrt(da);
				break;
			case OPCODE_EXP:
				presult = sw::unum::exp(pa);
				reference = std::exp(da);
				if (0.0 == reference) reference = double(sw::unum::minpos<nbits, es>());
				break;
			case OPCODE_EXP2:
				presult = sw::unum::exp2(pa);
				reference = std::exp2(da);
				if (0.0 == reference) reference = double(sw::unum::minpos<nbits, es>());
				break;
			case OPCODE_LOG:
				presult = sw::unum::log(pa);
				reference = std::log(da);
				break;
			case OPCODE_LOG2:
				presult = sw::unum::log2(pa);
				reference = std::log2(da);
				break;
			case OPCODE_LOG10:
				presult = sw::unum::log10(pa);
				reference = std::log10(da);
				break;
			case OPCODE_SIN:
				presult = sw::unum::sin(pa);
				reference = std::sin(da);
				break;
			case OPCODE_COS:
				presult = sw::unum::cos(pa);
				reference = std::cos(da);
				break;
			case OPCODE_TAN:
				presult = sw::unum::tan(pa);
				reference = std::tan(da);
				break;
			case OPCODE_ASIN:
				presult = sw::unum::asin(pa);
				reference = std::asin(da);
				break;
			case OPCODE_ACOS:
				presult = sw::unum::acos(pa);
				reference = std::acos(da);
				break;
			case OPCODE_ATAN:
				presult = sw::unum::atan(pa);
				reference = std::atan(da);
				break;
			case OPCODE_SINH:
				presult = sw::unum::sinh(pa);
				reference = std::sinh(da);
				break;
			case OPCODE_COSH:
				presult = sw::unum::cosh(pa);
				reference = std::cosh(da);
				break;
			case OPCODE_TANH:
				presult = sw::unum::tanh(pa);
				reference = std::tanh(da);
				break;
			case OPCODE_ASINH:
				presult = sw::unum::asinh(pa);
				reference = std::asinh(da);
				break;
			case OPCODE_ACOSH:
				presult = sw::unum::acosh(pa);
				reference = std::acosh(da);
				break;
			case OPCODE_ATANH:
				presult = sw::unum::atanh(pa);
				reference = std::atanh(da);
				break;
			case OPCODE_NOP:
			default:
				std::cerr << "Unsupported binary operator: operation ignored\n";
				break;
			}
			preference = reference;
		}

		// generate a random set of operands to test the binary operators for a posit configuration
		// Basic design is that we generate nrOfRandom posit values and store them in an operand array.
		// We will then execute the binary operator nrOfRandom combinations.
		template<size_t nbits, size_t es>
		int ValidateBinaryOperatorThroughRandoms(std::string tag, bool bReportIndividualTestCases, int opcode, uint32_t nrOfRandoms) {
			const size_t SIZE_STATE_SPACE = nrOfRandoms;
			int nrOfFailedTests = 0;
			posit<nbits, es> pa, pb, presult, preference;

			std::string operation_string;
			switch (opcode) {
			case OPCODE_ADD:
				operation_string = "+";
				break;
			case OPCODE_SUB:
				operation_string = "-";
				break;
			case OPCODE_MUL:
				operation_string = "*";
				break;
			case OPCODE_DIV:
				operation_string = "/";
				break;
			case OPCODE_POW:
				operation_string = "pow";
				break;
			case OPCODE_NOP:
			default:
				std::cerr << "Unsupported unary operator, test cancelled\n";
				break;
			}
			// generate the full state space set of valid posit values
			std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
			std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator and seed it with entropy.
									   //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
			std::uniform_int_distribution<unsigned long long> distr;
#ifdef POSIT_USE_LONG_DOUBLE
			std::vector<long double> operand_values(SIZE_STATE_SPACE);
			for (uint32_t i = 0; i < SIZE_STATE_SPACE; i++) {
				presult.set_raw_bits(distr(eng));  // take the bottom nbits bits as posit encoding
				operand_values[i] = (long double)(presult);
			}
			long double da, db;
#else // USE DOUBLE
			std::vector<double> operand_values(SIZE_STATE_SPACE);
			for (uint32_t i = 0; i < SIZE_STATE_SPACE; i++) {
				presult.set_raw_bits(distr(eng));  // take the bottom nbits bits as posit encoding
				operand_values[i] = double(presult);
			}
			double da, db;
#endif // POSIT_USE_LONG_DOUBLE
			unsigned ia, ib;  // random indices for picking operands to test
			for (unsigned i = 1; i < nrOfRandoms; i++) {
				ia = std::rand() % SIZE_STATE_SPACE;
				da = operand_values[ia];
				pa = da;
				ib = std::rand() % SIZE_STATE_SPACE;
				db = operand_values[ib];
				pb = db;
				// in case you have numeric_limits<long double>::digits trouble... this will show that
				//std::cout << "sizeof da: " << sizeof(da) << " bits in significant " << (std::numeric_limits<long double>::digits - 1) << " value da " << da << " at index " << ia << " pa " << pa << std::endl;
				//std::cout << "sizeof db: " << sizeof(db) << " bits in significant " << (std::numeric_limits<long double>::digits - 1) << " value db " << db << " at index " << ia << " pa " << pb << std::endl;
#if POSIT_THROW_ARITHMETIC_EXCEPTION
				try {
					executeBinary(opcode, da, db, pa, pb, preference, presult);
				}
				catch (const posit_arithmetic_exception& err) {
					if (pa.isnar() || pb.isnar() || (opcode == OPCODE_DIV && pb.iszero())) {
						std::cerr << "Correctly caught arithmetic exception: " << err.what() << std::endl;
					}
					else {
						throw err;
					}
				}
#else
				executeBinary(opcode, da, db, pa, pb, preference, presult);
#endif
				if (presult != preference) {
					nrOfFailedTests++;
					if (bReportIndividualTestCases) ReportBinaryArithmeticError("FAIL", operation_string, pa, pb, preference, presult);
				}
				else {
					if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", operation_string, pa, pb, preference, presult);
				}
			}

			return nrOfFailedTests;
		}

		// generate a random set of operands to test the binary operators for a posit configuration
		// Basic design is that we generate nrOfRandom posit values and store them in an operand array.
		// We will then execute the binary operator nrOfRandom combinations.
		template<size_t nbits, size_t es>
		int ValidateUnaryOperatorThroughRandoms(std::string tag, bool bReportIndividualTestCases, int opcode, uint32_t nrOfRandoms) {
			const size_t SIZE_STATE_SPACE = nrOfRandoms;
			int nrOfFailedTests = 0;
			posit<nbits, es> pa, pb, presult, preference;

			std::string operation_string;
			bool sqrtOperator = false;  // we need to filter negative values from the randoms
			switch (opcode) {
			default:
			case OPCODE_NOP:
				operation_string = "nop";
				break;
			case OPCODE_ADD:
			case OPCODE_SUB:
			case OPCODE_MUL:
			case OPCODE_DIV:
				std::cerr << "Unsupported binary operator, test cancelled\n";
				break;
			case OPCODE_SQRT:
				operation_string = "sqrt";
				sqrtOperator = true;
				break;
			case OPCODE_EXP:
				break;
			case OPCODE_EXP2:
				break;
			case OPCODE_LOG:
				break;
			case OPCODE_LOG2:
				break;
			case OPCODE_LOG10:
				break;
			case OPCODE_SIN:
				break;
			case OPCODE_COS:
				break;
			case OPCODE_TAN:
				break;
			case OPCODE_ASIN:
				break;
			case OPCODE_ACOS:
				break;
			case OPCODE_ATAN:
				break;
			case OPCODE_SINH:
				break;
			case OPCODE_COSH:
				break;
			case OPCODE_TANH:
				break;
			case OPCODE_ASINH:
				break;
			case OPCODE_ACOSH:
				break;
			case OPCODE_ATANH:
				break;
			}
			// generate the full state space set of valid posit values
			std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
			std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator and seed it with entropy.
									   //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
			std::uniform_int_distribution<unsigned long long> distr;
#ifdef POSIT_USE_LONG_DOUBLE
			std::vector<long double> operand_values(SIZE_STATE_SPACE);
			for (uint32_t i = 0; i < SIZE_STATE_SPACE; i++) {
				presult.set_raw_bits(distr(eng));  // take the bottom nbits bits as posit encoding
				operand_values[i] = (long double)(presult);
			}
			long double da, db;
#else // USE DOUBLE
			std::vector<double> operand_values(SIZE_STATE_SPACE);
			for (uint32_t i = 0; i < SIZE_STATE_SPACE; i++) {
				presult.set_raw_bits(distr(eng));  // take the bottom nbits bits as posit encoding
				if (sqrtOperator && presult.isneg()) {
					presult = -presult;
				}
				operand_values[i] = double(presult);
			}
			double da;
#endif // POSIT_USE_LONG_DOUBLE
			unsigned ia;  // random indices for picking operands to test
			for (unsigned i = 1; i < nrOfRandoms; i++) {
				ia = std::rand() % SIZE_STATE_SPACE;
				da = operand_values[ia];
				pa = da;
				// in case you have numeric_limits<long double>::digits trouble... this will show that
				//std::cout << "sizeof da: " << sizeof(da) << " bits in significant " << (std::numeric_limits<long double>::digits - 1) << " value da " << da << " at index " << ia << " pa " << pa << std::endl;
#if POSIT_THROW_ARITHMETIC_EXCEPTION
				try {
					executeUnary(opcode, da, pa, preference, presult);
				}
				catch (const posit_arithmetic_exception& err) {
					if (pa.isnar() || pb.isnar() || (opcode == OPCODE_DIV && pb.iszero())) {
						std::cerr << "Correctly caught arithmetic exception: " << err.what() << std::endl;
					}
					else {
						throw err;
					}
				}
#else
				executeUnary(opcode, da, pa, preference, presult);
#endif
				if (presult != preference) {
					nrOfFailedTests++;
					if (bReportIndividualTestCases) ReportUnaryArithmeticError("FAIL", operation_string, pa, preference, presult);
				}
				else {
					if (bReportIndividualTestCases) ReportUnaryArithmeticSuccess("PASS", operation_string, pa, preference, presult);
				}
			}

			return nrOfFailedTests;
		}

		template<size_t nbits, size_t es>
		int Compare(long double input, const posit<nbits, es>& presult, const posit<nbits, es>& ptarget, const posit<nbits+1,es>& pref, bool bReportIndividualTestCases) {
			int fail = 0;
			if (presult != ptarget) {
				fail++;
				if (bReportIndividualTestCases) {
					ReportConversionError("FAIL", "=", input, (long double)(ptarget), presult);
					std::cout << "reference   : " << pref.get() << std::endl;
					std::cout << "target bits : " << ptarget.get() << std::endl;
					std::cout << "actual bits : " << presult.get() << std::endl;
				}
			}
			else {
				// if (bReportIndividualTestCases) ReportConversionSuccess("PASS", "=", input, reference, presult);
			}
			return fail;
		}


		// generate a random set of conversion cases
		template<size_t nbits, size_t es>
		int ValidateConversionThroughRandoms(const std::string& tag, bool bReportIndividualTestCases, uint32_t nrOfRandoms) {
			// we are going to generate a test set that consists of all posit configs and their midpoints
			// we do this by enumerating a posit that is 1-bit larger than the test posit configuration
			// These larger posits will be at the mid-point between the smaller posit sample values
			// and we'll enumerate the exact value, and a perturbation smaller and a perturbation larger
			// to test the rounding logic of the conversion.
			posit<nbits + 1, es> pref, pprev, pnext;

			// setup the random number generator
			std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
			std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator and seed it with entropy.
									   //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
			std::uniform_int_distribution<unsigned long long> distr;

			// execute the test
			int nrOfFailedTests = 0;
			//long double minpos = minpos_value<nbits + 1, es>();
			long double da, input;
			posit<nbits, es> presult, ptarget;
			for (uint32_t i = 0; i < nrOfRandoms; ++i) {
				// generate random value
				unsigned long long value = distr(eng);
				pref.set_raw_bits(value);   // assign to a posit<nbits+1,es> to generate the reference we know how to perturb
				da = (long double)(pref);

				//std::cout << std::hex << "0x" << value << std::endl;
				//std::cout << std::dec << da << std::endl;
/*
				long double eps;
				if (value == 0) {
					eps = minpos / 2.0;
				}
				else {
					eps = da > 0 ? da * 1.0e-6 : da * -1.0e-6;
				}
				*/

				pprev = pnext = pref;
				--pprev;
				++pnext;
				bitblock<nbits> raw_target;
				if (value % 2) {
					// for odd values, we are between posit values, so we create the round-up and round-down cases

					// round-down case
					input = (long double)(pprev);
					presult = input;
					truncate(pprev.get(), raw_target);
					ptarget.set(raw_target);
					nrOfFailedTests += Compare(input, presult, ptarget, pref, bReportIndividualTestCases);
					// round-up
					input = (long double)(pnext);
					presult = input;
					truncate(pnext.get(), raw_target);
					ptarget.set(raw_target);
					nrOfFailedTests += Compare(input, presult, ptarget, pref, bReportIndividualTestCases);
				}
				else {
					// for even values, we are on a posit value, so we create the round-up and round-down cases
					// by perturbing negative for rounding up, and perturbing positive for rounding down
					// The problem you will run into for large posits is that you need 128-bit floats to be
					// able to make the perturbation small enough not to end up on a completely different posit.

					// round-up
					input = (long double)(pprev);
					presult = input;
					ptarget = (long double)(pref);
					//nrOfFailedTests += Compare(input, presult, ptarget, pref, bReportIndividualTestCases);
					// round-down
					input = (long double)(pnext);
					presult = input;
					ptarget = (long double)(pref);
					nrOfFailedTests += Compare(input, presult, ptarget, pref, bReportIndividualTestCases);
				}
			}
			return nrOfFailedTests;
		}

	} // namespace unum

} // namespace sw

