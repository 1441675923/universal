// posit_8b.cpp: performance characterization of standard posit<8,0> configuration
//
// Copyright (C) 2017-2018 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "common.hpp"
// enable posit arithmetic exceptions
#define POSIT_THROW_ARITHMETIC_EXCEPTION 1
//#define POSIT_VERBOSE_OUTPUT
#define POSIT_TRACE_DEBUG
#define POSIT_TRACE_MUL
#include <posit>

#include "posit_performance.hpp"

int main(int argc, char** argv)
try {
	using namespace std;
	using namespace sw::unum;

	constexpr size_t nbits = 16;
	constexpr size_t es = 1;

	OperatorPerformance perfReport;
	GeneratePerformanceReport<nbits, es>(perfReport);
	ReportPerformance<nbits, es>(cout, "posit<16,1>", perfReport);

	return EXIT_SUCCESS;
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const std::runtime_error& err) {
	std::cerr << "Uncaught arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}
