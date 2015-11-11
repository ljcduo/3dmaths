/////////////////////////////////////////////////////////////////////////////
//
// 3D��ѧ��������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.cpp - ������ѧͨ�ó���
// MathUtil.cpp - Miscellaneous math utilities
//
// ��¼gamemath.com�Ի�ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "MathUtil.h"
#include "Vector3.h"

const Vector3 kZeroVector(0.0f, 0.0f, 0.0f);

//---------------------------------------------------------------------------
// ʹһ���ǶȻ��Ƶ�-pi��pi��ͨ�����2pi�ĳ˷�����
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
// ��acos(x)һ�����������x������Χ�Ļ�������н�����Ŀ���ֵ��
// �ͱ�׼c����acos(x)һ�£����ֵ����0��pi��
// Same as acos(x), but if x is out of range, it is "clamped" to the nearest
// valid value.  The value returned is in range 0...pi, the same as the
// standard C acos() function

float safeAcos(float x) {

	// �����������
	// Check limit conditions

	if (x <= -1.0f) {
		return kPi;
	}
	if (x >= 1.0f) {
		return 0.0f;
	}

	// ֵ��������Χ��-ֱ��ʹ�ñ�׼C����
	// Value is in the domain - use standard C function

	return acos(x);
}
