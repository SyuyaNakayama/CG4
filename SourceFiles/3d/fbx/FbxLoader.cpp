#include "FbxLoader.h"
#include "DirectXCommon.h"
#include <cassert>
using namespace DirectX;

const std::string FbxLoader::BASE_DIRECTORY = "Resources/";
const std::string FbxLoader::DEFAULT_TEXTURE_FILE_NAME = "white1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader instance;
	return &instance;
}

void FbxLoader::Initialize()
{
	assert(fbxManager == nullptr);
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

	ParseNodeRecursive(model, fbxScene->GetRootNode());
	fbxScene->Destroy();

	model->CreateBuffers();

	return model;
}

Vector3 FbxDouble3ToVector3(const FbxDouble3& num1)
{
	return Vector3((float)num1[0], (float)num1[1], (float)num1[2]);
}
Vector3 FbxDouble4ToVector3(const FbxDouble4& num1)
{
	return Vector3((float)num1[0], (float)num1[1], (float)num1[2]);
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent)
{
	model->nodes.emplace_back();
	Node& node = model->nodes.back();
	node.name = fbxNode->GetName();

	FbxDouble3 rotation = fbxNode->LclRotation.Get(),
		scaling = fbxNode->LclScaling.Get(),
		translation = fbxNode->LclTranslation.Get();

	node.rotation = FbxDouble3ToVector3(rotation);
	node.scaling = FbxDouble3ToVector3(scaling);
	node.translation = FbxDouble3ToVector3(translation);

	node.rotation = node.rotation.x*PI/108.0f;

	Matrix4 matScaling = Matrix4::Scale(node.scaling),
		matRotation = Matrix4::Rotate(node.rotation),
		matTranslation = Matrix4::Translate(node.translation);

	node.globalTransform = node.transform = matScaling * matRotation * matTranslation;

	if (parent)
	{
		node.parent = parent;
		node.globalTransform *= parent->globalTransform;
	}

	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			model->meshNode = &node;
			ParseMesh(model, fbxNode);
		}
	}

	for (size_t i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
	}
}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	ParseMeshVertices(model, fbxMesh);
	ParseMeshFaces(model, fbxMesh);
	ParseMaterial(model, fbxNode);
}

std::string FbxLoader::ExtractFileName(const string& PATH)
{
	size_t pos1 = PATH.rfind('\\');
	if (pos1 != string::npos) { return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1); }
	
	pos1 = PATH.rfind('/');
	if (pos1 != string::npos) { return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1); }

	return PATH;
}

void FbxLoader::ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh)
{
	auto& vertices = model->vertices;

	const int CONTROL_POINTS_COUNT = fbxMesh->GetControlPointsCount();

	FbxModel::VertexPosNormalUv vert{};
	model->vertices.resize(CONTROL_POINTS_COUNT, vert);

	FbxVector4* pCoord = fbxMesh->GetControlPoints();

	for (size_t i = 0; i < CONTROL_POINTS_COUNT; i++)
	{
		FbxModel::VertexPosNormalUv& vertex = vertices[i];
		vertex.pos = FbxDouble4ToVector3(pCoord[i]);
	}
}

void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh)
{
	auto& vertices = model->vertices;
	auto& indices = model->indices;
	assert(indices.size() == 0);

	const int POLYGON_COUNT = fbxMesh->GetPolygonCount();
	const int TEXTURE_UV_COUNT = fbxMesh->GetTextureUVCount();

	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	for (size_t i = 0; i < POLYGON_COUNT; i++)
	{
		const int POLYGON_SIZE = fbxMesh->GetPolygonSize(i);
		assert(POLYGON_SIZE <= 4);

		for (size_t j = 0; j < POLYGON_SIZE; j++)
		{
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			FbxModel::VertexPosNormalUv& vertex = vertices[index];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal = FbxDouble4ToVector3(normal);
			}

			if (TEXTURE_UV_COUNT > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;

				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			if (j < 3) { indices.push_back(index); }
			else
			{
				int index2 = indices[indices.size() - 1];
				int index3 = index;
				int index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}
		}
	}
}

void FbxLoader::ParseMaterial(FbxModel* model, FbxNode* fbxNode)
{
	const int MATERIAL_COUNT = fbxNode->GetMaterialCount();
	if (MATERIAL_COUNT > 0)
	{
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		bool textureLoaded = false;

		if (material)
		{
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				model->ambient = FbxDouble3ToVector3(ambient.Get());
				
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				model->diffuse = FbxDouble3ToVector3(diffuse.Get());
			}

			const FbxProperty DIFFUSE_PROPERTY = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (DIFFUSE_PROPERTY.IsValid())
			{
				const FbxFileTexture* TEXTURE = DIFFUSE_PROPERTY.GetSrcObject<FbxFileTexture>();
				if (TEXTURE)
				{
					const char* FILEPATH = TEXTURE->GetFileName();
					string path_str(FILEPATH);
					string name = ExtractFileName(path_str);
					LoadTexture(model, BASE_DIRECTORY + model->name + "/" + name);
					textureLoaded = true;
				}
			}
		}

		if (!textureLoaded) { LoadTexture(model, BASE_DIRECTORY + DEFAULT_TEXTURE_FILE_NAME); }
	}
}

void FbxLoader::LoadTexture(FbxModel* model, const string& FULLPATH)
{
	HRESULT result = S_FALSE;

	TexMetadata& metadata = model->metadata;
	ScratchImage & scratchImg = model->scratchImg;

	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, FULLPATH.c_str(), -1, wfilepath, _countof(wfilepath));
	result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));
}