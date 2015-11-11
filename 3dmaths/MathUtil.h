/////////////////////////////////////////////////////////////////////////////
// 
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.h - 各种数学通用程序声明
// MathUtil.h - Declarations for miscellaneous math utilities
//
// 登录gamemath.com以获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATHUTIL_H_INCLUDED__
#define __MATHUTIL_H_INCLUDED__

#include <math.h>

// 声明pi以及一些倍数的全局常量
// Declare a global constant for pi and a few multiples.

const float kPi = 3.14159265f;
const float k2Pi = kPi * 2.0f;
const float kPiOver2 = kPi / 2.0f;
const float k1OverPi = 1.0f / kPi;
const float k1Over2Pi = 1.0f / k2Pi;
const float kPiOver180 = kPi / 180.0f;
const float k180OverPi = 180.0f / kPi;

// 使一个角度回绕到-pi到pi，通过添加2pi的乘法修正
// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi

extern float wrapPi(float radian);

// "安全"反三角函数
// "Safe" inverse trig functions

extern float safeAcos(float x);

// 在角度和弧度直接转换
// Convert between degrees and radians

inline float	degToRad(float deg) { return deg * kPiOver180; }
inline float	radToDeg(float rad) { return rad * k180OverPi; }

// 计算一个角度的sin值和cos值。在一些平台如果我们知道它需要两个值，同时计算比跟开计算要快得多。
// Compute the sin and cosine of an angle.  On some platforms, if we know
// that we need both values, it can be computed faster than computing
// the two values seperately.

inline void sinCos(float *returnSin, float *returnCos, float theta) {

	// 出于化简，我们只是使用常规的三角函数。
	// 注意某些平台可能做的更好。
	// For simplicity, we'll just use the normal trig functions.
	// Note that on some platforms we may be able to do better

	*returnSin = sin(theta);
	*returnCos = cos(theta);
}

// 下面两个函数本人暂时未学到。迟些再补充。
// Convert between "field of view" and "zoom"  See section 15.2.4.
// The FOV angle is specified in radians.

inline float	fovToZoom(float fov) { return 1.0f / tan(fov * .5f); }
inline float	zoomToFov(float zoom) { return 2.0f * atan(1.0f / zoom); }

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __MATHUTIL_H_INCLUDED__
