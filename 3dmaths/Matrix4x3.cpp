/////////////////////////////////////////////////////////////////////////////
//
// 3D数学基础：游戏与图形开发
// 3D Math Primer for Games and Graphics Development
//
// Matrix4x3.cpp - Matrix4x3实现
// Matrix4x3.cpp - Implementation of class Matrix4x3
//
// 登录gamemath.com以获得该文件的最新版本
// Visit gamemath.com for the latest version of this file.
//
// 更多细节请看11.5节
// For more details see section 11.5.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <math.h>

#include "Vector3.h"
#include "EulerAngles.h"
#include "Quaternion.h"
#include "RotationMatrix.h"
#include "Matrix4x3.h"
#include "MathUtil.h"

/////////////////////////////////////////////////////////////////////////////
//
// 注意：
// Notes:
//
// 请看11章获得类设计决策的更多信息。
// See Chapter 11 for more information on class design decisions.
//
//---------------------------------------------------------------------------
//
// 矩阵组织
// MATRIX ORGANIZATION
//
// 这个类的目的是为了用户可能执行转换操作，而不用摆弄加号、减号或者转置矩阵直到输出“看起来是正确的”。
// 不过当然，这个内部表述的详细情况是很重要的。不单是为了这个文件的实现是正确的，
// 而且偶尔直接进入矩阵变量也是必须的，或者有益于优化。因此，这里我们记录我们的矩阵约定。
// The purpose of this class is so that a user might perform transformations
// without fiddling with plus or minus signs or transposing the matrix
// until the output "looks right."  But of course, the specifics of the
// internal representation is important.  Not only for the implementation
// in this file to be correct, but occasionally direct access to the
// matrix variables is necessary, or beneficial for optimization.  Thus,
// we document our matrix conventions here.
//
// 我们使用行向量，所以跟矩阵相乘看起来是这样的：
// We use row vectors, so multiplying by our matrix looks like this:
//
//               | m11 m12 m13 |
//     [ x y z ] | m21 m22 m23 | = [ x' y' z' ]
//               | m31 m32 m33 |
//               | tx  ty  tz  |
// 
// 严格执行线性代数规则规定了这个乘法实际是未定义的。
// 为了绕过这个问题，我们可以假定输入和输出向量有第四个坐标为1。
// 同样，我们不可以根据线性代数规则以学术形式地反转4x3矩阵，我们也会假设
// 最右列是[ 0 0 0 1 ]。它看起来像如下这样的：
// Strict adherance to linear algebra rules dictates that this
// multiplication is actually undefined.  To circumvent this, we can
// consider the input and output vectors as having an assumed fourth
// coordinate of 1.  Also, since we cannot technically invert a 4x3 matrix
// according to linear algebra rules, we will also assume a rightmost
// column of [ 0 0 0 1 ].  This is shown below:
//
//                 | m11 m12 m13 0 |
//     [ x y z 1 ] | m21 m22 m23 0 | = [ x' y' z' 1 ]
//                 | m31 m32 m33 0 |
//                 | tx  ty  tz  1 |
//
// 万一你忘了矩阵乘法的线性代数规则（在第7.1.6和7.1.7节中描述过），查看操作*的扩展计算定义。
// In case you have forgotten your linear algebra rules for multiplying
// matrices (which are described in section 7.1.6 and 7.1.7), see the
// definition of operator* for the expanded computations.
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Matrix4x3类成员
// Matrix4x3 class members
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Matrix4x3::identity
//
// 设置矩阵为单位矩阵
// Set the matrix to identity

void	Matrix4x3::identity() {
	m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
	tx  = 0.0f; ty  = 0.0f; tz  = 1.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::zeroTranslation
//
// 将矩阵的第四行都置为0，该行包含了平移部分
// Zero the 4th row of the matrix, which contains the translation portion.

void	Matrix4x3::zeroTranslation() {
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setTranslation
//
// 用向量形式设置矩阵的平移部分
// Sets the translation portion of the matrix in vector form

void	Matrix4x3::setTranslation(const Vector3 &d) {
	tx = d.x; ty = d.y; tz = d.z;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupTranslation
//
// 用向量形式设置矩阵的平移部分，并设置线性变换部分为单位矩阵
// Sets the translation portion of the matrix in vector form

void	Matrix4x3::setupTranslation(const Vector3 &d) {

	// 设置线性变换部分为单位矩阵
	// Set the linear transformation portion to identity

	m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;

	// 设置平移部分
	// Set the translation portion

	tx = d.x; ty = d.y; tz = d.z;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupLocalToParent
// 
// 给定一个父参考帧中的局部参考帧的位置和朝向，构造矩阵来执行从局部空间->父空间的转换。
// Setup the matrix to perform a local -> parent transformation, given
// the position and orientation of the local reference frame within the
// parent reference frame.
//
// 一个非常常见的用法是构造从物体->世界矩阵。
// 作为一个示例，这种情况的转换是很直接的。
// 我们首先从物体坐标系到惯性坐标系的转换，然后平移到世界空间。
// A very common use of this will be to construct a object -> world matrix.
// As an example, the transformation in this case is straightforward.  We
// first rotate from object space into inertial space, then we translate
// into world space.
//
// 我们允许通过欧拉角或者旋转矩阵来指定方位。
// We allow the orientation to be specified using either euler angles,
// or a RotationMatrix

void	Matrix4x3::setupLocalToParent(const Vector3 &pos, const EulerAngles &orient) {

	// 创建旋转矩阵
	// Create a rotation matrix.

	RotationMatrix orientMatrix;
	orientMatrix.setup(orient);

	// 构造4x3矩阵。注意：如果我们真的关心速度，我们可以直接创建这个矩阵到各个变量，
	// 而不用使用临时的旋转矩阵。这会节约函数调用和一些的拷贝操作开销。
	// Setup the 4x3 matrix.  Note: if we were really concerned with
	// speed, we could create the matrix directly into these variables,
	// without using the temporary RotationMatrix object.  This would
	// save us a function call and a few copy operations.

	setupLocalToParent(pos, orientMatrix);
}

void	Matrix4x3::setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient) {

	// 复制矩阵的旋转部分。根据RotationMatrix.cpp的注释，这个旋转矩阵通常是惯性坐标系->物体坐标系的矩阵，
	// 也就是从父空间->局部空间。我们想要局部空间->父空间旋转，复制的时候必须要转置。
	// Copy the rotation portion of the matrix.  According to
	// the comments in RotationMatrix.cpp, the rotation matrix
	// is "normally" an inertial->object matrix, which is
	// parent->local.  We want a local->parent rotation, so we
	// must transpose while copying

	m11 = orient.m11; m12 = orient.m21; m13 = orient.m31;
	m21 = orient.m12; m22 = orient.m22; m23 = orient.m32;
	m31 = orient.m13; m32 = orient.m23; m33 = orient.m33;

	// 现在设置平移部分。平移发生在3x3部分之后，所以我们可以简单直接地复制位置。
	// Now set the translation portion.  Translation happens "after"
	// the 3x3 portion, so we can simply copy the position
	// field directly

	tx = pos.x; ty = pos.y; tz = pos.z;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupParentToLocal
// 
// 给定在父空间参考帧下的局部坐标的位置和方向参考帧，构造矩阵来执行父空间->局部空间。
// Setup the matrix to perform a parent -> local transformation, given
// the position and orientation of the local reference frame within the
// parent reference frame.
// 
// 它的一个非常常见的用法就是从世界坐标系->物体坐标系。
// 为了执行这个转换，我们通常首先会从世界坐标系到惯性坐标系的转换，然后从惯性坐标系到物体坐标系的旋转。
// 然而4x3矩阵可以完成后一个转换。所以我们可以创建两个矩阵T和R，然后连接M = TR。
// A very common use of this will be to construct a world -> object matrix.
// To perform this transformation, we would normally FIRST transform
// from world to inertial space, and then rotate from inertial space into
// object space.  However, out 4x3 matrix always translates last.  So
// we think about creating two matrices T and R, and then concatonating
// M = TR.
//
// 我们允许使用欧拉角或者旋转矩阵来指定朝向。
// We allow the orientation to be specified using either euler angles,
// or a RotationMatrix

void	Matrix4x3::setupParentToLocal(const Vector3 &pos, const EulerAngles &orient) {

	// 创建一个旋转矩阵。
	// Create a rotation matrix.

	RotationMatrix orientMatrix;
	orientMatrix.setup(orient);

	// 构造4x3矩阵。
	// Setup the 4x3 matrix.

	setupParentToLocal(pos, orientMatrix);
}

void	Matrix4x3::setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient) {

	// 复制矩阵的旋转部分。我们根据RotationMatrix.cpp注释中的布局，可以直接复制元素（不用转置）
	// Copy the rotation portion of the matrix.  We can copy the
	// elements directly (without transposing) according
	// to the layout as commented in RotationMatrix.cpp

	m11 = orient.m11; m12 = orient.m12; m13 = orient.m13;
	m21 = orient.m21; m22 = orient.m22; m23 = orient.m23;
	m31 = orient.m31; m32 = orient.m32; m33 = orient.m33;

	// 现在设置平移部分。通常地，我们通过取位置的负号来实现从世界到惯性坐标系。
	// 然而，我们必须修正这个事实——旋转是先发生的。所以必须先旋转平移部分。
	// 这和创建一个平移矩阵T来平移-pos，和旋转矩阵R，并且创建一个连接矩阵TR是一样的。
	// Now set the translation portion.  Normally, we would
	// translate by the negative of the position to translate
	// from world to inertial space.  However, we must correct
	// for the fact that the rotation occurs "first."  So we
	// must rotate the translation portion.  This is the same
	// as create a translation matrix T to translate by -pos,
	// and a rotation matrix R, and then creating the matrix
	// as the concatenation of TR

	tx = -(pos.x*m11 + pos.y*m21 + pos.z*m31);
	ty = -(pos.x*m12 + pos.y*m22 + pos.z*m32);
	tz = -(pos.x*m13 + pos.y*m23 + pos.z*m33);
}

//---------------------------------------------------------------------------
// Matrix4x3::setupRotate
//
// 构建矩阵执行关于主要轴的旋转
// Setup the matrix to perform a rotation about a cardinal axis
//
// 使用基于1的索引指定旋转轴
// The axis of rotation is specified using a 1-based index:
//
//  1 => 关于x轴旋转
//  2 => 关于y轴旋转
//  3 => 关于z轴旋转
//	1 => rotate about the x-axis
//	2 => rotate about the y-axis
//	3 => rotate about the z-axis
//
// theta是以弧度计的旋转量。使用左手法则定义正方向的旋转。
// theta is the amount of rotation, in radians.  The left-hand rule is
// used to define "positive" rotation.
// 
// 平移部分被重设。
// The translation portion is reset.
//
// 参见8.2.2获得更多信息。
// See 8.2.2 for more info.

void	Matrix4x3::setupRotate(int axis, float theta) {

	// 获得旋转角的sin和cos值
	// Get sin and cosine of rotation angle

	float	s, c;
	sinCos(&s, &c, theta);

	// 检查关于哪根轴旋转
	// Check which axis they are rotating about

	switch (axis) {

				// 关于x轴旋转
		case 1: // Rotate about the x-axis

			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = 0.0f; m22 = c;    m23 = s;
			m31 = 0.0f; m32 = -s;   m33 = c;
			break;

				// 关于y轴旋转
		case 2: // Rotate about the y-axis

			m11 = c;    m12 = 0.0f; m13 = -s;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = s;    m32 = 0.0f; m33 = c;
			break;
				
				// 关于z轴旋转
		case 3: // Rotate about the z-axis

			m11 = c;    m12 = s;    m13 = 0.0f;
			m21 = -s;   m22 = c;    m23 = 0.0f;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
			break;

		default:

			// 不存在的轴索引
			// bogus axis index

			assert(false);
	}

	// 重设平移部分
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupRotate
//
// 构建关于任意轴的旋转矩阵。
// 该轴必须通过原点。
// Setup the matrix to perform a rotation about an arbitrary axis.
// The axis of rotation must pass through the origin.
//
// axis定义了哪根旋转轴，并且必须是单位向量。
// axis defines the axis of rotation, and must be a unit vector.
//
// theta是旋转量，以弧度计。左手法则定义了旋转的正方向。
// theta is the amount of rotation, in radians.  The left-hand rule is
// used to define "positive" rotation.
//
// 平移部分会被重置。
// The translation portion is reset.
//
// 查看8.2.3来获得更多信息。
// See 8.2.3 for more info.

void	Matrix4x3::setupRotate(const Vector3 &axis, float theta) {

	// 快速清楚检查确保axis是单位向量
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(axis*axis - 1.0f) < .01f);

	// 获得旋转角的sin和cos值
	// Get sin and cosine of rotation angle

	float	s, c;
	sinCos(&s, &c, theta);

	// 计算1 - cos(theta) 和一些公共的子表达式
	// Compute 1 - cos(theta) and some common subexpressions

	float	a = 1.0f - c;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	float	az = a * axis.z;

	// 设置矩阵元素。这里仍然有一些机会去优化，出于许多公共的子表达式。
	// 我们会让编译器来处理它..
	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	m11 = ax*axis.x + c;
	m12 = ax*axis.y + axis.z*s;
	m13 = ax*axis.z - axis.y*s;

	m21 = ay*axis.x - axis.z*s;
	m22 = ay*axis.y + c;
	m23 = ay*axis.z + axis.x*s;

	m31 = az*axis.x + axis.y*s;
	m32 = az*axis.y - axis.x*s;
	m33 = az*axis.z + c;
	
	// 重置平移部分
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::fromQuaternion
//
// 构建矩阵来执行旋转，给定一个四元数形式的角位移。
// Setup the matrix to perform a rotation, given the angular displacement
// in quaternion form.
//
// 平抑部分会被重置。
// The translation portion is reset.
//
// 查看10.6.3来获得更多信息。
// See 10.6.3 for more info.

void	Matrix4x3::fromQuaternion(const Quaternion &q) {

	// 计算一些值来优化公共子表达式
	// Compute a few values to optimize common subexpressions

	float	ww = 2.0f * q.w;
	float	xx = 2.0f * q.x;
	float	yy = 2.0f * q.y;
	float	zz = 2.0f * q.z;

	// 设置矩阵元素。这里仍然有一些机会去优化，出于许多公共的子表达式。
	// 我们会让编译器来处理它..
	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	m11 = 1.0f - yy*q.y - zz*q.z;
	m12 = xx*q.y + ww*q.z;
	m13 = xx*q.z - ww*q.x;

	m21 = xx*q.y - ww*q.z;
	m22 = 1.0f - xx*q.x - zz*q.z;
	m23 = yy*q.z + ww*q.x;

	m31 = xx*q.z + ww*q.y;
	m32 = yy*q.z - ww*q.x;
	m33 = 1.0f - xx*q.x - yy*q.y;

	// 重置平移部分。
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupScale
// 
// 构建矩阵来执行每根轴上的缩放。使用向量形式的Vector3(k,k,k)来统一缩放k倍。
// Setup the matrix to perform scale on each axis.  For uniform scale by k,
// use a vector of the form Vector3(k,k,k)
//
// 这个平移部分会被重置。
// The translation portion is reset.
//
// 查看8.3.1来获得更多信息。
// See 8.3.1 for more info.

void	Matrix4x3::setupScale(const Vector3 &s) {

	// 设置矩阵元素。相当直接
	// Set the matrix elements.  Pretty straightforward

	m11 = s.x;  m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = s.y;  m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = s.z;

	// 重置平移部分。
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupScaleAlongAxis
//
// 构建矩阵来执行沿着任意轴的缩放。
// Setup the matrix to perform scale along an arbitrary axis.
//
// 这根轴被单位向量指定。
// The axis is specified using a unit vector.
//
// 平移部分会被重置。
// The translation portion is reset.
//
// 查看8.3.2节来获得更多信息。
// See 8.3.2 for more info.

void	Matrix4x3::setupScaleAlongAxis(const Vector3 &axis, float k) {

	// 快速清楚检查确保axis是单位向量
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(axis*axis - 1.0f) < .01f);

	// 计算k-1和一些公共子表达式
	// Compute k-1 and some common subexpressions

	float	a = k - 1.0f;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	float	az = a * axis.z;

	// 填充矩阵元素。我们会完成我们的公共子表达式的优化，因为对角相应的矩阵元素是相等的
	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal

	m11 = ax*axis.x + 1.0f;
	m22 = ay*axis.y + 1.0f;
	m32 = az*axis.z + 1.0f;

	m12 = m21 = ax*axis.y;
	m13 = m31 = ax*axis.z;
	m23 = m32 = ay*axis.z;

	// 重设平移部分
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupShear
// 
// 构建矩阵来执行切变
// Setup the matrix to perform a shear
//
// 切变类型被基于1的轴索引指定。通过一个矩阵变换一个点的效果如下面的伪代码所示：
// The type of shear is specified by the 1-based "axis" index.  The effect
// of transforming a point by the matrix is described by the pseudocode
// below:
//
//	axis == 1  =>  y += s*x, z += t*x
//	axis == 2  =>  x += s*y, z += t*y
//	axis == 3  =>  x += s*z, y += t*z
//
// 平移部分会被重置。
// The translation portion is reset.
//
// 查看8.6来获得更多信息。
// See 8.6 for more info.

void	Matrix4x3::setupShear(int axis, float s, float t) {

	// 检查它们想要那种切变类型
	// Check which type of shear they want

	switch (axis) {

				// 使用x切变y和z
		case 1: // Shear y and z using x

			m11 = 1.0f; m12 = s;    m13 = t;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
			break;

				//使用y切变x和z
		case 2: // Shear x and z using y

			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = s;    m22 = 1.0f; m23 = t;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
			break;
			
				// 使用z来切变x和y
		case 3: // Shear x and y using z

			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = s;    m32 = t;    m33 = 1.0f;
			break;

		default:

			// 不存在的轴索引
			// bogus axis index

			assert(false);
	}

	// 重设平移部分
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupProject
//
// 构建矩阵来执行投影到一个通过原点的平面。这个平面垂直于单位向量n。
// Setup the matrix to perform a projection onto a plane passing
// through the origin.  The plane is perpendicular to the
// unit vector n.
//
// 查看8.4.2来获得更多的信息。
// See 8.4.2 for more info.

void	Matrix4x3::setupProject(const Vector3 &n) {

	// 快速清楚检查确保axis是单位向量
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(n*n - 1.0f) < .01f);

	// 填充矩阵元素。我们会完成我们的公共子表达式的优化，因为对角相应的矩阵元素是相等的
	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal

	m11 = 1.0f - n.x*n.x;
	m22 = 1.0f - n.y*n.y;
	m33 = 1.0f - n.z*n.z;

	m12 = m21 = -n.x*n.y;
	m13 = m31 = -n.x*n.z;
	m23 = m32 = -n.y*n.z;

	// 重置平移部分
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupReflect
//
// 构建矩阵关于一个平行于基本平面的反射。
// Setup the matrix to perform a reflection about a plane parallel
// to a cardinal plane.
//
// axis轴是基于1的索引，指定了关于哪个平面的投影：
// axis is a 1-based index which specifies the plane to project about:
//
//  1 => 关于平面 x=k 的反射
//  2 => 关于平面 y=k 的反射
//  3 => 关于平面 z=k 的反射
//	1 => reflect about the plane x=k
//	2 => reflect about the plane y=k
//	3 => reflect about the plane z=k
//
// 平移部分会被恰当地设置，因为如果k!=0的时候平移必须会发生。
// The translation is set appropriately, since translation must occur if
// k != 0
//
// See 8.5 for more info.

void	Matrix4x3::setupReflect(int axis, float k) {

	// 检查关于哪个平面的反射
	// Check which plane they want to reflect about

	switch (axis) {
		
				// 关于平面x=k平面的反射
		case 1: // Reflect about the plane x=k

			m11 = -1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 =  1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 =  1.0f;

			tx = 2.0f * k;
			ty = 0.0f;
			tz = 0.0f;

			break;

				// 关于y=k平面的反射
		case 2: // Reflect about the plane y=k

			m11 =  1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 = -1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 =  1.0f;

			tx = 0.0f;
			ty = 2.0f * k;
			tz = 0.0f;

			break;

				// 关于z=k平面的反射
		case 3: // Reflect about the plane z=k

			m11 =  1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 =  1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 = -1.0f;

			tx = 0.0f;
			ty = 0.0f;
			tz = 2.0f * k;

			break;

		default:

			// 不存在的轴索引
			// bogus axis index

			assert(false);
	}

}

//---------------------------------------------------------------------------
// Matrix4x3::setupReflect
// 
// 设置矩阵执行关于任意通过原点的平面的反射。这个单位向量n垂直于平面。
// Setup the matrix to perform a reflection about an arbitrary plane
// through the origin.  The unit vector n is perpendicular to the plane.
// 
// 平移部分会被重置。
// The translation portion is reset.
//
// 查看8.5节来获得更多的信息。
// See 8.5 for more info.

void	Matrix4x3::setupReflect(const Vector3 &n) {

	// 快速清楚检查确保axis是单位向量
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(n*n - 1.0f) < .01f);

	// 计算公共子表达式
	// Compute common subexpressions

	float	ax = -2.0f * n.x;
	float	ay = -2.0f * n.y;
	float	az = -2.0f * n.z;

	// 填充矩阵元素。我们会完成我们的公共子表达式的优化，因为对角相应的矩阵元素是相等的
	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal

	m11 = 1.0f + ax*n.x;
	m22 = 1.0f + ay*n.y;
	m32 = 1.0f + az*n.z;

	m12 = m21 = ax*n.y;
	m13 = m31 = ax*n.z;
	m23 = m32 = ay*n.z;

	// 重置平移部分。
	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Vector * Matrix4x3
//
// 变换顶点。这会使得向量类看起来和线性代数纸上记号是一致的。
// Transform the point.  This makes using the vector class look like it
// does with linear algebra notation on paper.
//
// 我们也会提供*=操作，作为C语言的习惯约定。
// We also provide a *= operator, as per C convention.
//
// 参见7.1.7节
// See 7.1.7


Vector3	operator*(const Vector3 &p, const Matrix4x3 &m) {

	// 通过线性代数磨合
	// Grind through the linear algebra.

	return Vector3(
		p.x*m.m11 + p.y*m.m21 + p.z*m.m31 + m.tx,
		p.x*m.m12 + p.y*m.m22 + p.z*m.m32 + m.ty,
		p.x*m.m13 + p.y*m.m23 + p.z*m.m33 + m.tz
	);
}

Vector3 &operator*=(Vector3 &p, const Matrix4x3 &m) {
	p = p * m;
	return p;
}

//---------------------------------------------------------------------------
// Matrix4x3 * Matrix4x3
//
// 矩阵连接。这会使得矩阵类看起来和线性代数纸上记号是一致的。
// Matrix concatenation.  This makes using the vector class look like it
// does with linear algebra notation on paper.
//
// 我们也会提供*=操作，作为C语言的习惯约定。
// We also provide a *= operator, as per C convention.
//
// 参见7.1.6节
// See 7.1.6

Matrix4x3 operator*(const Matrix4x3 &a, const Matrix4x3 &b) {

	Matrix4x3 r;

	// 计算上面3x3（线性变换）部分
	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

	// 计算平移部分
	// Compute the translation portion

	r.tx = a.tx*b.m11 + a.ty*b.m21 + a.tz*b.m31 + b.tx;
	r.ty = a.tx*b.m12 + a.ty*b.m22 + a.tz*b.m32 + b.ty;
	r.tz = a.tx*b.m13 + a.ty*b.m23 + a.tz*b.m33 + b.tz;

	// 返回它。哎呀 - 会调用了构造函数。如果速度是至关重要的，我们可能需要
	// 一个不同的函数在我们想要的地方放置它...
	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...

	return r;
}

Matrix4x3 &operator*=(Matrix4x3 &a, const Matrix4x3 &b) {
	a = a * b;
	return a;
}

//---------------------------------------------------------------------------
// determinant
//
// 计算3x3部分的矩阵行列式
// Compute the determinant of the 3x3 portion of the matrix.
//
// 参见9.1.1节获得更多信息。
// See 9.1.1 for more info.

float	determinant(const Matrix4x3 &m) {
	return
		  m.m11 * (m.m22*m.m33 - m.m23*m.m32)
		+ m.m12 * (m.m23*m.m31 - m.m21*m.m33)
		+ m.m13 * (m.m21*m.m32 - m.m22*m.m31);
}

//---------------------------------------------------------------------------
// inverse
//
// 计算矩阵的逆。我们使用传统的用伴随矩阵除以行列式来的方法计算。
// Compute the inverse of a matrix.  We use the classical adjoint divided
// by the determinant method.
//
// 参见9.2.1来获得更多信息。
// See 9.2.1 for more info.

Matrix4x3 inverse(const Matrix4x3 &m) {

	// 计算行列式
	// Compute the determinant

	float	det = determinant(m);

	// 如果是奇异矩阵，行列式是零，并且没有逆矩阵
	// If we're singular, then the determinant is zero and there's
	// no inverse

	assert(fabs(det) > 0.000001f);

	// 计算一除以行列式，所以我们只需要除一次，然后乘去每一个元素。
	// Compute one over the determinant, so we divide once and
	// can *multiply* per element

	float	oneOverDet = 1.0f / det;

	// 计算3x3部分的逆矩阵，通过伴随矩阵除以行列式。
	// Compute the 3x3 portion of the inverse, by
	// dividing the adjoint by the determinant

	Matrix4x3	r;

	r.m11 = (m.m22*m.m33 - m.m23*m.m32) * oneOverDet;
	r.m12 = (m.m13*m.m32 - m.m12*m.m33) * oneOverDet;
	r.m13 = (m.m12*m.m23 - m.m13*m.m22) * oneOverDet;

	r.m21 = (m.m23*m.m31 - m.m21*m.m33) * oneOverDet;
	r.m22 = (m.m11*m.m33 - m.m13*m.m31) * oneOverDet;
	r.m23 = (m.m13*m.m21 - m.m11*m.m23) * oneOverDet;

	r.m31 = (m.m21*m.m32 - m.m22*m.m31) * oneOverDet;
	r.m32 = (m.m12*m.m31 - m.m11*m.m32) * oneOverDet;
	r.m33 = (m.m11*m.m22 - m.m12*m.m21) * oneOverDet;

	// 计算逆矩阵的平移部分
	// Compute the translation portion of the inverse

	r.tx = -(m.tx*r.m11 + m.ty*r.m21 + m.tz*r.m31);
	r.ty = -(m.tx*r.m12 + m.ty*r.m22 + m.tz*r.m32);
	r.tz = -(m.tx*r.m13 + m.ty*r.m23 + m.tz*r.m33);

	// 返回它。哎呀 - 会调用了构造函数。如果速度是至关重要的，我们可能需要
	// 一个不同的函数在我们想要的地方放置它...
	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...

	return r;
}

//---------------------------------------------------------------------------
// getTranslation
//
// 以向量形式返回矩阵的平移部分
// Return the translation row of the matrix in vector form

Vector3	getTranslation(const Matrix4x3 &m) {
	return Vector3(m.tx, m.ty, m.tz);
}

//---------------------------------------------------------------------------
// getPositionFromParentToLocalMatrix
//
// 提取从父空间->局部空间变换矩阵的平移部分（就像世界坐标系->物体坐标系矩阵）
// Extract the position of an object given a parent -> local transformation
// matrix (such as a world -> object matrix)
//
// 我们假定矩阵呈现的是坚固的变换。（没有缩放、倾斜、或者镜像）
// We assume that the matrix represents a rigid transformation.  (No scale,
// skew, or mirroring)

Vector3	getPositionFromParentToLocalMatrix(const Matrix4x3 &m) {

	// 通过转置3x3部分乘以负平移值。通过矩阵的转置，我们假定矩阵是正交的。（这个函数对于非坚固变换的变换是没有意义的）
	// Multiply negative translation value by the
	// transpose of the 3x3 portion.  By using the transpose,
	// we assume that the matrix is orthogonal.  (This function
	// doesn't really make sense for non-rigid transformations...)

	return Vector3(
		-(m.tx*m.m11 + m.ty*m.m12 + m.tz*m.m13),
		-(m.tx*m.m21 + m.ty*m.m22 + m.tz*m.m23),
		-(m.tx*m.m31 + m.ty*m.m32 + m.tz*m.m33)
	);
}

//---------------------------------------------------------------------------
// getPositionFromLocalToParentMatrix
//
// 提取给定局部坐标系->父坐标系的位置变换矩阵（例如物体坐标系->世界坐标系矩阵）
// Extract the position of an object given a local -> parent transformation
// matrix (such as an object -> world matrix)

Vector3	getPositionFromLocalToParentMatrix(const Matrix4x3 &m) {

	// 位置简明地是平移部分
	// Position is simply the translation portion

	return Vector3(m.tx, m.ty, m.tz);
}
