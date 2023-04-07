#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "Matrix4.h"
#include "DiffuseParticle.h"
#include "DirectionalParticle.h"

class ParticleManager final
{
private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 頂点データ構造体
	struct VertexPos
	{
		Vector3 pos; // xyz座標
		float scale; // スケール
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Matrix4 mat;	// 3D変換行列
		Matrix4 matBillboard; // ビルボード行列
	};

	static const int PARTICLE_MAX = 256; // パーティクル最大数

	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// 頂点バッファ
	static ComPtr<ID3D12Resource> vertBuff;
	static VertexPos* vertMap;
	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView;
	// 定数バッファ
	static ComPtr<ID3D12Resource> constBuff;
	static ConstBufferData* constMap;
	static uint32_t textureIndex;
	// 拡散するパーティクル
	static DiffuseParticle diffuseParticle;
	// 始点から終点へ向かうパーティクル
	static DirectionalParticle directionalParticle;
	
	static void InitializeGraphicsPipeline(); // グラフィックパイプライン生成
	static void CreateBuffers(); // バッファ生成
	static size_t AllParticleNum() { return diffuseParticle.GetParticles().size() + directionalParticle.GetParticles().size(); }
	static bool IsParticleMax() { return AllParticleNum() >= PARTICLE_MAX; }
	ParticleManager() = delete;
	ParticleManager(const ParticleManager& obj) = delete;

public:
	static void Initialize(); // 静的初期化
	static void Update(); // 毎フレーム処理
	static void Draw(); // 描画
	static void Clear(); // パーティクルの削除
	// パーティクルの追加
	static void Add(const DiffuseParticle::AddProp& particleProp);
	static void Add(const DirectionalParticle::AddProp& particleProp);
};

