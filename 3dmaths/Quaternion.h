/////////////////////////////////////////////////////////////////////////////
//
// 3D��ѧ��������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// Quaternion.h - ��Ԫ��������
// Quaternion.h - Declarations for class Quaternion
//
// ����gamemath.com����ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
// ������Ϣ���μ�Quaternion.cpp
// For more details, see Quaternion.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __QUATERNION_H_INCLUDED__
#define __QUATERNION_H_INCLUDED__

class Vector3;
class EulerAngles;

//---------------------------------------------------------------------------
// ��Ԫ����
// class Quaternion
//
// ʵ����Ԫ��������3D�ռ��еĽ�λ�ƣ���λ��
// Implement a quaternion, for purposes of representing an angular
// displacement (orientation) in 3D.

class Quaternion {
public:

// ��������
// Public data

	// ��Ԫ������4��ֵ��ͨ���أ�����ֱ�ӵز������ǡ�
	// Ȼ��������������Ϊ���У���Ϊ��ֱֹ�ӽ����ʹһЩ���������ӣ�
	// �����ļ�I/O������
	// The 4 values of the quaternion.  Normally, it will not
	// be necessary to manipulate these directly.  However,
	// we leave them public, since prohibiting direct access
	// makes some operations, such as file I/O, unnecessarily
	// complicated.

	float	w, x, y, z;

// ���в���
// Public operations

	// ����Ϊ��λ��
	// Set to identity

	void	identity() { w = 1.0f; x = y = z = 0.0f; }

	// ����ָ������ת����Ԫ��
	// Setup the quaternion to a specific rotation

	void	setToRotateAboutX(float theta);
	void	setToRotateAboutY(float theta);
	void	setToRotateAboutZ(float theta);
	void	setToRotateAboutAxis(const Vector3 &axis, float theta);

	// ����һ��ŷ������ʽ�ķ�λ������ִ������<->������ת
	// Setup to perform object<->inertial rotations,
	// given orientation in Euler angle format

	void	setToRotateObjectToInertial(const EulerAngles &orientation);
	void	setToRotateInertialToObject(const EulerAngles &orientation);

	// ���
	// Cross product

	Quaternion operator *(const Quaternion &a) const;

	// ��˺͸�ֵ��ϣ���ѭC++Լ��ϰ��
	// Multiplication with assignment, as per C++ convention

	Quaternion &operator *=(const Quaternion &a);

	// ��λ����Ԫ����
	// Normalize the quaternion.

	void	normalize();

	// ��ȡ�ͷ�����ת�ǶȺ���ת�ᡣ
	// Extract and return the rotation angle and axis.

	float	getRotationAngle() const;
	Vector3	getRotationAxis() const;
};

// һ��ȫ��Ψһ��ʶ����Ԫ������
// A global "identity" quaternion constant

extern const Quaternion kQuaternionIdentity;

// ��Ԫ�����
// Quaternion dot product.

extern float dotProduct(const Quaternion &a, const Quaternion &b);

// �������Բ�ֵ
// Spherical linear interpolation

extern Quaternion slerp(const Quaternion &p, const Quaternion &q, float t);

// ��Ԫ������
// Quaternion conjugation

extern Quaternion conjugate(const Quaternion &q);

// ��Ԫ������
// Quaternion exponentiation

extern Quaternion pow(const Quaternion &q, float exponent);

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __QUATERNION_H_INCLUDED__
