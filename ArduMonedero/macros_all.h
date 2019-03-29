/**
 * Some of the macros were ported from
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 */

#ifndef _MACROS_H
#define _MACROS_H

#include <Arduino.h>

/*
 * Macros to transform STATIC Serial.prints
 * Save a lot of SRAM! Must have at least 1 character
 */
#ifndef Sprint(x)
  #define Sprint(x)       (Serial.print(F(x)))
#endif
#ifndef Sprintln(x)
  #define Sprintln(x)     (Serial.println(F(x)))
#endif

#define FORCE_INLINE __attribute__((always_inline)) inline
#define _UNUSED      __attribute__((unused))
#define _O0          __attribute__((optimize("O0")))
#define _Os          __attribute__((optimize("Os")))
#define _O1          __attribute__((optimize("O1")))
#define _O2          __attribute__((optimize("O2")))
#define _O3          __attribute__((optimize("O3")))

// Clock speed factors
#define CYCLES_PER_MICROSECOND (F_CPU / 1000000L) // 16 or 20
#define INT0_PRESCALER 8

// Nanoseconds per cycle
#define NANOSECONDS_PER_CYCLE (1000000000.0 / F_CPU)

// Remove compiler warning on an unused variable
#define UNUSED(x) (void) (x)

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

#define A(CODE) " " CODE "\n\t"
#define L(CODE) CODE ":\n\t"

// Macros for bit masks
#undef _BV
#define _BV(b) (1 << (b))
#define TEST(n,b) !!((n)&_BV(b))
#define SBI(n,b) (n |= _BV(b))
#define CBI(n,b) (n &= ~_BV(b))
#define SET_BIT_TO(N,B,TF) do{ if (TF) SBI(N,B); else CBI(N,B); }while(0)

#define _BV32(b) (1UL << (b))
#define TEST32(n,b) !!((n)&_BV32(b))
#define SBI32(n,b) (n |= _BV32(b))
#define CBI32(n,b) (n &= ~_BV32(b))

// Macro to check if a number is a power of 2
#define IS_POWER_OF_2(x) ((x) && !((x) & ((x) - 1)))

// Macros for maths shortcuts
#undef M_PI
#define M_PI 3.14159265358979323846f
#define RADIANS(d) ((d)*M_PI/180.0f)
#define DEGREES(r) ((r)*180.0f/M_PI)
#define HYPOT2(x,y) (sq(x)+sq(y))

#define CIRCLE_AREA(R) (M_PI * sq(float(R)))
#define CIRCLE_CIRC(R) (2 * M_PI * (float(R)))

#define SIGN(a) ((a>0)-(a<0))
#define IS_POWER_OF_2(x) ((x) && !((x) & ((x) - 1)))

// Macros to contrain values
#define NOLESS(v,n) do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n) do{ if (v > n) v = n; }while(0)
#define LIMIT(v,n1,n2) do{ if (v < n1) v = n1; else if (v > n2) v = n2; }while(0)

// Macros to support option testing
#define _CAT(a, ...) a ## __VA_ARGS__
#define SWITCH_ENABLED_false 0
#define SWITCH_ENABLED_true  1
#define SWITCH_ENABLED_0     0
#define SWITCH_ENABLED_1     1
#define SWITCH_ENABLED_0x0   0
#define SWITCH_ENABLED_0x1   1
#define SWITCH_ENABLED_      1
#define ENABLED(b) _CAT(SWITCH_ENABLED_, b)
#define DISABLED(b) !ENABLED(b)

#define WITHIN(V,L,H) ((V) >= (L) && (V) <= (H))
#define NUMERIC(a) WITHIN(a, '0', '9')
#define DECIMAL(a) (NUMERIC(a) || a == '.')
#define NUMERIC_SIGNED(a) (NUMERIC(a) || (a) == '-' || (a) == '+')
#define DECIMAL_SIGNED(a) (DECIMAL(a) || (a) == '-' || (a) == '+')
#define COUNT(a) (sizeof(a)/sizeof(*a))
#define ZERO(a) memset(a,0,sizeof(a))
#define COPY(a,b) memcpy(a,b,MIN(sizeof(a),sizeof(b)))

// Macros for initializing arrays
#define ARRAY_6(v1, v2, v3, v4, v5, v6, ...) { v1, v2, v3, v4, v5, v6 }
#define ARRAY_5(v1, v2, v3, v4, v5, ...)     { v1, v2, v3, v4, v5 }
#define ARRAY_4(v1, v2, v3, v4, ...)         { v1, v2, v3, v4 }
#define ARRAY_3(v1, v2, v3, ...)             { v1, v2, v3 }
#define ARRAY_2(v1, v2, ...)                 { v1, v2 }
#define ARRAY_1(v1, ...)                     { v1 }

#define _ARRAY_N(N, ...) ARRAY_ ##N(__VA_ARGS__)
#define ARRAY_N(N, ...) _ARRAY_N(N, __VA_ARGS__)

// Macros for adding
#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INCREMENT_(n) INC_ ##n
#define INCREMENT(n) INCREMENT_(n)

// Macros for subtracting
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8
#define DECREMENT_(n) DEC_ ##n
#define DECREMENT(n) DECREMENT_(n)

#define PIN_EXISTS(PN) (defined(PN ##_PIN) && PN ##_PIN >= 0)

#define PENDING(NOW,SOON) ((long)(NOW-(SOON))<0)
#define ELAPSED(NOW,SOON) (!PENDING(NOW,SOON))

#define MMM_TO_MMS(MM_M) ((MM_M)/60.0f)
#define MMS_TO_MMM(MM_S) ((MM_S)*60.0f)

#define NOOP do{} while(0)

#define CEILING(x,y) (((x) + (y) - 1) / (y))

// Avoid double evaluation of arguments on MIN/MAX/ABS
#undef MIN
#undef MAX
#undef ABS
#ifdef __cplusplus

  // C++11 solution that is standards compliant. Return type is deduced automatically
  template <class L, class R> static inline constexpr auto MIN(const L lhs, const R rhs) -> decltype(lhs + rhs) {
    return lhs < rhs ? lhs : rhs;
  }
  template <class L, class R> static inline constexpr auto MAX(const L lhs, const R rhs) -> decltype(lhs + rhs){
    return lhs > rhs ? lhs : rhs;
  }
  template <class T> static inline constexpr const T ABS(const T v) {
    return v >= 0 ? v : -v;
  }
#else

  // Using GCC extensions, but Travis GCC version does not like it and gives
  //  "error: statement-expressions are not allowed outside functions nor in template-argument lists"
  #define MIN(a, b) \
    ({__typeof__(a) _a = (a); \
      __typeof__(b) _b = (b); \
      _a < _b ? _a : _b;})

  #define MAX(a, b) \
    ({__typeof__(a) _a = (a); \
      __typeof__(b) _b = (b); \
      _a > _b ? _a : _b;})

  #define ABS(a) \
    ({__typeof__(a) _a = (a); \
      _a >= 0 ? _a : -_a;})

#endif

#define MIN3(a, b, c)       MIN(MIN(a, b), c)
#define MIN4(a, b, c, d)    MIN(MIN3(a, b, c), d)
#define MIN5(a, b, c, d, e) MIN(MIN4(a, b, c, d), e)
#define MAX3(a, b, c)       MAX(MAX(a, b), c)
#define MAX4(a, b, c, d)    MAX(MAX3(a, b, c), d)
#define MAX5(a, b, c, d, e) MAX(MAX4(a, b, c, d), e)

#define UNEAR_ZERO(x) ((x) < 0.000001f)
#define NEAR_ZERO(x) WITHIN(x, -0.000001f, 0.000001f)
#define NEAR(x,y) NEAR_ZERO((x)-(y))

#define RECIPROCAL(x) (NEAR_ZERO(x) ? 0.0f : 1.0f / (x))
#define FIXFLOAT(f) (f + (f < 0.0f ? -0.00005f : 0.00005f))

//
// Maths macros that can be overridden by HAL
//
#define ATAN2(y, x) atan2f(y, x)
#define POW(x, y)   powf(x, y)
#define SQRT(x)     sqrtf(x)
#define RSQRT(x)    (1 / sqrtf(x))
#define CEIL(x)     ceilf(x)
#define FLOOR(x)    floorf(x)
#define LROUND(x)   lroundf(x)
#define FMOD(x, y)  fmodf(x, y)
#define HYPOT(x,y)  SQRT(HYPOT2(x,y))

#endif // _MACROS_H
