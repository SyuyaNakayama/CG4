#include "Quaternion.h"
#include <cmath>

void Quaternion::operator*=(const Quaternion& q)
{
	Vector3 iq1 = this->GetImaginary();
	Vector3 iq2 = q.GetImaginary();

	Quaternion ans =
	{
		this->w * q.w - Dot(iq1,iq2),
		Cross(iq1,iq2) + q.w * iq1 + this->w * iq2
	};
	*this = ans;
}

float Quaternion::Norm() const
{
	Vector3 i = GetImaginary();
	return sqrt(Dot(i, i) + w * w);
}

void Quaternion::Inverse()
{
	Conjugate();
	*this /= (Norm() * Norm());
}

Matrix4 Quaternion::MakeRotateMatrix() const
{
	Matrix4 mat =
	{
		w * w + x * x - y * y - z * z, 2 * (x * y + w * z), 2 * (x * z - w * y), 0,
		2 * (x * y - w * z), w * w - x * x + y * y - z * z, 2 * (y * z - w * x), 0,
		2 * (x * z + w * y), 2 * (y * z + w * x), w * w - x * x - y * y + z * z, 0,
		0,0,0,1
	};

	return mat;
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	Quaternion ans = q;
	ans.Normalize();
	return ans;
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
	Quaternion ans = q;
	ans.Inverse();
	return ans;
}

Quaternion Quaternion::MakeAxisAngle(const Vector3& axis, float angle)
{
	return Quaternion(cosf(angle / 2.0f), axis * sinf(angle / 2.0f));
}

Vector3 Quaternion::RotateVector(const Vector3& v, const Quaternion& q)
{
	Quaternion r = { 0,v };
	return Quaternion(q * r * Conjugate(q)).GetImaginary();
}

Matrix4 Quaternion::MakeRotateMatrix(const Quaternion& q)
{
	return q.MakeRotateMatrix();
}

float Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.w * q2.w + Dot(q1.GetImaginary(), q2.GetImaginary());
}

Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.w + q2.w, q1.GetImaginary() + q2.GetImaginary());
}

Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.w - q2.w, q1.GetImaginary() - q2.GetImaginary());
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion ans = q1;
	ans *= q2;
	return ans;
}

Quaternion operator/(const Quaternion& q, float norm)
{
	Quaternion ans = q;
	ans /= norm;
	return ans;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion q0temp = q0;
	float dot = Dot(q0temp, q1);
	if (dot < 0)
	{
		q0temp = -q0temp;
		dot = -dot;
	}

	const float EPSILON = 1.0e-5f;
	if (dot >= 1.0f - EPSILON) { return (1.0f - t) * q0temp + t * q1; }

	float theta = std::acos(dot);
	float scale0 = std::sin((1 - t) * theta) / std::sin(theta);
	float scale1 = std::sin(t * theta) / std::sin(theta);
	return scale0 * q0temp + scale1 * q1;
}

Quaternion DirectionToDirection(const Vector3& u, const Vector3& v)
{
	float dot = Dot(u, v);
	Vector3 axis = Normalize(Cross(u, v));
	float theta = std::acos(dot);
	return Quaternion::MakeAxisAngle(axis, theta);
}