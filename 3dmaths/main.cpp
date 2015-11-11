#include <iostream>
#include "MathUtil.h"
#include "Matrix4x3.h"
#include "vector3.h"
#include "RotationMatrix.h"
#include "EulerAngles.h"

using namespace std;

int main()
{
	Vector3 vec(2,3,4);
	Matrix4x3 mat;
	mat.setupReflect(1,1);
	vec = vec * mat;
	return 0;
}