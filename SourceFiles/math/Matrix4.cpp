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

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++)
		{
			ans.m[i][j] = mat.m[j][i];
		}
	}

	return ans;
}

Matrix4 Matrix4::Inverse(const Matrix4& m)
{
	Matrix4 result;
	float mat[4][8]{};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
		{
			mat[i][j] = m.m[i][j];
		}
	}

	mat[0][4] = 1;
	mat[1][5] = 1;
	mat[2][6] = 1;
	mat[3][7] = 1;

	for (int n = 0; n < 4; n++) 
	{
		// 最大の絶対値を探索する(とりあえず対象成分を最大と仮定しておく)
		float max = abs(mat[n][n]);
		int maxIndex = n;

		for (int i = n + 1; i < 4; i++)
		{
			if (abs(mat[i][n]) > max) 
			{
				max = abs(mat[i][n]);
				maxIndex = i;
			}
		}

		// 最大の絶対値が0だったら逆行列は求められない
		if (abs(mat[maxIndex][n]) <= 1.0e-6f) 
		{
			return result; //とりあえず単位行列返しちゃう
		}

		// 入れ替え
		if (n != maxIndex) 
		{
			for (int i = 0; i < 8; i++) 
			{
				float f = mat[maxIndex][i];
				mat[maxIndex][i] = mat[n][i];
				mat[n][i] = f;
			}
		}

		// 掛けたら1になる値を算出
		float mul = 1 / mat[n][n];

		// 掛ける
		for (int i = 0; i < 8; i++) { mat[n][i] *= mul; }

		// 他全部0にする
		for (int i = 0; i < 4; i++) 
		{
			if (n == i) { continue; }

			float mul = -mat[i][n];

			for (int j = 0; j < 8; j++) { mat[i][j] += mat[n][j] * mul; }
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) 
		{
			result.m[i][j] = mat[i][j + 4];
		}
	}

	return result;
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