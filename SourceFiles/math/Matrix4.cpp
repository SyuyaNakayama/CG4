#include "Matrix4.h"
#include "WorldTransform.h"
#include <cmath>
#include <cassert>

Matrix4 Matrix4::operator*=(const Matrix4& m2)
{
	Matrix4 result = Zero();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
			{
				result.m[i][j] += m[i][k] * m2.m[k][j];
			}
		}
	}
	*this = result;
	return *this;
}

Matrix4 Matrix4::Identity()
{
	Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::Zero()
{
	Matrix4 result
	{
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f
	};

	return result;
}

Matrix4 Matrix4::Scale(const Vector3& s)
{
	Matrix4 result
	{
		s.x ,0.0f,0.0f,0.0f,
		0.0f,s.y ,0.0f,0.0f,
		0.0f,0.0f,s.z ,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::RotateX(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos ,sin ,0.0f,
		0.0f,-sin,cos ,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::RotateY(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		cos ,0.0f,-sin,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		sin ,0.0f,cos ,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::RotateZ(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		cos ,sin ,0.0f,0.0f,
		-sin,cos ,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

Matrix4 Matrix4::Rotate(const Vector3& r)
{
	return RotateZ(r.z) * RotateX(r.x) * RotateY(r.y);
}

Matrix4 Matrix4::Translate(const Vector3& t)
{
	Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		t.x ,t.y ,t.z ,1.0f
	};

	return result;
}

Matrix4 Matrix4::Transpose(const Matrix4& mat)
{
	Matrix4 ans;

	for (size_t i = 0; i < 4; i++){
		for (size_t j = 0; j < 4; j++)
		{
			ans.m[i][j] = mat.m[j][i];
		}
	}

	return ans;
}

Matrix4 Matrix4::Inverse(const Matrix4& mat)
{
	Matrix4 m = mat;
	Matrix4 inv; // ここに逆行列が入る

	// 掃き出し法
	for (size_t i = 0; i < 4; i++)
	{
		float buf = 1.0f / m.m[i][i]; // 一時的なデータを蓄える
		for (size_t j = 0; j < 4; j++)
		{
			m.m[i][j] *= buf;
			inv.m[i][j] *= buf;
		}
		for (size_t j = 0; j < 4; j++)
		{
			if (i == j) { continue; }
			buf = m.m[j][i];
			for (size_t k = 0; k < 4; k++)
			{
				m.m[j][k] -= m.m[i][k] * buf;
				inv.m[j][k] -= inv.m[i][k] * buf;
			}
		}
	}

	return inv;
}

Matrix4 Matrix4::CreateFromVector(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4)
{
	Matrix4 result
	{
		vec1.x,vec1.y,vec1.z,0.0f,
		vec2.x,vec2.y,vec2.z,0.0f,
		vec3.x,vec3.y,vec3.z,0.0f,
		vec4.x,vec4.y,vec4.z,1.0f
	};

	return result;
}

Matrix4 Matrix4::GetBillboard()
{
	ViewProjection* vp = WorldTransform::GetViewProjection();
	Vector3 cameraAxisZ = vp->target - vp->eye;
	// 0ベクトルの時
	assert(!(cameraAxisZ == Vector3(0, 0, 0)));
	assert(!(vp->up == Vector3(0, 0, 0)));

	cameraAxisZ.Normalize();

	Vector3 cameraAxisX = Normalize(Cross(vp->up, cameraAxisZ));
	Vector3 cameraAxisY = Normalize(Cross(cameraAxisZ, cameraAxisX));
	return Matrix4::CreateFromVector(cameraAxisX, cameraAxisY, cameraAxisZ);
}

Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result
	{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w
	};

	return result;
}

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result = m1;
	result *= m2;
	return result;
}