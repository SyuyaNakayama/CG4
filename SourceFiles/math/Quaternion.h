#pragma once
#include "Matrix4.h"

class Quaternion
{
public:
	float x = 0, y = 0, z = 0, // 虚部
		w = 0; // 実部

	Quaternion(float w_, Vector3 iv) { x = iv.x, y = iv.y, z = iv.z, w = w_; }
	Quaternion(float w_ = 0, float x_ = 0, float y_ = 0, float z_ = 0) { x = x_, y = y_, z = z_; w = w_; }
	Vector3 GetImaginary() const { return { x,y,z }; }
	Quaternion operator-()const { return { -w,x,y,z }; }
	// 代入演算子オーバーロード
	void operator*=(const Quaternion& q);
	void operator/=(float norm) { x /= norm, y /= norm, z /= norm, w /= norm; }
	// 共役にする
	void Conjugate() { x = -x, y = -y, z = -z; }
	// このQuaternionのNormを返す
	float Norm() const;
	// 正規化する
	void Normalize() { *this /= Norm(); }
	// 逆Quaternionにする
	void Inverse();
	// 回転行列を求める
	Matrix4 MakeRotateMatrix() const;

	// 静的関数
	// メンバ変数の値を変えたくない時はこっちを使う
	// 単位Quaternion
	static Quaternion Identity() { return Quaternion(1, 0, 0, 0); }
	static Quaternion Conjugate(const Quaternion& q) { return Quaternion(q.w, -q.GetImaginary()); }
	static float Norm(const Quaternion& q) { return q.Norm(); }
	static Quaternion Normalize(const Quaternion& q);
	static Quaternion Inverse(const Quaternion& q);

	// 任意軸回転のQuaternion生成(axisは正規化されている必要がある)
	static Quaternion MakeAxisAngle(const Vector3& axis, float angle);
	// ベクトルをQuaternionで回転させる
	static Vector3 RotateVector(const Vector3& v, const Quaternion& q);
	// Quaternionから回転行列を求める
	static Matrix4 MakeRotateMatrix(const Quaternion& q);
};

float Dot(const Quaternion& q1, const Quaternion& q2);
// 2項演算子オーバーロード
Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator/(const Quaternion& q, float norm);

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

// uからvへの回転を生成(u,vは正規化されている前提)
Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);