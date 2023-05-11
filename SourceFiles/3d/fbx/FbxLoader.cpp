#include "FbxLoader.h"
#include "DirectXCommon.h"
#include <cassert>
using namespace DirectX;

FbxManager* FbxLoader::fbxManager = nullptr;
FbxImporter* FbxLoader::fbxImporter = nullptr;
const std::string FbxLoader::BASE_DIRECTORY = "Resources/fbx/";
const std::string FbxLoader::DEFAULT_TEXTURE_FILE_NAME = "white1x1.png";

void FbxLoader::Initialize()
{
	assert(fbxManager == nullptr); // 再初期化を防ぐ
	fbxManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);
	fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
	fbxImporter->Destroy();
	fbxManager->Destroy();
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	const string
		DIRECTORY_PATH = BASE_DIRECTORY + modelName + "/",
		FILE_NAME = modelName + ".fbx",
		FULLPATH = DIRECTORY_PATH + FILE_NAME;

	bool result = fbxImporter->Initialize(FULLPATH.c_str(), -1, fbxManager->GetIOSettings());
	assert(result);

	FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");
	fbxImporter->Import(fbxScene);

	FbxModel* model = new FbxModel();
	model->name = modelName;

	int nodeCount = fbxScene->GetNodeCount();
	model->nodes.reserve(nodeCount);

	model->ParseNodeRecursive(fbxScene->GetRootNode());
	fbxScene->Destroy();

	model->CreateBuffers();

	return model;
}