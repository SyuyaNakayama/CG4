#include "Vector.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"
#include <cmath>
#include <cassert>

Vector2 Vector2::Normalize()
{
	float len = Length();
	if (len != 0) { *this /= len; }
	return *this;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp *= s;
}

const Vector2 operator*(float s, const Vector2& v) { return v * s; }

const Vector2 operator/(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp /= s;
}

Vector3 Vector3::Normalize()
{
	float len = Length();
	if (len != 0) { *this /= len; }
	return *this;
}

Vector3 Vector3::abs()
{
	Vector3 ans = *this;

	ans.x = fabsf(ans.x);
	ans.y = fabsf(ans.y);
	ans.z = fabsf(ans.z);

	return ans;
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator*=(const Matrix4& m)
{
	Vector3 temp = *this * m;
	*this = temp;
	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

bool Vector3::operator<=(Vector3 vec)
{
	if (x <= vec.x && y <= vec.y && z <= vec.z) { return true; }
	return false;
}

bool Vector3::operator>=(Vector3 vec)
{
	if (x >= vec.x && y >= vec.y && z >= vec.z) { return true; }
	return false;
}

bool Vector3::operator<(Vector3 vec)
{
	if (x < vec.x && y < vec.y && z < vec.z) { return true; }
	return false;
}

bool Vector3::operator>(Vector3 vec)
{
	if (x > vec.x && y > vec.y && z > vec.z) { return true; }
	return false;
}

bool Vector3::operator==(Vector3 vec)
{
	if (x == vec.x && y == vec.y && z == vec.z) { return true; }
	return false;
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v) { return v * s; }

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}

float Dot(const Vector2& v1, const Vector2& v2) { return v1.x * v2.x + v1.y * v2.y; }
float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
float Cross(const Vector2& v1, const Vector2& v2) { return v1.x * v2.y - v1.y * v2.x; }
Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp;

	temp =
	{
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};

	return temp;
}
float Length(const Vector3& v) { return Vector3(v).Length(); }
Vector2 Normalize(const Vector2& v) { return Vector2(v).Normalize(); }
Vector3 Normalize(const Vector3& v) { return Vector3(v).Normalize(); }
Vector2 To2DVector(const Vector3& vec)
{
	Vector3 v = vec;
	v *= WorldTransform::GetViewProjection()->GetViewProjectionMatrix();
	v *= DirectXCommon::GetInstance()->GetViewportMatrix();
	return v;
}
Vector3 Lerp(const Vector3& start, const Vector3& end, const float t) { return start * (1.0f - t) + end * t; }

Vector3 BezierCurve(std::vector<Vector3> p, float t)
{
	assert(p.size() >= 2);
	// êßå‰ì_2Ç¬ÇÃÇ∆Ç´ÇÕê¸å`ï‚ä‘
	if (p.size() == 2) { return Lerp(p[0], p[1], t); }

	std::vector<Vector3> controlPoints;
	for (size_t i = 0; i < p.size() - 1; i++) { controlPoints.push_back(Lerp(p[i], p[i + 1], t)); }
	while (controlPoints.size() != 2)
	{
		std::vector<Vector3> points;
		for (size_t i = 0; i < controlPoints.size() - 1; i++) 
		{
			points.push_back(Lerp(controlPoints[i], controlPoints[i + 1], t));
		}
		controlPoints = points;
	}
	return Lerp(controlPoints[0], controlPoints[1], t);
}

Vector3 SplineCurve(const std::vector<Vector3>& points, size_t startIndex, float t)
{
	size_t n = points.size() - 2;

	if (startIndex > n) { return points[n]; }
	if (startIndex < 1) { return points[1]; }

	std::vector<Vector3> p =
	{
		points[startIndex - 1],
		points[startIndex],
		points[startIndex + 1],
		points[startIndex + 2],
	};

	return 0.5f * (
		2 * p[1] + (-p[0] + p[2]) * t +
		(2 * p[0] - 5 * p[1] + 4 * p[2] - p[3]) * t * t +
		(-p[0] + 3 * p[1] - 3 * p[2] + p[3]) * t * t * t);
}