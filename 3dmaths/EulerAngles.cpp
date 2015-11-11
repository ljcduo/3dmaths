/////////////////////////////////////////////////////////////////////////////
//
// 3D��ѧ��������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.cpp - ŷ������ʵ��
// EulerAngles.cpp - Implementation of class EulerAngles
//
// ��¼gamemath.com����ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "EulerAngles.h"
#include "Quaternion.h"
#include "MathUtil.h"
#include "Matrix4x3.h"
#include "RotationMatrix.h"

/////////////////////////////////////////////////////////////////////////////
//
// ע�⣺
// Notes:
//
// �μ���11������ù��������ĸ�����ƾ��ߡ�
// See Chapter 11 for more information on class design decisions.
//
// �μ�10.3���ŷ����Լ���ĸ�����Ϣ��
// See section 10.3 for more information on the Euler angle conventions
// assumed.
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// ȫ������
// global data
//
/////////////////////////////////////////////////////////////////////////////

// ȫ�֡���λ��ŷ���ǳ������������ǲ�����ȷ��֪�����������ܱ���������������Ĺ�ϵ��
// �������������п����ڳ�ʼ��֮ǰ���ο���Ȼ�����ڴ󲿷ֵ�ʵ�֣��ڳ���ʼ��ʱ���Ƿ����ʼ����
// ���κ����屻����֮ǰ��
// The global "identity" Euler angle constant.  Now we may not know exactly
// when this object may get constructed, in relation to other objects, so
// it is possible for the object to be referenced before it is initialized.
// However, on most implementations, it will be zero-initialized at program
// startup anyway, before any other objects are constructed.

const EulerAngles kEulerAnglesIdentity(0.0f, 0.0f, 0.0f);

/////////////////////////////////////////////////////////////////////////////
//
// ŷ������ʵ��
// class EulerAngles Implementation
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EulerAngles::canonize
//
// ����ŷ��������Ϊ����׼��ֵ���ⲻ��ı�ŷ���ǳ��ֵ�3D��λ��
//��������Ƕ��ǳ�������Ŀ�ģ�������ٶȵȣ�����������ǲ����õġ�
// Set the Euler angle triple to its "canonical" value.  This does not change
// the meaning of the Euler angles as a representation of Orientation in 3D,
// but if the angles are for other purposes such as angular velocities, etc,
// then the operation might not be valid.
//
// �μ�10.3������ø�����Ϣ��
// See section 10.3 for more information.

void	EulerAngles::canonize() {

	// ���ȣ�����pitch��-pi��pi
	// First, wrap pitch in range -pi ... pi

	pitch = wrapPi(pitch);

	// ���ڣ�������ġ����桱��pitch������׼��Χ-pi/2��pi/2
	// Now, check for "the back side" of the matrix, pitch outside
	// the canonical range of -pi/2 ... pi/2

	if (pitch < -kPiOver2) {
		pitch = -kPi - pitch;
		heading += kPi;
		bank += kPi;
	} else if (pitch > kPiOver2) {
		pitch = kPi - pitch;
		heading += kPi;
		bank += kPi;
	}

	// ���ˣ����ڼ���������������Ҫһ�����̶ȣ�
	// OK, now check for the gimbel lock case (within a slight
	// tolerance)

	if (fabs(pitch) > kPiOver2 - 1e-4) {

		// ��������������������������ֱ�����ת����heading
		// We are in gimbel lock.  Assign all rotation
		// about the vertical axis to heading

		heading += bank;
		bank = 0.0f;

	} else {

		// û�����������������Ƶ���׼��Χ��bank�Ƕȡ�
		// Not in gimbel lock.  Wrap the bank angle in
		// canonical range 

		bank = wrapPi(bank);
	}

	// ��heading���Ƶ���׼��Χ
	// Wrap heading in canonical range

	heading = wrapPi(heading);
}

//---------------------------------------------------------------------------
// EulerAngles::fromObjectToInertialQuaternion
//
// ����ŷ���ǣ�������������ϵ->��������ϵ��ת��Ԫ��
// Setup the Euler angles, given an object->inertial rotation quaternion
//
// �μ�10.6.6����ø�����Ϣ��
// See 10.6.6 for more information.

void	EulerAngles::fromObjectToInertialQuaternion(const Quaternion &q) {

	// ��ȡsin(pitch)
	// Extract sin(pitch)

	float sp = -2.0f * (q.y*q.z - q.w*q.x);

	// �������������Ҫ����һЩ��ֵ�ϵĲ���ȷ
	// Check for Gimbel lock, giving slight tolerance for numerical imprecision

	if (fabs(sp) > 0.9999f) {

		// ֱ����Ϊ��ֱ���ϻ�ֱ����
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// ����heading����bank��Ϊ0
		// Compute heading, slam bank to zero

		heading = atan2(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		bank = 0.0f;

	} else {

		// ����Ƕȡ����ǲ���Ҫʹ�á���ȫ��asin��������Ϊ�����Ǽ��������ʱ���Ѿ���鷶Χ����
		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel lock

		pitch	= asin(sp);
		heading	= atan2(q.x*q.z + q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
		bank	= atan2(q.x*q.y + q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
	}
}

//---------------------------------------------------------------------------
// EulerAngles::fromInertialToObjectQuaternion
//
// ����ŷ���ǣ��ӹ�������ϵ->��������ϵ��ת��Ԫ��
// Setup the Euler angles, given an inertial->object rotation quaternion
//
// �μ�10.6.6������ø�����Ϣ��
// See 10.6.6 for more information.

void	EulerAngles::fromInertialToObjectQuaternion(const Quaternion &q) {

	// ��ȡsin(pitch)
	// Extract sin(pitch)

	float sp = -2.0f * (q.y*q.z + q.w*q.x);

	// �������������Ҫ����һЩ��ֵ�ϵĲ���ȷ
	// Check for Gimbel lock, giving slight tolerance for numerical imprecision

	if (fabs(sp) > 0.9999f) {

		// ֱ����Ϊ��ֱ���ϻ�ֱ����
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// ����heading����bank��Ϊ0
		// Compute heading, slam bank to zero

		heading = atan2(-q.x*q.z - q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		bank = 0.0f;

	} else {

		// ����Ƕȡ����ǲ���Ҫʹ�á���ȫ��asin��������Ϊ�����Ǽ��������ʱ���Ѿ���鷶Χ����
		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel lock

		pitch	= asin(sp);
		heading	= atan2(q.x*q.z - q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
		bank	= atan2(q.x*q.y - q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
	}
}

//---------------------------------------------------------------------------
// EulerAngles::fromObjectToWorldMatrix
//
// ����ŷ���ǣ�������������ϵ->��������ϵ����
// Setup the Euler angles, given an object->world transformation matrix.
//
// �������ٶ��������ġ�ƽ�Ʋ��ֱ����ԡ�
// The matrix is assumed to be orthogonal.  The translation portion is
// ignored.
//
// See 10.6.2 for more information.

void	EulerAngles::fromObjectToWorldMatrix(const Matrix4x3 &m) {

	// ��m32��ȡsin(pitch)
	// Extract sin(pitch) from m32.

	float	sp = -m.m32;

	// ���������
	// Check for Gimbel lock
	
	if (fabs(sp) > 9.99999f) {

		// ֱ����Ϊ��ֱ���ϻ�ֱ����
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// ����heading����bank��Ϊ0
		// Compute heading, slam bank to zero

		heading = atan2(-m.m23, m.m11);
		bank = 0.0f;

	} else {

		// ����Ƕȡ����ǲ���Ҫʹ�á���ȫ��asin��������Ϊ�����Ǽ��������ʱ���Ѿ���鷶Χ����
		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel lock

		heading = atan2(m.m31, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m12, m.m22);
	}
}

//---------------------------------------------------------------------------
// EulerAngles::fromWorldToObjectMatrix
//
// ����ŷ���ǣ�������������ϵ->��������ϵ�任����
// Setup the Euler angles, given a world->object transformation matrix.
//
// �������ٶ��������ġ����ƽ�Ʋ��ֻᱻ���ԡ�
// The matrix is assumed to be orthogonal.  The translation portion is
// ignored.
//
// �μ�10.6.2����ø�����Ϣ��
// See 10.6.2 for more information.

void	EulerAngles::fromWorldToObjectMatrix(const Matrix4x3 &m) {

	// ��m23��ȡsin(pitch)
	// Extract sin(pitch) from m23.

	float	sp = -m.m23;

	// ���������
	// Check for Gimbel lock
	
	if (fabs(sp) > 9.99999f) {

		// ֱ����Ϊ��ֱ���ϻ�ֱ����
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// ����heading����bank��Ϊ0
		// Compute heading, slam bank to zero

		heading = atan2(-m.m31, m.m11);
		bank = 0.0f;

	} else {
		
		// ����Ƕȡ����ǲ���Ҫʹ�á���ȫ��asin��������Ϊ�����Ǽ��������ʱ���Ѿ���鷶Χ����
		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel lock

		heading = atan2(m.m13, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m21, m.m22);
	}
}

//---------------------------------------------------------------------------
// EulerAngles::fromRotationMatrix
//
// ����ŷ���ǣ�����һ����ת����
// Setup the Euler angles, given a rotation matrix.
//
// �μ�10.6.2����ø�����Ϣ��
// See 10.6.2 for more information.

void	EulerAngles::fromRotationMatrix(const RotationMatrix &m) {

	// ��m23��ȡsin(pitch)
	// Extract sin(pitch) from m23.

	float	sp = -m.m23;

	// ���������
	// Check for Gimbel lock
	
	if (fabs(sp) > 9.99999f) {

		// ֱ����Ϊ��ֱ���ϻ�ֱ����
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// ����heading����bank��Ϊ0
		// Compute heading, slam bank to zero

		heading = atan2(-m.m31, m.m11);
		bank = 0.0f;

	} else {

		// ����Ƕȡ����ǲ���Ҫʹ�á���ȫ��asin��������Ϊ�����Ǽ��������ʱ���Ѿ���鷶Χ����
		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel lock

		heading = atan2(m.m13, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m21, m.m22);
	}
}
