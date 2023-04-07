#pragma once
#include "Vector.h"

class Matrix4
{
public:
	float m[4][4] =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	Matrix4 operator*=(const Matrix4& m2);

	// 静的関数
	static Matrix4 Identity();
	static Matrix4 Zero();
	static Matrix4 Scale(const Vector3& s);
	static Matrix4 RotateX(float angle);
	static Matrix4 RotateY(float angle);
	static Matrix4 RotateZ(float angle);
	static Matrix4 Rotate(const Vector3& r);
	static Matrix4 Translate(const Vector3& t);
	static Matrix4 Transpose(const Matrix4& mat);
	static Matrix4 Inverse(const Matrix4& mat);
	static Matrix4 CreateFromVector(const Vector3& vec1 = { 1,0,0 }, const Vector3& vec2 = { 0,1,0 }, const Vector3& vec3 = { 0,0,1 }, const Vector3& vec4 = {});
	static Matrix4 GetBillboard(); // 現在使用しているカメラに対してのビルボード行列を返す
};

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
Vector3 operator*(const Vector3& v, const Matrix4& m);