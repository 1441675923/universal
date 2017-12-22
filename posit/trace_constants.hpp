#pragma once
// trace_constants.hpp: definition of constants that direct intermediate result reporting
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.


namespace sw {
namespace unum {


# ifndef POSIT_VERBOSE_OUTPUT
// posit decode and conversion
constexpr bool _trace_decode      = false;
constexpr bool _trace_conversion  = false;
constexpr bool _trace_rounding    = false;

// arithmetic operator tracing
constexpr bool _trace_add         = false;
constexpr bool _trace_sub         = false;
constexpr bool _trace_mul         = false;
constexpr bool _trace_div         = false;
constexpr bool _trace_reciprocate = false;

# else // !POSIT_VERBOSE_OUTPUT

#ifndef POSIT_TRACE_CONVERSION
// posit decode and conversion
constexpr bool _trace_decode      = false;
constexpr bool _trace_conversion  = false;
constexpr bool _trace_rounding    = false;
#else
// posit decode and conversion
constexpr bool _trace_decode      = true;
constexpr bool _trace_conversion  = true;
constexpr bool _trace_rounding    = true;
#endif // !POSIT_VERBOSE_CONVERSION

// arithmetic operator tracing
#ifndef POSIT_TRACE_ADD
constexpr bool _trace_add = false;
#else
constexpr bool _trace_add = true;
#endif

#ifndef POSIT_TRACE_SUB
constexpr bool _trace_sub = false;
#else
constexpr bool _trace_sub = true;
#endif

#ifndef POSIT_TRACE_MUL
constexpr bool _trace_mul = false;
#else
constexpr bool _trace_mul = true;
#endif

#ifndef POSIT_TRACE_DIV
constexpr bool _trace_div = false;
#else
constexpr bool _trace_div = true;
#endif

#ifndef POSIT_TRACE_RECIPROCATE
constexpr bool _trace_reciprocate = false;
#else
constexpr bool _trace_reciprocate = true;
#endif


# endif


}  // namespace unum

}  // namespace sw