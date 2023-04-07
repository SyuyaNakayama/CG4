#pragma once
#include "Collider.h"
#include <list>

class CollisionManager final
{
private:
	static std::list<BoxCollider*> boxColliders;
	static std::list<IncludeCollider*> includeColliders;
	static std::list<SphereCollider*> sphereColliders;
	static std::list<PlaneCollider*> planeColliders;
	static std::list<PolygonCollider*> polygonColliders;
	static std::list<RayCollider*> rayColliders;

public:
	// 個別当たり判定
	static bool CheckCollisionFiltering(BaseCollider* colliderA, BaseCollider* colliderB);
	static bool CheckCollision2Boxes(BoxCollider* colliderA, BoxCollider* colliderB);
	static bool CheckCollision2IncludeObjects(IncludeCollider* colliderA, IncludeCollider* colliderB);
	static bool CheckCollision2Spheres(SphereCollider* colliderA, SphereCollider* colliderB);
	static bool CheckCollisionSpherePlane(SphereCollider* colliderA, PlaneCollider* colliderB, Vector3* inter = nullptr);
	static bool CheckCollisionSpherePolygon(SphereCollider* colliderA, PolygonCollider* colliderB, Vector3* inter = nullptr);
	static bool CheckCollisionRayPlane(RayCollider* colliderA, PlaneCollider* colliderB, float* distance = nullptr);
	static bool CheckCollisionRayPolygon(RayCollider* colliderA, PolygonCollider* colliderB, float* distance = nullptr);
	static bool CheckCollisionRaySphere(RayCollider* colliderA, SphereCollider* colliderB, float* distance = nullptr, Vector3* inter = nullptr);
	static bool CheckCollisionRayBox(RayCollider* colliderA, BoxCollider* colliderB);

	CollisionManager() = delete;
	// コライダー登録関数
	static void PushCollider(BoxCollider* collider) { boxColliders.push_back(collider); }
	static void PushCollider(IncludeCollider* collider) { includeColliders.push_back(collider); }
	static void PushCollider(SphereCollider* collider) { sphereColliders.push_back(collider); }
	static void PushCollider(PlaneCollider* collider) { planeColliders.push_back(collider); }
	static void PushCollider(PolygonCollider* collider) { polygonColliders.push_back(collider); }
	static void PushCollider(RayCollider* collider) { rayColliders.push_back(collider); }
	static void PopCollider(BoxCollider* collider) { boxColliders.remove(collider); }
	static void PopCollider(IncludeCollider* collider) { includeColliders.remove(collider); }
	static void PopCollider(SphereCollider* collider) { sphereColliders.remove(collider); }
	static void PopCollider(PlaneCollider* collider) { planeColliders.remove(collider); }
	static void PopCollider(PolygonCollider* collider) { polygonColliders.remove(collider); }
	static void PopCollider(RayCollider* collider) { rayColliders.remove(collider); }
	// 種類別当たり判定
	static void CheckBoxCollisions();
	static void CheckIncludeCollisions();
	static void CheckSphereCollisions();
	static void CheckSpherePlaneCollisions();
	static void CheckSpherePolygonCollisions();
	static void CheckRayPlaneCollisions();
	static void CheckRayPolygonCollisions();
	static void CheckRaySphereCollisions();
	static void CheckRayBoxCollisions();
	static void CheckRayCastCollision(RayCollider* collider);
	// 全当たり判定
	static void CheckAllCollisions();
};