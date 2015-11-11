/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.cpp - 欧拉角类实现
// EulerAngles.cpp - Implementation of class EulerAngles
//
// 登录gamemath.com来获得该文件的最新版本
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
// 注意：
// Notes:
//
// 参见第11章来获得关于这个类的更多设计决策。
// See Chapter 11 for more information on class design decisions.
//
// 参见10.3获得欧拉角约定的更多信息。
// See section 10.3 for more information on the Euler angle conventions
// assumed.
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// 全局数据
// global data
//
/////////////////////////////////////////////////////////////////////////////

// 全局“单位”欧拉角常量。现在我们不能正确地知道这个物体可能被构建和其他物体的关系，
// 所以这个物体很有可能在初始化之前被参考。然而，在大部分的实现，在程序开始的时候是非零初始化，
// 在任何物体被构建之前。
// The global "identity" Euler angle constant.  Now we may not know exactly
// when this object may get constructed, in relation to other objects, so
// it is possible for the object to be referenced before it is initialized.
// However, on most implementations, it will be zero-initialized at program
// startup anyway, before any other objects are constructed.

const EulerAngles kEulerAnglesIdentity(0.0f, 0.0f, 0.0f);

/////////////////////////////////////////////////////////////////////////////
//
// 欧拉角类实现
// class EulerAngles Implementation
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EulerAngles::canonize
//
// 设置欧拉角三数为“标准”值。这不会改变欧拉角呈现的3D方位，
//但是如果角度是出于其他目的（例如角速度等），这个操作是不可用的。
// Set the Euler angle triple to its "canonical" value.  This does not change
// the meaning of the Euler angles as a representation of Orientation in 3D,
// but if the angles are for other purposes such as angular velocities, etc,
// then the operation might not be valid.
//
// 参见10.3节来获得更多信息。
// See section 10.3 for more information.

void	EulerAngles::canonize() {

	// 首先，回绕pitch到-pi到pi
	// First, wrap pitch in range -pi ... pi

	pitch = wrapPi(pitch);

	// 现在，检查矩阵的“后面”，pitch超出标准范围-pi/2到pi/2
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

	// 好了，现在检查万向锁情况（需要一点容忍度）
	// OK, now check for the gimbel lock case (within a slight
	// tolerance)

	if (fabs(pitch) > kPiOver2 - 1e-4) {

		// 我们遇到了万向锁。将所有竖直轴的旋转交给heading
		// We are in gimbel lock.  Assign all rotation
		// about the vertical axis to heading

		heading += bank;
		bank = 0.0f;

	} else {

		// 没有遇到万向锁。回绕到标准范围的bank角度。
		// Not in gimbel lock.  Wrap the bank angle in
		// canonical range 

		bank = wrapPi(bank);
	}

	// 将heading回绕到标准范围
	// Wrap heading in canonical range

	heading = wrapPi(heading);
}

//---------------------------------------------------------------------------
// EulerAngles::fromObjectToInertialQuaternion
//
// 构建欧拉角，给定物体坐标系->惯性坐标系旋转四元数
// Setup the Euler angles, given an object->inertial rotation quaternion
//
// 参见10.6.6来获得更多信息。
// See 10.6.6 for more information.

void	EulerAngles::fromObjectToInertialQuaternion(const Quaternion &q) {

	// 提取sin(pitch)
	// Extract sin(pitch)

	float sp = -2.0f * (q.y*q.z - q.w*q.x);

	// 检查万向锁，需要容忍一些数值上的不精确
	// Check for Gimbel lock, giving slight tolerance for numerical imprecision

	if (fabs(sp) > 0.9999f) {

		// 直接设为垂直向上或垂直向下
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// 计算heading，将bank置为0
		// Compute heading, slam bank to zero

		heading = atan2(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		bank = 0.0f;

	} else {

		// 计算角度。我们不需要使用“安全”asin函数，因为但我们检查万向锁时，已经检查范围错误
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
// 构建欧拉角，从惯性坐标系->物体坐标系旋转四元数
// Setup the Euler angles, given an inertial->object rotation quaternion
//
// 参见10.6.6节来获得更多信息。
// See 10.6.6 for more information.

void	EulerAngles::fromInertialToObjectQuaternion(const Quaternion &q) {

	// 提取sin(pitch)
	// Extract sin(pitch)

	float sp = -2.0f * (q.y*q.z + q.w*q.x);

	// 检查万向锁，需要容忍一些数值上的不精确
	// Check for Gimbel lock, giving slight tolerance for numerical imprecision

	if (fabs(sp) > 0.9999f) {

		// 直接设为垂直向上或垂直向下
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// 计算heading，将bank置为0
		// Compute heading, slam bank to zero

		heading = atan2(-q.x*q.z - q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		bank = 0.0f;

	} else {

		// 计算角度。我们不需要使用“安全”asin函数，因为但我们检查万向锁时，已经检查范围错误
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
// 构建欧拉角，给定物体坐标系->世界坐标系矩阵。
// Setup the Euler angles, given an object->world transformation matrix.
//
// 这个矩阵假定是正交的。平移部分被忽略。
// The matrix is assumed to be orthogonal.  The translation portion is
// ignored.
//
// See 10.6.2 for more information.

void	EulerAngles::fromObjectToWorldMatrix(const Matrix4x3 &m) {

	// 从m32提取sin(pitch)
	// Extract sin(pitch) from m32.

	float	sp = -m.m32;

	// 检查万向锁
	// Check for Gimbel lock
	
	if (fabs(sp) > 9.99999f) {

		// 直接设为垂直向上或垂直向下
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// 计算heading，将bank置为0
		// Compute heading, slam bank to zero

		heading = atan2(-m.m23, m.m11);
		bank = 0.0f;

	} else {

		// 计算角度。我们不需要使用“安全”asin函数，因为但我们检查万向锁时，已经检查范围错误
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
// 构建欧拉角，给定世界坐标系->物体坐标系变换矩阵。
// Setup the Euler angles, given a world->object transformation matrix.
//
// 这个矩阵假定是正交的。这个平移部分会被忽略。
// The matrix is assumed to be orthogonal.  The translation portion is
// ignored.
//
// 参见10.6.2来获得更多信息。
// See 10.6.2 for more information.

void	EulerAngles::fromWorldToObjectMatrix(const Matrix4x3 &m) {

	// 从m23提取sin(pitch)
	// Extract sin(pitch) from m23.

	float	sp = -m.m23;

	// 检查万向锁
	// Check for Gimbel lock
	
	if (fabs(sp) > 9.99999f) {

		// 直接设为垂直向上或垂直向下
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// 计算heading，将bank置为0
		// Compute heading, slam bank to zero

		heading = atan2(-m.m31, m.m11);
		bank = 0.0f;

	} else {
		
		// 计算角度。我们不需要使用“安全”asin函数，因为但我们检查万向锁时，已经检查范围错误
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
// 构建欧拉角，给定一个旋转矩阵。
// Setup the Euler angles, given a rotation matrix.
//
// 参见10.6.2来获得更多信息。
// See 10.6.2 for more information.

void	EulerAngles::fromRotationMatrix(const RotationMatrix &m) {

	// 从m23提取sin(pitch)
	// Extract sin(pitch) from m23.

	float	sp = -m.m23;

	// 检查万向锁
	// Check for Gimbel lock
	
	if (fabs(sp) > 9.99999f) {

		// 直接设为垂直向上或垂直向下
		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// 计算heading，将bank置为0
		// Compute heading, slam bank to zero

		heading = atan2(-m.m31, m.m11);
		bank = 0.0f;

	} else {

		// 计算角度。我们不需要使用“安全”asin函数，因为但我们检查万向锁时，已经检查范围错误
		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel lock

		heading = atan2(m.m13, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m21, m.m22);
	}
}
