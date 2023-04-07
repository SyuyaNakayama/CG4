#pragma once
#include "Sprite.h"
#include <vector>
#include <unordered_map>

// 形状データ
class Mesh
{
public:
	// 頂点データ構造体
	struct VertexData
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
		ColorRGBA color; // 色
	};

private:
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// マテリアル
	struct Material
	{
		std::string name;
		Vector3 ambient = { 0.3f,0.3f,0.3f };
		Vector3 diffuse;
		Vector3 specular;
		float alpha = 1.0f;
		std::string textureFilename;
	};

	struct ConstBufferData
	{
		Vector3 ambient;
		float pad1;
		Vector3 diffuse;
		float pad2;
		Vector3 specular;
		float alpha;
	};

	ComPtr<ID3D12Resource> vertBuff;	// 頂点バッファ
	ComPtr<ID3D12Resource> indexBuff;	// インデックスバッファ
	ComPtr<ID3D12Resource> constBuffer;	// 定数バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};	// 頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};	// インデックスバッファビュー
	VertexData* vertMap = nullptr;		// 頂点バッファのマップ
	ConstBufferData* constMap = nullptr;

	void LoadMaterial(const std::string& DIRECTORY_PATH, const std::string& FILENAME); // マテリアル読み込み
	void CalculateSmoothedVertexNormals();

protected:
	Material material;					// マテリアル
	std::unique_ptr<Sprite> sprite;		// テクスチャ
	std::vector<VertexData> vertices;	// 頂点データ配列
	std::vector<UINT16> indices;		// 頂点インデックス配列
	std::unordered_map<UINT16, std::vector<UINT16>> smoothData;	// 頂点座標スムージング用データ

	void LoadOBJ(const std::string& modelName, bool smoothing);
	void CreateBuffers(); // バッファの生成
	void Draw();

public:
	virtual ~Mesh() = default;
	void Update();
	void SetSprite(std::unique_ptr<Sprite> sprite_) { sprite = move(sprite_); }
	void SetAnbient(Vector3 anbient) { material.ambient = anbient; }
	Sprite* GetSprite() { return sprite.get(); }
};
