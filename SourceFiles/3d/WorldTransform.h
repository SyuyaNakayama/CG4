#pragma once
#include "ViewProjection.h"
#include "Quaternion.h"

class WorldTransform
{
private:
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Matrix4 world; // ワールド行列
	};

	// ビュープロジェクションのポインタ
	static ViewProjection* viewProjection_;
public:
	Matrix4 matWorld;
	ConstBufferData* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;
	Vector3 scale = { 1,1,1 }, rotation, translation;
	WorldTransform* parent = nullptr;

	void Initialize();
	void Update();
	// 回転にQuaternionを使う場合
	void Update(const Quaternion& rotQ);
	static void CameraUpdate();
	Vector3 GetWorldPosition() { return { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] }; }
	static void SetViewProjection(ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	static ViewProjection* GetViewProjection() { return viewProjection_; }
};