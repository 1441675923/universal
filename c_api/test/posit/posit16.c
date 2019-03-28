// posit16.c: example test of the posit API for C programs using 16-bit posits
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include <posit_c_api.h>

int main(int argc, char* argv[]) 
{
	posit16_t pa, pb, pc;
	char str[POSIT_FORMAT16_SIZE];
	bool failures = false;

	// special case values
	pa = NAR16;
	pb = ZERO16;
	pc = posit_add16(pa, pb);
	posit_format16(pc, str);
	printf("posit value = %s\n", str);

	pa = NAR16;
	pb = ZERO16;
	pc = posit_sub16(pa, pb);
	posit_format16(pc, str);
	printf("posit value = %s\n", str);

	pa = NAR16;
	pb = ZERO16;
	pc = posit_mul16(pa, pb);
	posit_format16(pc, str);
	printf("posit value = %s\n", str);

	pa = NAR16;
	pb = ZERO16;
	pc = posit_div16(pa, pb);
	posit_format16(pc, str);
	printf("posit value = %s\n", str);

	// partial state space
	int fails = 0;
	for (int a = 0; a < 256; ++a) {
		pa = (posit16_t)a;
		for (int b = 0; b < 256; ++b) {
			pb = (posit16_t)b;
			pc = posit_add16(pa, pb);

			double da, db, dref;
			da = posit_value16(pa);
			db = posit_value16(pb);
			dref = da + db;

			posit16_t pref = posit_assign16f((float)dref);
			if (pref != pc) {
				printf("FAIL: 16.1x%04xp + 16.1x%04xp produced 16.1x%04xp instead of 16.1x%04xp\n", pa, pb, pc, pref);
				++fails;
			}
		}
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
		pa = (posit16_t)a;
		for (int b = 0; b < 256; ++b) {
			pb = (posit16_t)b;
			pc = posit_sub16(pa, pb);

			double da, db, dref;
			da = posit_value16(pa);
			db = posit_value16(pb);
			dref = da - db;

			posit16_t pref = posit_assign16f((float)dref);
			if (pref != pc) {
				printf("FAIL: 16.1x%04xp - 16.1x%04xp produced 16.1x%04xp instead of 16.1x%04xp\n", pa, pb, pc, pref);
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
		pa = (posit16_t)a;
		for (int b = 0; b < 256; ++b) {
			pb = (posit16_t)b;
			pc = posit_mul16(pa, pb);

			double da, db, dref;
			da = posit_value16(pa);
			db = posit_value16(pb);
			dref = da * db;

			posit16_t pref = posit_assign16f((float)dref);
			if (pref != pc) {
				printf("FAIL: 16.1x%04xp * 16.1x%04xp produced 16.1x%04xp instead of 16.1x%04xp\n", pa, pb, pc, pref);
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
		pa = (posit16_t)a;
		for (int b = 0; b < 256; ++b) {
			pb = (posit16_t)b;
			pc = posit_div16(pa, pb);

			double da, db, dref;
			da = posit_value16(pa);
			db = posit_value16(pb);
			dref = da / db;

			posit16_t pref = posit_assign16f((float)dref);
			if (pref != pc) {
				printf("FAIL: 16.1x%04xp / 16.1x%04xp produced 16.1x%04xp instead of 16.1x%04xp\n", pa, pb, pc, pref);
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
