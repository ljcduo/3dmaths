////////////////////////////////////////////////////////////////////////////
//
// 3D��ѧ������������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// Quaternion.cpp - ��Ԫ��ʵ��
// Quaternion.cpp - Quaternion implementation
//
// ��¼gamemath.com�Ի�ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
// ����ϸ�ڲμ�11.3��
// For more details see section 11.3.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <math.h>

#include "Quaternion.h"
#include "MathUtil.h"
#include "vector3.h"
#include "EulerAngles.h"

/////////////////////////////////////////////////////////////////////////////
//
// ȫ������
// global data
//
/////////////////////////////////////////////////////////////////////////////

// ȫ�ֱ�ʶ����Ԫ����ע������û����Ԫ����Ĺ��캯������Ϊ���ǲ���Ҫ��
// The global identity quaternion.  Notice that there are no constructors
// to the Quaternion class, since we really don't need any.

const Quaternion kQuaternionIdentity = {
	1.0f, 0.0f, 0.0f, 0.0f
};

/////////////////////////////////////////////////////////////////////////////
//
// ��Ԫ����Ա��
// class Quaternion members
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Quaternion::setToRotateAboutX
// Quaternion::setToRotateAboutY
// Quaternion::setToRotateAboutZ
// Quaternion::setToRotateAboutAxis
//
// ������Ԫ����ָ������ת
// Setup the quaternion to rotate about the specified axis

void	Quaternion::setToRotateAboutX(float theta) {

	// ������
	// Compute the half angle

	float	thetaOver2 = theta * .5f;

	// ����ֵ
	// Set the values

	w = cos(thetaOver2);
	x = sin(thetaOver2);
	y = 0.0f;
	z = 0.0f;
}

void	Quaternion::setToRotateAboutY(float theta) {

	// ������
	// Compute the half angle

	float	thetaOver2 = theta * .5f;

	// ����ֵ
	// Set the values

	w = cos(thetaOver2);
	x = 0.0f;
	y = sin(thetaOver2);
	z = 0.0f;
}

void	Quaternion::setToRotateAboutZ(float theta) {

	// ������
	// Compute the half angle

	float	thetaOver2 = theta * .5f;

	// ����ֵ
	// Set the values

	w = cos(thetaOver2);
	x = 0.0f;
	y = 0.0f;
	z = sin(thetaOver2);
}

void	Quaternion::setToRotateAboutAxis(const Vector3 &axis, float theta) {

	// �����ת������ѵ�λ��
	// The axis of rotation must be normalized

	assert(fabs(vectorMag(axis) - 1.0f) < .01f);

	// �����Ǻ�����sinֵ
	// Compute the half angle and its sin

	float	thetaOver2 = theta * .5f;
	float	sinThetaOver2 = sin(thetaOver2);

	// ����ֵ
	// Set the values

	w = cos(thetaOver2);
	x = axis.x * sinThetaOver2;
	y = axis.y * sinThetaOver2;
	z = axis.z * sinThetaOver2;
}

//---------------------------------------------------------------------------
// EulerAngles::setToRotateObjectToInertial
//
// ����ŷ������ʽ�ķ�λ��������Ԫ��ִ������->������ת
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in Euler angle format
//
// �μ�10.6.5��ø�����Ϣ��
// See 10.6.5 for more information.

void	Quaternion::setToRotateObjectToInertial(const EulerAngles &orientation) {

	// �����ǵ�sin��cosֵ
	// Compute sine and cosine of the half angles

	float	sp, sb, sh;
	float	cp, cb, ch;
	sinCos(&sp, &cp, orientation.pitch * 0.5f);
	sinCos(&sb, &cb, orientation.bank * 0.5f);
	sinCos(&sh, &ch, orientation.heading * 0.5f);

	// ����ֵ
	// Compute values

	w =  ch*cp*cb + sh*sp*sb;
	x =  ch*sp*cb + sh*cp*sb;
	y = -ch*sp*sb + sh*cp*cb;
	z = -sh*sp*cb + ch*cp*sb;
}

//---------------------------------------------------------------------------
// EulerAngles::setToRotateInertialToObject
//
// ����ŷ������ʽ�ķ�λ��������Ԫ����ִ�й���->������ת
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in Euler angle format
//
// �μ�10.6.5��ø�����Ϣ��
// See 10.6.5 for more information.

void	Quaternion::setToRotateInertialToObject(const EulerAngles &orientation) {

	// �����ǵ�sin��cosֵ
	// Compute sine and cosine of the half angles

	float	sp, sb, sh;
	float	cp, cb, ch;
	sinCos(&sp, &cp, orientation.pitch * 0.5f);
	sinCos(&sb, &cb, orientation.bank * 0.5f);
	sinCos(&sh, &ch, orientation.heading * 0.5f);

	// ����ֵ
	// Compute values

	w =  ch*cp*cb + sh*sp*sb;
	x = -ch*sp*cb - sh*cp*sb;
	y =  ch*sp*sb - sh*cb*cp;
	z =  sh*sp*cb - ch*cp*sb;
}

//---------------------------------------------------------------------------
// Quaternion::operator *
//
// ��Ԫ����ˣ�����������˵�ŷ����λ�ơ���˵�˳���Ǵ����ң�
// ��Ӧŷ����λ�Ƶ�˳�����Ǳ�׼����Ĳ�֮ͬ����
// �μ�10.4.8����������ڱ����׼����Ļ���ԭ��
// Quaternion cross product, which concatonates multiple angular
// displacements.  The order of multiplication, from left to right,
// corresponds to the order that the angular displacements are
// applied.  This is backwards from the *standard* definition of
// quaternion multiplication.  See section 10.4.8 for the rationale
// behind this deviation from the standard.

Quaternion Quaternion::operator *(const Quaternion &a) const {
	Quaternion result;

	result.w = w*a.w - x*a.x - y*a.y - z*a.z;
	result.x = w*a.x + x*a.w + z*a.y - y*a.z;
	result.y = w*a.y + y*a.w + x*a.z - z*a.x;
	result.z = w*a.z + z*a.w + y*a.x - x*a.y;

	return result;
}

//---------------------------------------------------------------------------
// Quaternion::operator *=
//
// �ϲ���˺͸�ֵ����C++Լ��ϰ��һ��
// Combined cross product and assignment, as per C++ convention

Quaternion &Quaternion::operator *=(const Quaternion &a) {

	// ��˺͸�ֵ
	// Multuiply and assign

	*this = *this * a;

	// ������ֵ����
	// Return reference to l-value

	return *this;
}

//---------------------------------------------------------------------------
// Quaternion::normalize
//
// ����λ����һ����Ԫ����ע�������أ���Ԫ�����ǵ�λ���ģ�������ֵ��ȷ���ƣ���
// �μ�10.4.6����ø�����Ϣ��
// "Normalize" a quaternion.  Note that normally, quaternions
// are always normalized (within limits of numerical precision).
// See section 10.4.6 for more information.
//
// ����ṩ�ĺ�������Ҫ������㡰�����䶯�������������ܻᷢ���������Ԫ��������
// This function is provided primarily to combat floating point "error
// creep," which can occur when many successive quaternion operations
// are applied.

void	Quaternion::normalize() {

	// ������Ԫ���Ĵ�С
	// Compute magnitude of the quaternion

	float	mag = (float)sqrt(w*w + x*x + y*y + z*z);

	// ���Ƿ����ȣ���ֹ��0����
	// Check for bogus length, to protect against divide by zero

	if (mag > 0.0f) {

		// ��λ����
		// Normalize it

		float	oneOverMag = 1.0f / mag;
		w *= oneOverMag;
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;

	} else {

		// ������һ������
		// Houston, we have a problem

		assert(false);

		// ��releaseģʽ����������ĳЩ����
		// In a release build, just slam it to something

		identity();
	}
}

//---------------------------------------------------------------------------
// Quaternion::getRotationAngle
//
// ����theta����ת
// Return the rotation angle theta

float	Quaternion::getRotationAngle() const {

	// �����ǡ� ��ס w = cos(theta/2)
	// Compute the half angle.  Remember that w = cos(theta / 2)

	float thetaOver2 = safeAcos(w);

	// ������ת��
	// Return the rotation angle

	return thetaOver2 * 2.0f;
}

//---------------------------------------------------------------------------
// Quaternion::getRotationAxis
// 
// ������ת��
// Return the rotation axis

Vector3	Quaternion::getRotationAxis() const {

	// ����sin^2(theta/2)����סw=cos(theta/2)��sin^2(x)+cos^2(x)=1
	// Compute sin^2(theta/2).  Remember that w = cos(theta/2),
	// and sin^2(x) + cos^2(x) = 1

	float sinThetaOver2Sq = 1.0f - w*w;

	// ��ֹ��ֵ����ȷ
	// Protect against numerical imprecision

	if (sinThetaOver2Sq <= 0.0f) {

		// ��λ��Ԫ����������ֵ����ȷ����������������õ���������Ϊ��������Ҫ
		// Identity quaternion, or numerical imprecision.  Just
		// return any valid vector, since it doesn't matter

		return Vector3(1.0f, 0.0f, 0.0f);
	}

	// ���� 1 / sin(theta/2)
	// Compute 1 / sin(theta/2)

	float	oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

	// ������ת��
	// Return axis of rotation

	return Vector3(
		x * oneOverSinThetaOver2,
		y * oneOverSinThetaOver2,
		z * oneOverSinThetaOver2
	);
}

/////////////////////////////////////////////////////////////////////////////
//
// �ǳ�Ա����
// Nonmember functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// dotProduct
//
// ��Ԫ����ˡ�����ʹ�÷ǳ�Ա��������˴�����Ԫ�����ʽ����ϡ��Źֵ��﷨
// Quaternion dot product.  We use a nonmember function so we can
// pass quaternion expressions as operands without having "funky syntax"
//
// See 10.4.10

float dotProduct(const Quaternion &a, const Quaternion &b) {
	return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

//---------------------------------------------------------------------------
// slerp
//
// �������Բ�ֵ
// Spherical linear interpolation.
//
// See 10.4.13

Quaternion slerp(const Quaternion &q0, const Quaternion &q1, float t) {

	// ��鳬����Χ���������ҷ��ر߽��
	// Check for out-of range parameter and return edge points if so

	if (t <= 0.0f) return q0;
	if (t >= 1.0f) return q1;

	// ʹ�õ�˼���������Ԫ��֮��ĽǶȵ�cosֵ
	// Compute "cosine of angle between quaternions" using dot product

	float cosOmega = dotProduct(q0, q1);

	// ���������ˣ�ʹ��-q1��������Ԫ��q��-q������ͬ����ת��
	// �������ܲ�����ͬ�������ֵ������ʹ��q��-q��ת��ȷ�ĽǶȡ�
	// If negative dot, use -q1.  Two quaternions q and -q
	// represent the same rotation, but may produce
	// different slerp.  We chose q or -q to rotate using
	// the acute angle.

	float q1w = q1.w;
	float q1x = q1.x;
	float q1y = q1.y;
	float q1z = q1.z;
	if (cosOmega < 0.0f) {
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}

	// ����Ӧ����������λ��Ԫ�������Ե��Ӧ����<=0.0
	// We should have two unit quaternions, so dot should be <= 1.0

	assert(cosOmega < 1.1f);

	// ����岹�����������Ԫ���������ͬ��
	// Compute interpolation fraction, checking for quaternions
	// almost exactly the same

	float k0, k1;
	if (cosOmega > 0.9999f) {

		// �ǳ��ӽ� - ����ʹ�����Բ�ֵ�����Ƿ�ֹ��0����
		// Very close - just use linear interpolation,
		// which will protect againt a divide by zero

		k0 = 1.0f-t;
		k1 = t;

	} else {

		// ʹ�����ǵ�ʽsin^2(omega) + cos^2(omega) = 1����Ƕȵ�sinֵ
		// Compute the sin of the angle using the
		// trig identity sin^2(omega) + cos^2(omega) = 1

		float sinOmega = sqrt(1.0f - cosOmega*cosOmega);

		// ��sin��cos����Ƕ�
		// Compute the angle from its sin and cosine

		float omega = atan2(sinOmega, cosOmega);

		// �����ĸ�ĵ�������������ֻ����һ�γ�������
		// Compute inverse of denominator, so we only have
		// to divide once

		float oneOverSinOmega = 1.0f / sinOmega;

		// �����ֵ����
		// Compute interpolation parameters

		k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sin(t * omega) * oneOverSinOmega;
	}

	// ����
	// Interpolate

	Quaternion result;
	result.x = k0*q0.x + k1*q1x;
	result.y = k0*q0.y + k1*q1y;
	result.z = k0*q0.z + k1*q1z;
	result.w = k0*q0.w + k1*q1w;

	// ������
	// Return it

	return result;
}

//---------------------------------------------------------------------------
// conjugate
//
// ������Ԫ���Ĺ������ԭʼ��Ԫ���෴��ת����Ԫ�����μ�10.4.7
// Compute the quaternion conjugate.  This is the quaternian
// with the opposite rotation as the original quaternian.  See 10.4.7

Quaternion conjugate(const Quaternion &q) {
	Quaternion result;

	// ��ͬ��ת��
	// Same rotation amount

	result.w = q.w;

	// �෴����
	// Opposite axis of rotation

	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;

	// ������
	// Return it

	return result;
}

//---------------------------------------------------------------------------
// pow
//
// ��Ԫ������
// Quaternion exponentiation.
//
// �μ�10.4.12
// See 10.4.12

Quaternion pow(const Quaternion &q, float exponent) {

	// ����׼��Ԫ�������
	// ��ֹ�����г�0����
	// Check for the case of an identity quaternion.
	// This will protect against divide by zero

	if (fabs(q.w) > .9999f) {
		return q;
	}

	// ��ȡ���alpha(alpha=theta/2)
	// Extract the half angle alpha (alpha = theta/2)

	float	alpha = acos(q.w);

	// ������alphaֵ
	// Compute new alpha value

	float	newAlpha = alpha * exponent;

	// ������wֵ
	// Compute new w value

	Quaternion result;
	result.w = cos(newAlpha);

	// ������xyzֵ
	// Compute new xyz values

	float	mult = sin(newAlpha) / sin(alpha);
	result.x = q.x * mult;
	result.y = q.y * mult;
	result.z = q.z * mult;

	// ������
	// Return it

	return result;
}
