/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// Matrix4x3.h - Matrix4x3类声明
// Matrix4x3.h - Declarations for class Matrix4x3
//
// 登录gamemath.com以获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
// 更多细节，请看Matrix4x3.cpp
// For more details, see Matrix4x3.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATRIX4X3_H_INCLUDED__
#define __MATRIX4X3_H_INCLUDED__

class Vector3;
class EulerAngles;
class Quaternion;
class RotationMatrix;

//---------------------------------------------------------------------------
// Matrix4x3类
// class Matrix4x3
//
// 实现4x3转化矩阵。这个类可以表现任何仿射变化。
// Implement a 4x3 transformation matrix.  This class can represent
// any 3D affine transformation.

class Matrix4x3 {
public:

// 公有数据
// Public data

	// 这个矩阵的值。上面3x3部分基本上包括了所有线性变换，最后一行是平移部分。
	// 查看Matrix4x3.cpp来获得更多细节。
	// The values of the matrix.  Basically the upper 3x3 portion
	// contains a linear transformation, and the last row is the
	// translation portion.  See the Matrix4x3.cpp for more
	// details.

	float	m11, m12, m13;
	float	m21, m22, m23;
	float	m31, m32, m33;
	float	tx,  ty,  tz;

// 公有操作
// Public operations

	// 设为单位矩阵
	// Set to identity

	void	identity();

	// 直接使用矩阵平移部分
	// Access the translation portion of the matrix directly

	void	zeroTranslation();
	void	setTranslation(const Vector3 &d);
	void	setupTranslation(const Vector3 &d);

	// 构造矩阵来表现一个从父空间到局部空间或相反方向的特定的转换，
	// 假定本地空间是父空间的具体指定位置和朝向。
	// 这个朝向可能是被欧拉角或者旋转矩阵来指定的。
	// Setup the matrix to perform a specific transforms from parent <->
	// local space, assuming the local space is in the specified position
	// and orientation within the parent space.  The orientation may be
	// specified using either Euler angles, or a rotation matrix

	void	setupLocalToParent(const Vector3 &pos, const EulerAngles &orient);
	void	setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient);
	void	setupParentToLocal(const Vector3 &pos, const EulerAngles &orient);
	void	setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient);

	// 构建矩阵执行关于主要轴的旋转
	// Setup the matrix to perform a rotation about a cardinal axis

	void	setupRotate(int axis, float theta);

	// 构建关于任意轴的旋转矩阵。
	// Setup the matrix to perform a rotation about an arbitrary axis

	void	setupRotate(const Vector3 &axis, float theta);

	// 构建矩阵来执行旋转，给定一个四元数形式的角位移。
	// Setup the matrix to perform a rotation, given
	// the angular displacement in quaternion form

	void	fromQuaternion(const Quaternion &q);

	// 构建矩阵来执行每根轴上的缩放。使用向量形式的Vector3(k,k,k)来统一缩放k倍。
	// Setup the matrix to perform scale on each axis

	void	setupScale(const Vector3 &s);

	// 构建矩阵来执行沿着任意轴的缩放。
	// Setup the matrix to perform scale along an arbitrary axis

	void	setupScaleAlongAxis(const Vector3 &axis, float k);

	// 构建矩阵来执行切变
	// Setup the matrix to perform a shear

	void	setupShear(int axis, float s, float t);

	// 构建矩阵来执行投影到一个通过原点的平面。
	// Setup the matrix to perform a projection onto a plane passing
	// through the origin

	void	setupProject(const Vector3 &n);

	// 构建矩阵关于一个平行于基本平面的反射。
	// Setup the matrix to perform a reflection about a plane parallel
	// to a cardinal plane

	void	setupReflect(int axis, float k = 0.0f);

	// 设置矩阵执行关于任意通过原点的平面的反射。
	// Setup the matrix to perform a reflection about an arbitrary plane
	// through the origin

	void	setupReflect(const Vector3 &n);
};

// 操作*用来变换点，也用来连接矩阵。
// 从左到右相乘的顺序和变换的顺序是一样的。
// Operator* is used to transforms a point, and also concatonate matrices.
// The order of multiplications from left to right is the same as
// the order of transformations

Vector3		operator*(const Vector3 &p, const Matrix4x3 &m);
Matrix4x3	operator*(const Matrix4x3 &a, const Matrix4x3 &b);

// 操作*=使得和C++标准一致
// Operator *= for conformance to C++ standards

Vector3		&operator*=(Vector3 &p, const Matrix4x3 &m);
Matrix4x3	&operator*=(const Matrix4x3 &a, const Matrix4x3 &m);

// 计算3x3矩阵部分的行列式
// Compute the determinant of the 3x3 portion of the matrix

float	determinant(const Matrix4x3 &m);

// 计算矩阵的逆
// Compute the inverse of a matrix

Matrix4x3 inverse(const Matrix4x3 &m);

// 从矩阵中提取平移部分
// Extract the translation portion of the matrix

Vector3	getTranslation(const Matrix4x3 &m);

// 从局部坐标系->父坐标系或父坐标系->局部坐标系提取位置/方位
// Extract the position/orientation from a local->parent matrix,
// or a parent->local matrix

Vector3	getPositionFromParentToLocalMatrix(const Matrix4x3 &m);
Vector3	getPositionFromLocalToParentMatrix(const Matrix4x3 &m);

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ROTATIONMATRIX_H_INCLUDED__
