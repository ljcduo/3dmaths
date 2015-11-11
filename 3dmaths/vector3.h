/////////////////////////////////////////////////////////////////////////////
//
// 3D��ѧ��������Ϸ��ͼ�ο���
// 3D Math Primer for Games and Graphics Development
//
// Vector3.h - 3D��������
// Vector3.h - Declarations for 3D vector class
//
// ��¼gamemath.com�Ի�ø��ļ������°汾
// Visit gamemath.com for the latest version of this file.
//
// �����ע�ͣ��뿴������
// For additional comments, see Chapter 6.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR3_H_INCLUDED__
#define __VECTOR3_H_INCLUDED__

#include <math.h>

/////////////////////////////////////////////////////////////////////////////
//
// Vector3�� - һ���򵥵�3D������
// class Vector3 - a simple 3D vector class
//
/////////////////////////////////////////////////////////////////////////////

class Vector3 {
public:

// ���г��֣�����û��̫��ѡ��
// Public representation:  Not many options here.

	float x,y,z;

// ���캯��
// Constructors

	// Ĭ�Ϲ��캯�����������ֲ�ȷ����״̬
	// Default constructor leaves vector in
	// an indeterminate state

	Vector3() {}

	// �������캯��
	// Copy constructor

	Vector3(const Vector3 &a) : x(a.x), y(a.y), z(a.z) {}

	// ��������ֵ�Ĺ��캯��
	// Construct given three values

	Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

// ��׼�������
// Standard object maintenance

	// ��ֵ����������C���Թ������ҷ�����ֵ���á�
	// Assignment.  We adhere to C convention and
	// return reference to the lvalue

	Vector3 &operator =(const Vector3 &a) {
		x = a.x; y = a.y; z = a.z;
		return *this;
	}

	// ������
	// Check for equality

	bool operator ==(const Vector3 &a) const {
		return x==a.x && y==a.y && z==a.z;
	}

	bool operator !=(const Vector3 &a) const {
		return x!=a.x || y!=a.y || z!=a.z;
	}


// ��������
// Vector operations

	// ��������Ϊ0����
	// Set the vector to zero

	void zero() { x = y = z = 0.0f; }

	// һԪ��������ŷ��������ĸ�ֵ��
	// Unary minus returns the negative of the vector

	Vector3 operator -() const { return Vector3(-x,-y,-z); }

	// ��Ԫ������Ӻźͼ��ţ������ļӼ���
	// Binary + and - add and subtract vectors

	Vector3 operator +(const Vector3 &a) const {
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator -(const Vector3 &a) const {
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	// �����ĳ˷��ͳ���
	// Multiplication and division by scalar

	Vector3 operator *(float a) const {
		return Vector3(x*a, y*a, z*a);
	}

	Vector3 operator /(float a) const {
									 // ע�⣺����û�м���ĸ�Ƿ�Ϊ0
		float	oneOverA = 1.0f / a; // NOTE: no check for divide by zero here 
		return Vector3(x*oneOverA, y*oneOverA, z*oneOverA);
	}

	// �ϲ���ֵ������ʹ�ú�C���Է���Լ��һ��
	// Combined assignment operators to conform to
	// C notation convention

	Vector3 &operator +=(const Vector3 &a) {
		x += a.x; y += a.y; z += a.z;
		return *this;
	}

	Vector3 &operator -=(const Vector3 &a) {
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}

	Vector3 &operator *=(float a) {
		x *= a; y *= a; z *= a;
		return *this;
	}

	Vector3 &operator /=(float a) {
		float	oneOverA = 1.0f / a;
		x *= oneOverA; y *= oneOverA; z *= oneOverA;
		return *this;
	}

	// ������λ��
	// Normalize the vector

	void	normalize() {
		float magSq = x*x + y*y + z*z;
							// ������0�����
		if (magSq > 0.0f) { // check for divide-by-zero
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	// ������ˡ��������������׼�˷�������������¡�
	// Vector dot product.  We overload the standard
	// multiplication symbol to do this

	float operator *(const Vector3 &a) const {
		return x*a.x + y*a.y + z*a.z;
	}
};

/////////////////////////////////////////////////////////////////////////////
//
// �ǳ�Ա����
// Nonmember functions
//
/////////////////////////////////////////////////////////////////////////////

// ���������Ĵ�С
// Compute the magnitude of a vector

inline float vectorMag(const Vector3 &a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

// �������������Ĳ��
// Compute the cross product of two vectors

inline Vector3 crossProduct(const Vector3 &a, const Vector3 &b) {
	return Vector3(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	);
}

// �����ҳˣ�����һ����
// Scalar on the left multiplication, for symmetry

inline Vector3 operator *(float k, const Vector3 &v) {
	return Vector3(k*v.x, k*v.y, k*v.z);
}

// ���������ľ���
// Compute the distance between two points

inline float distance(const Vector3 &a, const Vector3 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

// ���������ľ����ƽ�������ȽϾ���ʱ�����Ƿǳ����õģ���Ϊƽ�������������
// Compute the distance between two points, squared.  Often useful
// when comparing distances, since the square root is slow

inline float distanceSquared(const Vector3 &a, const Vector3 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return dx*dx + dy*dy + dz*dz;
}

/////////////////////////////////////////////////////////////////////////////
//
// ȫ�ֱ���
// Global variables
//
/////////////////////////////////////////////////////////////////////////////

// �����ṩȫ��������
// We provide a global zero vector constant

extern const Vector3 kZeroVector;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __VECTOR3_H_INCLUDED__
