/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.h - 欧拉角类声明
// EulerAngles.h - Declarations for class EulerAngles
// 
// 登录gamemath.com来获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
// 更多细节，请参见EulerAngles.cpp
// For more details, see EulerAngles.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EULERANGLES_H_INCLUDED__
#define __EULERANGLES_H_INCLUDED__

// 前向声明
// Forward declarations

class Quaternion;
class Matrix4x3;
class RotationMatrix;

//---------------------------------------------------------------------------
// class EulerAngles
//
// 这个类呈现了heading-pitch-bank欧拉角三数
// This class represents a heading-pitch-bank Euler angle triple.

class EulerAngles {
public:

// 公有数据
// Public data

	// 直接表示。储存三个度数，以弧度形式
	// Straightforward representation.  Store the three angles, in
	// radians

	float	heading;
	float	pitch;
	float	bank;

// 公有操作
// Public operations

	// 默认构造器不做任何工作
	// Default constructor does nothing

	EulerAngles() {}

	// 用三个数值构造
	// Construct from three values

	EulerAngles(float h, float p, float b) :
		heading(h), pitch(p), bank(b) {}

	// 设置单位三数（均为0）
	// Set to identity triple (all zeros)

	void	identity() { pitch = bank = heading = 0.0f; }

	// 确定“标准”欧拉角三数
	// Determine "canonical" Euler angle triple

	void	canonize();

	// 转换四元数到欧拉角形式。
	// 这个四元数输入明确假定执行从物体坐标系到惯性坐标系还是惯性坐标系到物体坐标系。
	// Convert the quaternion to Euler angle format.  The input quaternion
	// is assumed to perform the rotation from object-to-inertial
	// or inertial-to-object, as indicated.

	void	fromObjectToInertialQuaternion(const Quaternion &q);
	void	fromInertialToObjectQuaternion(const Quaternion &q);

	// 转换这个转化矩阵到欧拉角形式。这个输入矩阵明确假定执行从物体坐标系到惯性坐标系还是惯性坐标系到物体坐标系。
	// 这个矩阵的平移部分被忽略。这个矩阵假定是正交矩阵。
	// Convert the transform matrix to Euler angle format.  The input
	// matrix is assumed to perform the transformation from
	// object-to-world, or world-to-object, as indicated.  The
	// translation portion of the matrix is ignored.  The
	// matrix is assumed to be orthogonal.

	void	fromObjectToWorldMatrix(const Matrix4x3 &m);
	void	fromWorldToObjectMatrix(const Matrix4x3 &m);

	// 从旋转矩阵到欧拉角形式的转换。
	// Convert a rotation matrix to Euler Angle form.

	void	fromRotationMatrix(const RotationMatrix &m);
};

// 全局“单位”欧拉角常量
// A global "identity" Euler angle constant

extern const EulerAngles kEulerAnglesIdentity;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __EULERANGLES_H_INCLUDED__
