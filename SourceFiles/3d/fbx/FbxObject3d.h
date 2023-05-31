#pragma once
#include "FbxModel.h"
#include "WorldTransform.h"
#include "LightGroup.h"

class FbxObject3d
{
protected:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static const int MAX_BONES = 32;
	
	// 定数バッファ用データ構造体（座標変換行列用）
	struct ConstBufferData
	{
		Matrix4 viewproj;    // ビュープロジェクション行列
		Matrix4 world; // ワールド行列
		Vector3 cameraPos; // カメラ座標（ワールド座標）
	};

	struct ConstBufferDataSkin
	{
		Matrix4 bones[MAX_BONES];
	};
	
private: // 静的メンバ変数
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	static LightGroup* lightGroup;

protected: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff, constBuffSkin;
	ConstBufferData* constMap = nullptr;
	ConstBufferDataSkin* constMapSkin = nullptr;
	WorldTransform* worldTransform;
	// モデル
	FbxModel* model = nullptr;
	FbxTime frameTime, startTime, endTime, currentTime;
	bool isPlay = false;

public:
	static void SetLightGroup(LightGroup* lightGroup) { FbxObject3d::lightGroup = lightGroup; }

	/// <summary>
	/// グラフィックパイプラインの生成
	/// </summary>
	static void CreateGraphicsPipeline();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WorldTransform* worldTransform, FbxModel* model);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void PlayAnimation();
};

