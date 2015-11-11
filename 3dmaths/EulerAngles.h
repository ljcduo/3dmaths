/////////////////////////////////////////////////////////////////////////////
//
// 3D��ѧ��������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.h - ŷ����������
// EulerAngles.h - Declarations for class EulerAngles
// 
// ��¼gamemath.com����ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
// ����ϸ�ڣ���μ�EulerAngles.cpp
// For more details, see EulerAngles.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EULERANGLES_H_INCLUDED__
#define __EULERANGLES_H_INCLUDED__

// ǰ������
// Forward declarations

class Quaternion;
class Matrix4x3;
class RotationMatrix;

//---------------------------------------------------------------------------
// class EulerAngles
//
// ����������heading-pitch-bankŷ��������
// This class represents a heading-pitch-bank Euler angle triple.

class EulerAngles {
public:

// ��������
// Public data

	// ֱ�ӱ�ʾ�����������������Ի�����ʽ
	// Straightforward representation.  Store the three angles, in
	// radians

	float	heading;
	float	pitch;
	float	bank;

// ���в���
// Public operations

	// Ĭ�Ϲ����������κι���
	// Default constructor does nothing

	EulerAngles() {}

	// ��������ֵ����
	// Construct from three values

	EulerAngles(float h, float p, float b) :
		heading(h), pitch(p), bank(b) {}

	// ���õ�λ��������Ϊ0��
	// Set to identity triple (all zeros)

	void	identity() { pitch = bank = heading = 0.0f; }

	// ȷ������׼��ŷ��������
	// Determine "canonical" Euler angle triple

	void	canonize();

	// ת����Ԫ����ŷ������ʽ��
	// �����Ԫ��������ȷ�ٶ�ִ�д���������ϵ����������ϵ���ǹ�������ϵ����������ϵ��
	// Convert the quaternion to Euler angle format.  The input quaternion
	// is assumed to perform the rotation from object-to-inertial
	// or inertial-to-object, as indicated.

	void	fromObjectToInertialQuaternion(const Quaternion &q);
	void	fromInertialToObjectQuaternion(const Quaternion &q);

	// ת�����ת������ŷ������ʽ��������������ȷ�ٶ�ִ�д���������ϵ����������ϵ���ǹ�������ϵ����������ϵ��
	// ��������ƽ�Ʋ��ֱ����ԡ��������ٶ�����������
	// Convert the transform matrix to Euler angle format.  The input
	// matrix is assumed to perform the transformation from
	// object-to-world, or world-to-object, as indicated.  The
	// translation portion of the matrix is ignored.  The
	// matrix is assumed to be orthogonal.

	void	fromObjectToWorldMatrix(const Matrix4x3 &m);
	void	fromWorldToObjectMatrix(const Matrix4x3 &m);

	// ����ת����ŷ������ʽ��ת����
	// Convert a rotation matrix to Euler Angle form.

	void	fromRotationMatrix(const RotationMatrix &m);
};

// ȫ�֡���λ��ŷ���ǳ���
// A global "identity" Euler angle constant

extern const EulerAngles kEulerAnglesIdentity;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __EULERANGLES_H_INCLUDED__
