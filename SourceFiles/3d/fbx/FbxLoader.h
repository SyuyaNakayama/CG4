#pragma once

#include "fbxsdk.h"
#include "FbxModel.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <string>

class FbxLoader
{
private:
	using string = std::string;
public:
	static const string BASE_DIRECTORY;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	void Initialize(ID3D12Device* device);
	void Finalize();
	FbxModel* LoadModelFromFile(const string& modelName);
	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	string ExtractFileName(const string& PATH);
private:
	ID3D12Device* device = nullptr;
	FbxManager* fbxManager = nullptr;
	FbxImporter* fbxImporter = nullptr;
	static const string DEFAULT_TEXTURE_FILE_NAME;

	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	void LoadTexture(FbxModel* model, const string& FULLPATH);
};