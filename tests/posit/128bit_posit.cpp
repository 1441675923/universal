// create_small_posit_lookup_table.cpp

#include "stdafx.h"
#include <sstream>
#include "../../posit/posit.hpp"
#include "../../posit/posit_operators.hpp"

using namespace std;

/*
Standard posits with nbits = 128 have 4 exponent bits.
*/

int main(int argc, char** argv)
{
	try {
        const size_t nbits = 128;
        const size_t es = 4;
        posit<nbits,es> p;

		cout << spec_to_string(p) << endl;
	}
	catch (char* e) {
		cerr << e << endl;
	}

	return 0;
}
