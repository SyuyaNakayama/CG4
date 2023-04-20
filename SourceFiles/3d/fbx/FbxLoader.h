#pragma once
#include "fbxsdk.h"
#include "FbxModel.h"

class FbxLoader
{
private:
	using string = std::string;
	static const string BASE_DIRECTORY;

	static FbxManager* fbxManager;
	static FbxImporter* fbxImporter;
	static const string DEFAULT_TEXTURE_FILE_NAME;

	// コンストラクタ、デストラクタ削除
	FbxLoader() = delete;
	~FbxLoader() = delete;

	static void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	static void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	static void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	static void LoadTexture(FbxModel* model, const string& FULLPATH);
	static void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);
	static void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	static string ExtractFileName(const string& PATH);

public:
	static void Initialize();
	static void Finalize();
	static FbxModel* LoadModelFromFile(const string& modelName);
};