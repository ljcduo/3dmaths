/////////////////////////////////////////////////////////////////////////////
// 
// 3D��ѧ��������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.h - ������ѧͨ�ó�������
// MathUtil.h - Declarations for miscellaneous math utilities
//
// ��¼gamemath.com�Ի�ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATHUTIL_H_INCLUDED__
#define __MATHUTIL_H_INCLUDED__

#include <math.h>

// ����pi�Լ�һЩ������ȫ�ֳ���
// Declare a global constant for pi and a few multiples.

const float kPi = 3.14159265f;
const float k2Pi = kPi * 2.0f;
const float kPiOver2 = kPi / 2.0f;
const float k1OverPi = 1.0f / kPi;
const float k1Over2Pi = 1.0f / k2Pi;
const float kPiOver180 = kPi / 180.0f;
const float k180OverPi = 180.0f / kPi;

// ʹһ���ǶȻ��Ƶ�-pi��pi��ͨ�����2pi�ĳ˷�����
// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi

extern float wrapPi(float radian);

// "��ȫ"�����Ǻ���
// "Safe" inverse trig functions

extern float safeAcos(float x);

// �ڽǶȺͻ���ֱ��ת��
// Convert between degrees and radians

inline float	degToRad(float deg) { return deg * kPiOver180; }
inline float	radToDeg(float rad) { return rad * k180OverPi; }

// ����һ���Ƕȵ�sinֵ��cosֵ����һЩƽ̨�������֪������Ҫ����ֵ��ͬʱ����ȸ�������Ҫ��öࡣ
// Compute the sin and cosine of an angle.  On some platforms, if we know
// that we need both values, it can be computed faster than computing
// the two values seperately.

inline void sinCos(float *returnSin, float *returnCos, float theta) {

	// ���ڻ�������ֻ��ʹ�ó�������Ǻ�����
	// ע��ĳЩƽ̨�������ĸ��á�
	// For simplicity, we'll just use the normal trig functions.
	// Note that on some platforms we may be able to do better

	*returnSin = sin(theta);
	*returnCos = cos(theta);
}

// ������������������ʱδѧ������Щ�ٲ��䡣
// Convert between "field of view" and "zoom"  See section 15.2.4.
// The FOV angle is specified in radians.

inline float	fovToZoom(float fov) { return 1.0f / tan(fov * .5f); }
inline float	zoomToFov(float zoom) { return 2.0f * atan(1.0f / zoom); }

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __MATHUTIL_H_INCLUDED__
