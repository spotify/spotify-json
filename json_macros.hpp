// Copyright 2014 Felix Bruns and Johan Lindström.

#pragma once

#if _MSC_VER
#define json_force_inline __forceinline
#define json_never_inline __declspec(noinline)
#elif __GNUC__
#define json_force_inline __attribute__((always_inline)) inline
#define json_never_inline __attribute__((noinline))
#else
#define json_force_inline inline
#define json_never_inline
#endif
