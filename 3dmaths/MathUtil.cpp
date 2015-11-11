/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.cpp - 各种数学通用程序
// MathUtil.cpp - Miscellaneous math utilities
//
// 登录gamemath.com以获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "MathUtil.h"
#include "Vector3.h"

const Vector3 kZeroVector(0.0f, 0.0f, 0.0f);

//---------------------------------------------------------------------------
// 使一个角度回绕到-pi到pi，通过添加2pi的乘法修正
// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi

float wrapPi(float radian) {
	radian += kPi;
	radian -= floor(radian * k1Over2Pi) * k2Pi;
	radian -= kPi;
	return radian;
}

//---------------------------------------------------------------------------
// safeAcos
//
// 和acos(x)一样，不过如果x超出范围的话，它会夹紧最近的可用值。
// 和标准c函数acos(x)一致，这个值返回0到pi。
// Same as acos(x), but if x is out of range, it is "clamped" to the nearest
// valid value.  The value returned is in range 0...pi, the same as the
// standard C acos() function

float safeAcos(float x) {

	// 检查限制条件
	// Check limit conditions

	if (x <= -1.0f) {
		return kPi;
	}
	if (x >= 1.0f) {
		return 0.0f;
	}

	// 值在作用域范围内-直接使用标准C函数
	// Value is in the domain - use standard C function

	return acos(x);
}
