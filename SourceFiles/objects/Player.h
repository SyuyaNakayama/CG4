#pragma once
#include "Input.h"
#include "Model.h"
#include "Camera.h"
#include "Jump.h"
#include "Timer.h"

class Player : public BoxCollider, public RayCollider
{
private:
	const int MAX_HP = 4000;

	enum class PartId { body, legR, legL };

	Input* input_;
	std::array<WorldTransform, 3> modelsTrans_;
	std::array<std::unique_ptr<Model>, 3> model_;
	Camera eyeCamera;
	bool isCameraChange = false;
	// ライト
	LightGroup* lightGroup_;
	// 前フレーム座標
	Vector3 prePos;
	//モーション
	bool isUp = true;
	Timer timerStandby = 50;
	Timer timerWalk = 20;
	int walkNum = 0;
	// ステージ横幅、縦幅
	Vector2 stageSize;
	int hp = MAX_HP;
	Jump jump;
	Sprite* hpUI;

	void (Player::* State)() = nullptr;
	void StandbyMotion();
	void WalkMotion();
	void Move();
	// ライト処理
	void (Player::* LightUpdate)() = &Player::RedFire;
	void RedFire();
	void BlueFire();

	void ObjectUpdate();

public:
	void Initialize(const Vector3& startPos);
	void Update();
	void Draw();

	// アクセッサ
	WorldTransform GetWorldTransform() { return worldTransform; }
	void SetStageSize(Vector2 size) { stageSize = { size.x - 1.0f, size.y - 1.0f }; }
	bool IsCameraChange() { return isCameraChange; }
	bool IsBlueFire() { return LightUpdate == &Player::BlueFire; }

	// 当たり判定の処理
	void OnCollision(BoxCollider* boxCollider) override;
	//void OnCollision(SphereCollider* sphereCollider) override;
	//void OnCollision(PlaneCollider* boxCollider) override;
	//void OnCollision(PolygonCollider* sphereCollider) override;
	//void OnCollision(RayCollider* sphereCollider) override;
	//void OnCollision(IncludeCollider* sphereCollider) override;
};

