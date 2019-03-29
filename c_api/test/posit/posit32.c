// posit32.c: example test of the posit API for C programs using 32-bit posits
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include <posit_c_api.h>

int main(int argc, char* argv[]) 
{
	posit32_t pa, pb, pc;
	char str[POSIT_FORMAT32_SIZE];
	bool failures = false;

	// special case values
	pa = NAR32;
	pb = ZERO32;
	pc = posit_add32(pa, pb);
	posit_format32(pc, str);
	printf("posit value = %s\n", str);

	pa = NAR32;
	pb = ZERO32;
	pc = posit_sub32(pa, pb);
	posit_format32(pc, str);
	printf("posit value = %s\n", str);

	pa = NAR32;
	pb = ZERO32;
	pc = posit_mul32(pa, pb);
	posit_format32(pc, str);
	printf("posit value = %s\n", str);

	pa = NAR32;
	pb = ZERO32;
	pc = posit_div32(pa, pb);
	posit_format32(pc, str);
	printf("posit value = %s\n", str);

	// partial state space
	int fails = 0;
	for (int a = 0; a < 256; ++a) {
		pa = (posit32_t)(a);
		for (int b = 0; b < 256; ++b) {
			pb = (posit32_t)(b);
			pc = posit_add32(pa, pb);

			double da, db, dref;
			da = posit_value32(pa);
			db = posit_value32(pb);
			dref = da + db;

			posit32_t pref = posit_assign32f(dref);
			if (pref != pc) {
				printf("FAIL: 32.2x%08lx + 32.2x%08lx produced 32.2x%08lx instead of 32.2x%08lx\n", pa, pb, pc, pref);
				++fails;
				break;
			}
		}
		if (fails) break;
	}
	if (fails) {
		printf("addition        FAIL\n");
		failures = true;
	}
	else {
		printf("addition        PASS\n");
	}

	// partial state space
	fails = 0;
	for (int a = 0; a < 256; ++a) {
		pa = (posit32_t)(a);
		for (int b = 0; b < 256; ++b) {
			pb = (posit32_t)(b);
			pc = posit_sub32(pa, pb);

			double da, db, dref;
			da = posit_value32(pa);
			db = posit_value32(pb);
			dref = da - db;

			posit32_t pref = posit_assign32f(dref);
			if (pref != pc) {
				printf("FAIL: 32.2x%08lx - 32.2x%08lx produced 32.2x%08lx instead of 32.2x%08lx\n", pa, pb, pc, pref);
				++fails;
			}
		}
	}
	if (fails) {
		printf("subtraction     FAIL\n");
		failures = true;
	}
	else {
		printf("subtraction     PASS\n");
	}

	// partial state space
	fails = 0;
	for (int a = 0; a < 256; ++a) {
		pa = (posit32_t)(a);
		for (int b = 0; b < 256; ++b) {
			pb = (posit32_t)(b);
			pc = posit_mul32(pa, pb);

			double da, db, dref;
			da = posit_value32(pa);
			db = posit_value32(pb);
			dref = da * db;

			posit32_t pref = posit_assign32f(dref);
			if (pref != pc) {
				printf("FAIL: 32.2x%08lx * 32.2x%08lx produced 32.2x%08lx instead of 32.2x%08lx\n", pa, pb, pc, pref);
				++fails;
			}
		}
	}
	if (fails) {
		printf("multiplication  FAIL\n");
		failures = true;
	}
	else {
		printf("multiplication  PASS\n");
	}

	// partial state space
	fails = 0;
	for (int a = 0; a < 256; ++a) {
		pa = (posit32_t)(a);
		for (int b = 0; b < 256; ++b) {
			pb = (posit32_t)(b);
			pc = posit_div32(pa, pb);

			double da, db, dref;
			da = posit_value32(pa);
			db = posit_value32(pb);
			dref = da / db;

			posit32_t pref = posit_assign32f(dref);
			if (pref != pc) {
				printf("FAIL: 32.2x%08lx / 32.2x%08lx produced 32.2x%08lx instead of 32.2x%08lx\n", pa, pb, pc, pref);
				++fails;
			}
		}
	}
	if (fails) {
		printf("division        FAIL\n");
		failures = true;
	}
	else {
		printf("division        PASS\n");
	}

	return failures > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
