/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// Vector3.h - 3D向量声明
// Vector3.h - Declarations for 3D vector class
//
// 登录gamemath.com以获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
// 额外的注释，请看第六章
// For additional comments, see Chapter 6.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR3_H_INCLUDED__
#define __VECTOR3_H_INCLUDED__

#include <math.h>

/////////////////////////////////////////////////////////////////////////////
//
// Vector3类 - 一个简单的3D向量类
// class Vector3 - a simple 3D vector class
//
/////////////////////////////////////////////////////////////////////////////

class Vector3 {
public:

// 公有呈现：这里没有太多选择。
// Public representation:  Not many options here.

	float x,y,z;

// 构造函数
// Constructors

	// 默认构造函数让向量保持不确定的状态
	// Default constructor leaves vector in
	// an indeterminate state

	Vector3() {}

	// 拷贝构造函数
	// Copy constructor

	Vector3(const Vector3 &a) : x(a.x), y(a.y), z(a.z) {}

	// 给定三个值的构造函数
	// Construct given three values

	Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

// 标准对象操作
// Standard object maintenance

	// 赋值。我们遵守C语言惯例并且返回左值引用。
	// Assignment.  We adhere to C convention and
	// return reference to the lvalue

	Vector3 &operator =(const Vector3 &a) {
		x = a.x; y = a.y; z = a.z;
		return *this;
	}

	// 检查相等
	// Check for equality

	bool operator ==(const Vector3 &a) const {
		return x==a.x && y==a.y && z==a.z;
	}

	bool operator !=(const Vector3 &a) const {
		return x!=a.x || y!=a.y || z!=a.z;
	}


// 向量操作
// Vector operations

	// 设置向量为0向量
	// Set the vector to zero

	void zero() { x = y = z = 0.0f; }

	// 一元运算符减号返回向量的负值。
	// Unary minus returns the negative of the vector

	Vector3 operator -() const { return Vector3(-x,-y,-z); }

	// 二元运算符加号和减号：向量的加减法
	// Binary + and - add and subtract vectors

	Vector3 operator +(const Vector3 &a) const {
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator -(const Vector3 &a) const {
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	// 标量的乘法和除法
	// Multiplication and division by scalar

	Vector3 operator *(float a) const {
		return Vector3(x*a, y*a, z*a);
	}

	Vector3 operator /(float a) const {
									 // 注意：这里没有检查分母是否为0
		float	oneOverA = 1.0f / a; // NOTE: no check for divide by zero here 
		return Vector3(x*oneOverA, y*oneOverA, z*oneOverA);
	}

	// 合并赋值操作来使得和C语言符号约定一致
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

	// 向量单位化
	// Normalize the vector

	void	normalize() {
		float magSq = x*x + y*y + z*z;
							// 检查除以0的情况
		if (magSq > 0.0f) { // check for divide-by-zero
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	// 向量点乘。我们重载这个标准乘法符号来做这件事。
	// Vector dot product.  We overload the standard
	// multiplication symbol to do this

	float operator *(const Vector3 &a) const {
		return x*a.x + y*a.y + z*a.z;
	}
};

/////////////////////////////////////////////////////////////////////////////
//
// 非成员函数
// Nonmember functions
//
/////////////////////////////////////////////////////////////////////////////

// 计算向量的大小
// Compute the magnitude of a vector

inline float vectorMag(const Vector3 &a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

// 计算两个向量的叉乘
// Compute the cross product of two vectors

inline Vector3 crossProduct(const Vector3 &a, const Vector3 &b) {
	return Vector3(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	);
}

// 标量右乘，出于一致性
// Scalar on the left multiplication, for symmetry

inline Vector3 operator *(float k, const Vector3 &v) {
	return Vector3(k*v.x, k*v.y, k*v.z);
}

// 计算两点间的距离
// Compute the distance between two points

inline float distance(const Vector3 &a, const Vector3 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

// 计算两点间的距离的平方。当比较距离时，这是非常有用的，因为平方根运算很慢。
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
// 全局变量
// Global variables
//
/////////////////////////////////////////////////////////////////////////////

// 我们提供全局零向量
// We provide a global zero vector constant

extern const Vector3 kZeroVector;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __VECTOR3_H_INCLUDED__
