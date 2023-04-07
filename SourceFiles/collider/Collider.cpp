#include "Collider.h"
#include "CollisionManager.h"
#include <cassert>

float IncludeCollider::includeRadius = 0.1f;

BoxCollider::BoxCollider() { CollisionManager::PushCollider(this); }
BoxCollider::~BoxCollider() { CollisionManager::PopCollider(this); }
SphereCollider::SphereCollider() { CollisionManager::PushCollider(this); }
SphereCollider::~SphereCollider() { CollisionManager::PopCollider(this); }
RayCollider::RayCollider() { CollisionManager::PushCollider(this); }
RayCollider::~RayCollider() { CollisionManager::PopCollider(this); }
PolygonCollider::PolygonCollider() { CollisionManager::PushCollider(this); }
PolygonCollider::~PolygonCollider() { CollisionManager::PopCollider(this); }
PlaneCollider::PlaneCollider() { CollisionManager::PushCollider(this); }
PlaneCollider::~PlaneCollider() { CollisionManager::PopCollider(this); }
IncludeCollider::IncludeCollider() { CollisionManager::PushCollider(this); }
IncludeCollider::~IncludeCollider() { CollisionManager::PopCollider(this); }

void PolygonCollider::SetVertices()
{
	Vector3 objPos = worldTransform.translation;
	Vector3 objRad = worldTransform.scale;
	vertices.clear();
	vertices.push_back(objPos + Vector3(-objRad.x, objRad.y, -objRad.z));
	vertices.push_back(objPos + Vector3(objRad.x, objRad.y, -objRad.z));
	vertices.push_back(objPos + Vector3(objRad.x, -objRad.y, -objRad.z));
	vertices.push_back(objPos - objRad);
}

void PolygonCollider::ComputeNormal()
{
	assert(vertices.size() >= 3);
	// –@ü‚ÌŒvŽZ
	Vector3 vec1 = vertices[2] - vertices[0];
	Vector3 vec2 = vertices[1] - vertices[0];
	normal = Normalize(Cross(vec1, vec2));
}

void PolygonCollider::ToPlaneCollider(PlaneCollider* planeCollider)
{
	planeCollider->SetDistance(distance);
	planeCollider->SetRotation(worldTransform.rotation);
	planeCollider->SetBaseNormal(baseNormal);
}

void PolygonCollider::UpdateVertices()
{
	for (Vector3& vertex : vertices) { vertex *= worldTransform.matWorld; }
}

void MeshCollider::ConstructTriangles(Model* model)
{
	/*triangles.clear();

	const Mesh& mesh = model->GetMesh();

	int start = 0;

	const std::vector<Mesh::VertexData>& vertices = mesh.GetVertices();
	const std::vector<unsigned short>& indices = mesh.GetIndices();

	size_t triangleNum = indices.size() / 3;

	triangles.resize(triangles.size() + triangleNum);

	for (int i = 0; i < triangleNum; i++) {

		PolygonCollider& tri = triangles[start + i];
		int idx0 = indices[i * 3 + 0];
		int idx1 = indices[i * 3 + 1];
		int idx2 = indices[i * 3 + 2];

		tri.AddVertices(vertices[idx0].pos);
		tri.AddVertices(vertices[idx1].pos);
		tri.AddVertices(vertices[idx2].pos);

		tri.ComputeNormal();
	}*/
}
