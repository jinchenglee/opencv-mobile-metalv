/**
 * Math Constants Fallback for Bare-Metal Newlib
 * 
 * Newlib on bare-metal systems may not define standard math constants
 * like M_PI, M_E, etc. This header provides them if they're missing.
 * 
 * Include this before any OpenCV headers or math operations if you
 * encounter "M_PI not declared" errors.
 */

#ifndef BAREMETAL_MATH_CONSTANTS_H
#define BAREMETAL_MATH_CONSTANTS_H

#include <math.h>

/* Define math constants if not already defined by the system */

#ifndef M_E
#define M_E        2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E    1.4426950408889634074   /* log_2 e */
#endif

#ifndef M_LOG10E
#define M_LOG10E   0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LN2
#define M_LN2      0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
#define M_LN10     2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4     0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
#define M_1_PI     0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI     0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2  0.70710678118654752440  /* 1/sqrt(2) */
#endif

/* Float versions */
#ifndef M_Ef
#define M_Ef       2.7182818284590452354f
#endif

#ifndef M_LOG2Ef
#define M_LOG2Ef   1.4426950408889634074f
#endif

#ifndef M_LOG10Ef
#define M_LOG10Ef  0.43429448190325182765f
#endif

#ifndef M_LN2f
#define M_LN2f     0.69314718055994530942f
#endif

#ifndef M_LN10f
#define M_LN10f    2.30258509299404568402f
#endif

#ifndef M_PIf
#define M_PIf      3.14159265358979323846f
#endif

#ifndef M_PI_2f
#define M_PI_2f    1.57079632679489661923f
#endif

#ifndef M_PI_4f
#define M_PI_4f    0.78539816339744830962f
#endif

#ifndef M_1_PIf
#define M_1_PIf    0.31830988618379067154f
#endif

#ifndef M_2_PIf
#define M_2_PIf    0.63661977236758134308f
#endif

#ifndef M_2_SQRTPIf
#define M_2_SQRTPIf 1.12837916709551257390f
#endif

#ifndef M_SQRT2f
#define M_SQRT2f   1.41421356237309504880f
#endif

#ifndef M_SQRT1_2f
#define M_SQRT1_2f 0.70710678118654752440f
#endif

/* Long double versions */
#ifndef M_El
#define M_El       2.7182818284590452353602874713526625L
#endif

#ifndef M_LOG2El
#define M_LOG2El   1.4426950408889634073599246810018921L
#endif

#ifndef M_LOG10El
#define M_LOG10El  0.4342944819032518276511289189166051L
#endif

#ifndef M_LN2l
#define M_LN2l     0.6931471805599453094172321214581766L
#endif

#ifndef M_LN10l
#define M_LN10l    2.3025850929940456840179914546843642L
#endif

#ifndef M_PIl
#define M_PIl      3.1415926535897932384626433832795029L
#endif

#ifndef M_PI_2l
#define M_PI_2l    1.5707963267948966192313216916397514L
#endif

#ifndef M_PI_4l
#define M_PI_4l    0.7853981633974483096156608458198757L
#endif

#ifndef M_1_PIl
#define M_1_PIl    0.3183098861837906715377675267450287L
#endif

#ifndef M_2_PIl
#define M_2_PIl    0.6366197723675813430755350534900574L
#endif

#ifndef M_2_SQRTPIl
#define M_2_SQRTPIl 1.1283791670955125738961589031215452L
#endif

#ifndef M_SQRT2l
#define M_SQRT2l   1.4142135623730950488016887242096981L
#endif

#ifndef M_SQRT1_2l
#define M_SQRT1_2l 0.7071067811865475244008443621048490L
#endif

#endif /* BAREMETAL_MATH_CONSTANTS_H */

