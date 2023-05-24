#pragma once
#include <string>
#include <vector>
#include <DirectXTex.h>
#include <windows.h>
#include <wrl.h>
#include <d3dx12.h>
#include "fbxsdk.h"
#include "Matrix4.h"

struct Node
{
	std::string name;
	Vector3 scaling = { 1,1,1 },
		rotation,
		translation;
	Matrix4 transform, globalTransform;
	Node* parent = nullptr;
};

class FbxModel
{
public:
	static const int MAX_BONE_INDICES = 4;
	
	struct VertexPosNormalUvSkin
	{
		Vector3 pos, normal;
		Vector2 uv;
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};

	struct Bone
	{
		std::string name;
		Matrix4 invInitialPose;
		FbxCluster* fbxCluster;

		Bone(const std::string& name) { this->name = name; }
	};

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;
	static const string BASE_DIRECTORY;
	static const string DEFAULT_TEXTURE_FILE_NAME;

	ComPtr<ID3D12Resource> vertBuff, indexBuff, texBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};

	string name;
	vector<Node> nodes;
	Node* meshNode = nullptr;
	vector<VertexPosNormalUvSkin> vertices;
	vector<uint16_t> indices;
	Vector3 ambient = { 1,1,1 }, diffuse = { 1,1,1 };
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	VertexPosNormalUvSkin* vertMap = nullptr;
	uint16_t* indexMap = nullptr;
	vector<Bone> bones;
	FbxScene* fbxScene = nullptr;

	void ParseMeshVertices(FbxMesh* fbxMesh);
	void ParseMeshFaces(FbxMesh* fbxMesh);
	void ParseMaterial(FbxNode* fbxNode);
	void LoadTexture(const string& FULLPATH);
	void ParseMesh(FbxNode* fbxNode);
	void ParseSkin(FbxMesh* fbxMesh);
	string ExtractFileName(const string& PATH);

public:
	friend class FbxLoader;

	~FbxModel() { fbxScene->Destroy(); }
	static void ConvertMatrixFromFbx(Matrix4* dst, const FbxAMatrix& src);
	void ParseNodeRecursive(FbxNode* fbxNode, Node* parent = nullptr);
	void CreateBuffers();
	void Draw();
	vector<Bone>& GetBones() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
	const Matrix4& GetModelTransform() { return meshNode->globalTransform; }
};