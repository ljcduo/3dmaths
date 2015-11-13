////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// Quaternion.cpp - 四元数实现
// Quaternion.cpp - Quaternion implementation
//
// 登录gamemath.com以获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
// 更多细节参见11.3节
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
// 全局数据
// global data
//
/////////////////////////////////////////////////////////////////////////////

// 全局标识符四元数。注意这里没有四元数类的构造函数，因为我们不需要。
// The global identity quaternion.  Notice that there are no constructors
// to the Quaternion class, since we really don't need any.

const Quaternion kQuaternionIdentity = {
	1.0f, 0.0f, 0.0f, 0.0f
};

/////////////////////////////////////////////////////////////////////////////
//
// 四元数成员类
// class Quaternion members
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Quaternion::setToRotateAboutX
// Quaternion::setToRotateAboutY
// Quaternion::setToRotateAboutZ
// Quaternion::setToRotateAboutAxis
//
// 构建四元数绕指定轴旋转
// Setup the quaternion to rotate about the specified axis

void	Quaternion::setToRotateAboutX(float theta) {

	// 计算半角
	// Compute the half angle

	float	thetaOver2 = theta * .5f;

	// 设置值
	// Set the values

	w = cos(thetaOver2);
	x = sin(thetaOver2);
	y = 0.0f;
	z = 0.0f;
}

void	Quaternion::setToRotateAboutY(float theta) {

	// 计算半角
	// Compute the half angle

	float	thetaOver2 = theta * .5f;

	// 设置值
	// Set the values

	w = cos(thetaOver2);
	x = 0.0f;
	y = sin(thetaOver2);
	z = 0.0f;
}

void	Quaternion::setToRotateAboutZ(float theta) {

	// 计算半角
	// Compute the half angle

	float	thetaOver2 = theta * .5f;

	// 设置值
	// Set the values

	w = cos(thetaOver2);
	x = 0.0f;
	y = 0.0f;
	z = sin(thetaOver2);
}

void	Quaternion::setToRotateAboutAxis(const Vector3 &axis, float theta) {

	// 这个旋转轴必须已单位化
	// The axis of rotation must be normalized

	assert(fabs(vectorMag(axis) - 1.0f) < .01f);

	// 计算半角和它的sin值
	// Compute the half angle and its sin

	float	thetaOver2 = theta * .5f;
	float	sinThetaOver2 = sin(thetaOver2);

	// 设置值
	// Set the values

	w = cos(thetaOver2);
	x = axis.x * sinThetaOver2;
	y = axis.y * sinThetaOver2;
	z = axis.z * sinThetaOver2;
}

//---------------------------------------------------------------------------
// EulerAngles::setToRotateObjectToInertial
//
// 给定欧拉角形式的方位，设置四元数执行物体->惯性旋转
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in Euler angle format
//
// 参见10.6.5获得更多信息。
// See 10.6.5 for more information.

void	Quaternion::setToRotateObjectToInertial(const EulerAngles &orientation) {

	// 计算半角的sin和cos值
	// Compute sine and cosine of the half angles

	float	sp, sb, sh;
	float	cp, cb, ch;
	sinCos(&sp, &cp, orientation.pitch * 0.5f);
	sinCos(&sb, &cb, orientation.bank * 0.5f);
	sinCos(&sh, &ch, orientation.heading * 0.5f);

	// 计算值
	// Compute values

	w =  ch*cp*cb + sh*sp*sb;
	x =  ch*sp*cb + sh*cp*sb;
	y = -ch*sp*sb + sh*cp*cb;
	z = -sh*sp*cb + ch*cp*sb;
}

//---------------------------------------------------------------------------
// EulerAngles::setToRotateInertialToObject
//
// 给定欧拉角形式的方位，构建四元数来执行惯性->物体旋转
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in Euler angle format
//
// 参见10.6.5获得更多信息。
// See 10.6.5 for more information.

void	Quaternion::setToRotateInertialToObject(const EulerAngles &orientation) {

	// 计算半角的sin和cos值
	// Compute sine and cosine of the half angles

	float	sp, sb, sh;
	float	cp, cb, ch;
	sinCos(&sp, &cp, orientation.pitch * 0.5f);
	sinCos(&sb, &cb, orientation.bank * 0.5f);
	sinCos(&sh, &ch, orientation.heading * 0.5f);

	// 计算值
	// Compute values

	w =  ch*cp*cb + sh*sp*sb;
	x = -ch*sp*cb - sh*cp*sb;
	y =  ch*sp*sb - sh*cb*cp;
	z =  sh*sp*cb - ch*cp*sb;
}

//---------------------------------------------------------------------------
// Quaternion::operator *
//
// 四元数叉乘，用于连接相乘的欧拉角位移。相乘的顺序是从左到右，
// 对应欧拉角位移的顺序。这是标准定义的不同之处。
// 参见10.4.8来获得隐藏在背离标准定义的基本原理。
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
// 合并叉乘和赋值，像C++约定习俗一样
// Combined cross product and assignment, as per C++ convention

Quaternion &Quaternion::operator *=(const Quaternion &a) {

	// 相乘和赋值
	// Multuiply and assign

	*this = *this * a;

	// 返回左值引用
	// Return reference to l-value

	return *this;
}

//---------------------------------------------------------------------------
// Quaternion::normalize
//
// “单位化”一个四元数。注意正常地，四元数都是单位化的（包含数值精确限制）。
// 参见10.4.6来获得更多信息。
// "Normalize" a quaternion.  Note that normally, quaternions
// are always normalized (within limits of numerical precision).
// See section 10.4.6 for more information.
//
// 这个提供的函数首先要解决浮点“错误蠕动”，这个想象可能会发生在许多四元数操作。
// This function is provided primarily to combat floating point "error
// creep," which can occur when many successive quaternion operations
// are applied.

void	Quaternion::normalize() {

	// 计算四元数的大小
	// Compute magnitude of the quaternion

	float	mag = (float)sqrt(w*w + x*x + y*y + z*z);

	// 检查非法长度，防止除0操作
	// Check for bogus length, to protect against divide by zero

	if (mag > 0.0f) {

		// 单位化它
		// Normalize it

		float	oneOverMag = 1.0f / mag;
		w *= oneOverMag;
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;

	} else {

		// 我们有一个问题
		// Houston, we have a problem

		assert(false);

		// 在release模式，仅仅返回某些东西
		// In a release build, just slam it to something

		identity();
	}
}

//---------------------------------------------------------------------------
// Quaternion::getRotationAngle
//
// 返回theta角旋转
// Return the rotation angle theta

float	Quaternion::getRotationAngle() const {

	// 计算半角。 记住 w = cos(theta/2)
	// Compute the half angle.  Remember that w = cos(theta / 2)

	float thetaOver2 = safeAcos(w);

	// 返回旋转角
	// Return the rotation angle

	return thetaOver2 * 2.0f;
}

//---------------------------------------------------------------------------
// Quaternion::getRotationAxis
// 
// 返回旋转轴
// Return the rotation axis

Vector3	Quaternion::getRotationAxis() const {

	// 计算sin^2(theta/2)。记住w=cos(theta/2)和sin^2(x)+cos^2(x)=1
	// Compute sin^2(theta/2).  Remember that w = cos(theta/2),
	// and sin^2(x) + cos^2(x) = 1

	float sinThetaOver2Sq = 1.0f - w*w;

	// 防止数值不精确
	// Protect against numerical imprecision

	if (sinThetaOver2Sq <= 0.0f) {

		// 单位四元数，或者数值不精确。仅仅返回任意可用的向量，因为它并不重要
		// Identity quaternion, or numerical imprecision.  Just
		// return any valid vector, since it doesn't matter

		return Vector3(1.0f, 0.0f, 0.0f);
	}

	// 计算 1 / sin(theta/2)
	// Compute 1 / sin(theta/2)

	float	oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

	// 返回旋转轴
	// Return axis of rotation

	return Vector3(
		x * oneOverSinThetaOver2,
		y * oneOverSinThetaOver2,
		z * oneOverSinThetaOver2
	);
}

/////////////////////////////////////////////////////////////////////////////
//
// 非成员函数
// Nonmember functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// dotProduct
//
// 四元数点乘。我们使用非成员函数，因此传递四元数表达式不用稀奇古怪的语法
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
// 球面线性差值
// Spherical linear interpolation.
//
// See 10.4.13

Quaternion slerp(const Quaternion &q0, const Quaternion &q1, float t) {

	// 检查超出范围参数，并且返回边界点
	// Check for out-of range parameter and return edge points if so

	if (t <= 0.0f) return q0;
	if (t >= 1.0f) return q1;

	// 使用点乘计算任意四元数之间的角度的cos值
	// Compute "cosine of angle between quaternions" using dot product

	float cosOmega = dotProduct(q0, q1);

	// 如果负数点乘，使用-q1。两个四元数q和-q代表相同的旋转，
	// 不过可能产生不同的球面差值。我们使用q或-q旋转精确的角度。
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

	// 我们应该有两个单位四元数，所以点乘应该是<=0.0
	// We should have two unit quaternions, so dot should be <= 1.0

	assert(cosOmega < 1.1f);

	// 计算插补分数，检查四元数差不多是相同的
	// Compute interpolation fraction, checking for quaternions
	// almost exactly the same

	float k0, k1;
	if (cosOmega > 0.9999f) {

		// 非常接近 - 仅仅使用线性差值，我们防止除0操作
		// Very close - just use linear interpolation,
		// which will protect againt a divide by zero

		k0 = 1.0f-t;
		k1 = t;

	} else {

		// 使用三角等式sin^2(omega) + cos^2(omega) = 1计算角度的sin值
		// Compute the sin of the angle using the
		// trig identity sin^2(omega) + cos^2(omega) = 1

		float sinOmega = sqrt(1.0f - cosOmega*cosOmega);

		// 从sin和cos计算角度
		// Compute the angle from its sin and cosine

		float omega = atan2(sinOmega, cosOmega);

		// 计算分母的倒数，所以我们只用做一次除法操作
		// Compute inverse of denominator, so we only have
		// to divide once

		float oneOverSinOmega = 1.0f / sinOmega;

		// 计算差值参数
		// Compute interpolation parameters

		k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sin(t * omega) * oneOverSinOmega;
	}

	// 插入
	// Interpolate

	Quaternion result;
	result.x = k0*q0.x + k1*q1x;
	result.y = k0*q0.y + k1*q1y;
	result.z = k0*q0.z + k1*q1z;
	result.w = k0*q0.w + k1*q1w;

	// 返回它
	// Return it

	return result;
}

//---------------------------------------------------------------------------
// conjugate
//
// 计算四元数的共轭。这是原始四元数相反旋转的四元数。参见10.4.7
// Compute the quaternion conjugate.  This is the quaternian
// with the opposite rotation as the original quaternian.  See 10.4.7

Quaternion conjugate(const Quaternion &q) {
	Quaternion result;

	// 相同旋转数
	// Same rotation amount

	result.w = q.w;

	// 相反轴向
	// Opposite axis of rotation

	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;

	// 返回它
	// Return it

	return result;
}

//---------------------------------------------------------------------------
// pow
//
// 四元数的幂
// Quaternion exponentiation.
//
// 参见10.4.12
// See 10.4.12

Quaternion pow(const Quaternion &q, float exponent) {

	// 检查标准四元数的情况
	// 防止它进行除0操作
	// Check for the case of an identity quaternion.
	// This will protect against divide by zero

	if (fabs(q.w) > .9999f) {
		return q;
	}

	// 提取半角alpha(alpha=theta/2)
	// Extract the half angle alpha (alpha = theta/2)

	float	alpha = acos(q.w);

	// 计算新alpha值
	// Compute new alpha value

	float	newAlpha = alpha * exponent;

	// 计算新w值
	// Compute new w value

	Quaternion result;
	result.w = cos(newAlpha);

	// 计算新xyz值
	// Compute new xyz values

	float	mult = sin(newAlpha) / sin(alpha);
	result.x = q.x * mult;
	result.y = q.y * mult;
	result.z = q.z * mult;

	// 返回它
	// Return it

	return result;
}
