#include "Jump.h"
#include "Input.h"
#include "CollisionManager.h"
#include <imgui.h>

void Jump::Start(float jumpV0)
{
	if (isJump) { return; }
	jumpSpd = jumpV0;
	isJump = true;
	//collisionAttribute = CollisionAttribute::Player;
	//collisionMask = CollisionMask::Player;
}

void Jump::Update()
{
	ImGui::Text("isJump : %d", isJump);
	if (!isJump) { return; }
	pwt->translation.y += jumpSpd;
	pwt->Update();
	worldTransform = *pwt;
	jumpSpd -= gravity;
}

// 自分の真下の地面を検出
void Jump::OnCollision(BoxCollider* collider)
{
	// 相手コライダーの上底のy座標を取得
	float pairPosY = collider->GetWorldPosition().y + collider->GetRadius().y;
	// 自座標と相手座標のy軸の差を計算
	float disY = std::abs(worldTransform.translation.y - pairPosY);
	// それが自分のy軸方向の半径未満なら着地している
	if (worldTransform.scale.y >= disY)
	{
		// 落ちてる状態じゃなければ無視
		if (jumpSpd >= 0 || !isJump) { return; }
		// ジャンプ状態解除
		worldTransform.translation.y = collider->GetWorldPosition().y;
		isJump = false;
	}
	else
	{
		Start(0);
	}

	//// 落ちてる状態じゃなければ無視
	//if (jumpSpd >= 0 || !isJump) { return; }
	//// 相手コライダーの上底のy座標を取得
	//float pairPosY = collider->GetWorldPosition().y + collider->GetRadius().y;
	//// めり込みを解消
	//// 地面にめりこんでいるか？
	//if (CollisionManager::CheckCollision2Boxes(this, collider))
	//{
	//	pwt->translation.y = pairPosY;
	//	// ジャンプ状態解除
	//	isJump = false;
	//}
	//// めり込んでなくてジャンプ中でないなら落下
	//else if (jumpSpd < 0)
	//{
	//	Start(0);
	//	isJump = true;
	//}
}
