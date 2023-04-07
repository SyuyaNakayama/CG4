#pragma once
#include "Matrix4.h"
#include "WindowsAPI.h"
#include <wrl.h>
#include <d3d12.h>

class ViewProjection
{
private:
	Matrix4 matView, matProjection;

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Matrix4 viewproj; // ビュープロジェクション行列
		Vector3 cameraPos; // カメラ座標(ワールド座標)
	};

public:
	ConstBufferData* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;
	Vector3 eye = { 0, 0, -50.0f }, target, up = Vector3::MakeYAxis();
	float fovAngleY = 45.0f; // 画角(度)
	// アスペクト比(基本はWindowsAPI::WIN_SIZEに準拠)
	float aspectRatio = WindowsAPI::WIN_SIZE.x / WindowsAPI::WIN_SIZE.y;
	float nearZ = 0.1f, farZ = 1000.0f; // 映る範囲

	void Initialize();
	void Update();
	void CameraMove(const Vector3& move);
	Matrix4 GetViewProjectionMatrix() const { return matView * matProjection; }
};