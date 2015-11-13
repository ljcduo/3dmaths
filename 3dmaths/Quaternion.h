/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏和图形开发
// 3D Math Primer for Games and Graphics Development
//
// Quaternion.h - 四元数类声明
// Quaternion.h - Declarations for class Quaternion
//
// 访问gamemath.com来获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
// 更多信息，参见Quaternion.cpp
// For more details, see Quaternion.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __QUATERNION_H_INCLUDED__
#define __QUATERNION_H_INCLUDED__

class Vector3;
class EulerAngles;

//---------------------------------------------------------------------------
// 四元数类
// class Quaternion
//
// 实现四元数，呈现3D空间中的角位移（方位）
// Implement a quaternion, for purposes of representing an angular
// displacement (orientation) in 3D.

class Quaternion {
public:

// 公有数据
// Public data

	// 四元数的这4个值。通常地，不会直接地操作它们。
	// 然而，我们让它设为公有，因为禁止直接进入会使一些操作更复杂，
	// 诸如文件I/O操作。
	// The 4 values of the quaternion.  Normally, it will not
	// be necessary to manipulate these directly.  However,
	// we leave them public, since prohibiting direct access
	// makes some operations, such as file I/O, unnecessarily
	// complicated.

	float	w, x, y, z;

// 公有操作
// Public operations

	// 设置为单位数
	// Set to identity

	void	identity() { w = 1.0f; x = y = z = 0.0f; }

	// 构建指定轴旋转的四元数
	// Setup the quaternion to a specific rotation

	void	setToRotateAboutX(float theta);
	void	setToRotateAboutY(float theta);
	void	setToRotateAboutZ(float theta);
	void	setToRotateAboutAxis(const Vector3 &axis, float theta);

	// 给定一个欧拉角形式的方位，构建执行物体<->惯性旋转
	// Setup to perform object<->inertial rotations,
	// given orientation in Euler angle format

	void	setToRotateObjectToInertial(const EulerAngles &orientation);
	void	setToRotateInertialToObject(const EulerAngles &orientation);

	// 叉乘
	// Cross product

	Quaternion operator *(const Quaternion &a) const;

	// 相乘和赋值结合，遵循C++约定习俗
	// Multiplication with assignment, as per C++ convention

	Quaternion &operator *=(const Quaternion &a);

	// 单位化四元数。
	// Normalize the quaternion.

	void	normalize();

	// 提取和返回旋转角度和旋转轴。
	// Extract and return the rotation angle and axis.

	float	getRotationAngle() const;
	Vector3	getRotationAxis() const;
};

// 一个全局唯一标识符四元数常量
// A global "identity" quaternion constant

extern const Quaternion kQuaternionIdentity;

// 四元数点乘
// Quaternion dot product.

extern float dotProduct(const Quaternion &a, const Quaternion &b);

// 球面线性插值
// Spherical linear interpolation

extern Quaternion slerp(const Quaternion &p, const Quaternion &q, float t);

// 四元数共轭
// Quaternion conjugation

extern Quaternion conjugate(const Quaternion &q);

// 四元数的幂
// Quaternion exponentiation

extern Quaternion pow(const Quaternion &q, float exponent);

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __QUATERNION_H_INCLUDED__
