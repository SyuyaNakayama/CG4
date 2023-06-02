#include "FbxModel.h"
#include "D3D12Common.h"
#include "SpriteCommon.h"

const std::string FbxModel::BASE_DIRECTORY = "Resources/";
const std::string FbxModel::DEFAULT_TEXTURE_FILE_NAME = "white1x1.png";

void FbxModel::ConvertMatrixFromFbx(Matrix4* dst, const FbxAMatrix& src)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
		{
			dst->m[i][j] = (float)src.Get(i, j);
		}
	}
}

Vector3 FbxDouble3ToVector3(const FbxDouble3& num1)
{
	return Vector3((float)num1[0], (float)num1[1], (float)num1[2]);
}
Vector3 FbxDouble4ToVector3(const FbxDouble4& num1)
{
	return Vector3((float)num1[0], (float)num1[1], (float)num1[2]);
}

void FbxModel::ParseNodeRecursive(FbxNode* fbxNode, Node* parent)
{
	nodes.emplace_back();
	Node& node = nodes.back();
	node.name = fbxNode->GetName();

	FbxDouble3 rotation = fbxNode->LclRotation.Get(),
		scaling = fbxNode->LclScaling.Get(),
		translation = fbxNode->LclTranslation.Get();

	node.rotation = FbxDouble3ToVector3(rotation) * PI / 180.0f;
	node.scaling = FbxDouble3ToVector3(scaling);
	node.translation = FbxDouble3ToVector3(translation);

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
			meshNode = &node;
			ParseMesh(fbxNode);
		}
	}

	for (size_t i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(fbxNode->GetChild(i), &node);
	}
}

void FbxModel::ParseMesh(FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	ParseMeshVertices(fbxMesh);
	ParseMeshFaces(fbxMesh);
	ParseMaterial(fbxNode);
	ParseSkin(fbxMesh);
}

std::string FbxModel::ExtractFileName(const string& PATH)
{
	size_t pos1 = PATH.rfind('\\');
	if (pos1 != string::npos) { return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1); }

	pos1 = PATH.rfind('/');
	if (pos1 != string::npos) { return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1); }

	return PATH;
}

void FbxModel::ParseMeshVertices(FbxMesh* fbxMesh)
{
	const int CONTROL_POINTS_COUNT = fbxMesh->GetControlPointsCount();

	FbxModel::VertexPosNormalUvSkin vert{};
	vertices.resize(CONTROL_POINTS_COUNT, vert);

	FbxVector4* pCoord = fbxMesh->GetControlPoints();

	for (size_t i = 0; i < CONTROL_POINTS_COUNT; i++)
	{
		FbxModel::VertexPosNormalUvSkin& vertex = vertices[i];
		vertex.pos = FbxDouble4ToVector3(pCoord[i]);
	}
}

void FbxModel::ParseMeshFaces(FbxMesh* fbxMesh)
{
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

			FbxModel::VertexPosNormalUvSkin& vertex = vertices[index];
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

void FbxModel::ParseMaterial(FbxNode* fbxNode)
{
	const int MATERIAL_COUNT = fbxNode->GetMaterialCount();
	if (MATERIAL_COUNT > 0)
	{
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		bool textureLoaded = false;

		if (material)
		{
			string name = material->GetName(); // マテリアル名(デバッグ用)

			// ベースカラー
			const FbxProperty PROP_BASE_COLOR = FbxSurfaceMaterialUtils::GetProperty("baseColor", material);
			if (PROP_BASE_COLOR.IsValid())
			{
				// プロパティの値読み取り
				Vector3 baseColor = FbxDouble3ToVector3(PROP_BASE_COLOR.Get<FbxDouble3>());
				ColorRGB baseColorRGB = { baseColor.x,baseColor.y,baseColor.z };
				this->baseColor = baseColorRGB; // 読み取った値を書き込む
				// テクスチャ読み込み
				const FbxFileTexture* texture = PROP_BASE_COLOR.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&baseTexture, BASE_DIRECTORY + name + "/" + name);
					baseColor = {};
					textureLoaded = true;
				}
			}

			// 金属度
			const FbxProperty PROP_METALNESS = FbxSurfaceMaterialUtils::GetProperty("metalness", material);
			if (PROP_METALNESS.IsValid())
			{
				metalness = PROP_METALNESS.Get<float>();
				// テクスチャ読み込み
				const FbxFileTexture* texture = PROP_METALNESS.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&metalnessTexture, BASE_DIRECTORY + name + "/" + name);
					metalness = 0.0f;
				}
			}

			// 隙間
			const FbxProperty PROP_SPECULAR = FbxSurfaceMaterialUtils::GetProperty("specular", material);
			if (PROP_SPECULAR.IsValid()) { specular = PROP_SPECULAR.Get<float>(); }

			// 粗さ
			const FbxProperty PROP_SPECULAR_ROUGHNESS = FbxSurfaceMaterialUtils::GetProperty("specularRoughness", material);
			if (PROP_SPECULAR_ROUGHNESS.IsValid())
			{
				roughness = PROP_SPECULAR_ROUGHNESS.Get<float>();
				// テクスチャ読み込み
				const FbxFileTexture* texture = PROP_SPECULAR_ROUGHNESS.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filepath = texture->GetFileName();
					// ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtractFileName(path_str);
					// テクスチャ読み込み
					LoadTexture(&roughnessTexture, BASE_DIRECTORY + name + "/" + name);
					roughness = 0.0f;
				}
			}
		}

		if (!textureLoaded) { LoadTexture(&baseTexture, BASE_DIRECTORY + DEFAULT_TEXTURE_FILE_NAME); }
	}
}

void FbxModel::LoadTexture(TextureData* texData, const string& FULLPATH)
{
	HRESULT result = S_FALSE;

	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, FULLPATH.c_str(), -1, wfilepath, _countof(wfilepath));
	result = LoadFromWICFile(wfilepath, DirectX::WIC_FLAGS_NONE, &texData->metadata, texData->scratchImg);
	assert(SUCCEEDED(result));
}

void FbxModel::ParseSkin(FbxMesh* fbxMesh)
{
	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	// スキニング情報がなければ終了
	if (fbxSkin == nullptr)
	{
		// 各頂点についての処理
		for (int i = 0; i < vertices.size(); i++)
		{
			// 最初のボーン(単位行列)の影響100%にする
			vertices[i].boneIndex[0] = 0;
			vertices[i].boneWeight[0] = 1.0f;
		}
		return;
	}

	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		const char* boneName = fbxCluster->GetLink()->GetName();

		bones.emplace_back(FbxModel::Bone(boneName));
		FbxModel::Bone& bone = bones.back();
		bone.fbxCluster = fbxCluster;

		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		Matrix4 initialPose;
		ConvertMatrixFromFbx(&initialPose, fbxMat);

		bone.invInitialPose = Matrix4::Inverse(initialPose);
	}

	struct WeightSet
	{
		UINT index;
		float weight;
	};

	std::vector<std::list<WeightSet>> weightLists(vertices.size());

	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			int vertIndex = controlPointIndices[j];
			float weight = (float)controlPointWeights[j];

			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		auto& weightList = weightLists[i];

		weightList.sort([](auto const& lhs, auto const rhs) { return lhs.weight > rhs.weight; });

		int weightArrayIndex = 0;

		for (auto& weightSet : weightList)
		{
			vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;

			if (++weightArrayIndex < FbxModel::MAX_BONE_INDICES) { continue; }

			float weight = 0.0f;
			for (int j = 1; j < FbxModel::MAX_BONE_INDICES; j++)
			{
				weight += vertices[i].boneWeight[j];
			}
			vertices[i].boneWeight[0] = 1.0f - weight;
			break;
		}
	}
}

void FbxModel::CreateBuffers()
{
#pragma region 頂点バッファ設定
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin) * vertices.size());
	CreateBuffer(&vertBuff, &vertMap, sizeVB);
	std::copy(vertices.begin(), vertices.end(), vertMap);

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);
#pragma endregion
#pragma region インデックスバッファ設定
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());
	CreateBuffer(&indexBuff, &indexMap, sizeIB);
	std::copy(indices.begin(), indices.end(), indexMap);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
#pragma endregion
	baseTexture.CreateTexture(0);
	metalnessTexture.CreateTexture(1);
	roughnessTexture.CreateTexture(2);

	// 定数バッファ生成
	CreateBuffer(&constBuffMaterial, &constMapMaterial, (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff);
	TransferMaterial();
}

void FbxModel::Draw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	ID3D12DescriptorHeap* descHeap = SpriteCommon::GetDescriptorHeap();
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->SetGraphicsRootDescriptorTable(1, descHeap->GetGPUDescriptorHandleForHeapStart());
	cmdList->SetGraphicsRootConstantBufferView(3, constBuffMaterial->GetGPUVirtualAddress());
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

void FbxModel::TransferMaterial()
{
	// 定数バッファへデータ転送
	constMapMaterial->baseColor = baseColor;
	constMapMaterial->metalness = metalness;
	constMapMaterial->specular = specular;
	constMapMaterial->roughness = roughness;
}

void TextureData::CreateTexture(int srvIndex)
{
	// テクスチャデータがない場合はなにもせずに終了
	if (scratchImg.GetImageCount() == 0) { return; }

	Result result;
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
	assert(img);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height,
		(UINT16)metadata.arraySize, (UINT16)metadata.mipLevels);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE, &texresDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&texBuff));

	result = texBuff->WriteToSubresource(0, nullptr, img->pixels, (UINT)img->rowPitch, (UINT)img->slicePitch);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(), &srvDesc,
		SpriteCommon::GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	// GPUハンドル取得
	ID3D12DescriptorHeap* descHeapSRV = SpriteCommon::GetDescriptorHeap();
	gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(), srvIndex,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
}