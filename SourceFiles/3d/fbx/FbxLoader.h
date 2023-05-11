#pragma once
#include "FbxModel.h"

class FbxLoader
{
private:
	using string = std::string;
	static const string BASE_DIRECTORY;
	static const string DEFAULT_TEXTURE_FILE_NAME;

	static FbxManager* fbxManager;
	static FbxImporter* fbxImporter;

	// コンストラクタ、デストラクタ削除
	FbxLoader() = delete;
	~FbxLoader() = delete;

public:
	static void Initialize();
	static void Finalize();
	static FbxModel* LoadModelFromFile(const string& modelName);
};