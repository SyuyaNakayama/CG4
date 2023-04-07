#include "WorldTransform.h"
#include "D3D12Common.h"

ViewProjection* WorldTransform::viewProjection_ = nullptr;

void WorldTransform::Initialize()
{
	CreateBuffer(constBuffer.GetAddressOf(), &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
}

void WorldTransform::Update()
{
	Matrix4 matScale = Matrix4::Scale(scale);
	Matrix4 matRot = Matrix4::Rotate(rotation);
	Matrix4 matTrans = Matrix4::Translate(translation);
	matWorld = matScale * matRot * matTrans;
	if (parent) { matWorld *= parent->matWorld; }

	constMap->world = matWorld;
}

void WorldTransform::Update(const Quaternion& rotQ)
{
	Matrix4 matScale = Matrix4::Scale(scale);
	Matrix4 matRot = rotQ.MakeRotateMatrix();
	Matrix4 matTrans = Matrix4::Translate(translation);
	matWorld = matScale * matRot * matTrans;
	if (parent) { matWorld *= parent->matWorld; }

	constMap->world = matWorld;
}

void WorldTransform::CameraUpdate()
{
	assert(viewProjection_);
	viewProjection_->Update();
}