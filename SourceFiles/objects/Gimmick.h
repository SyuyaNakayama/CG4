#pragma once
#include "Collider.h"
#include "Model.h"
#include "ParticleManager.h"
#include "Input.h"
#include "Collider.h"
#include "Player.h"

// ギミックのパラメータ
struct GimmickParam {
	Vector3 pos;		// 座標
	Vector3 scale;		// スケール
	Vector3 rot;		// 回転
	int flag = false;	// フラグ
};

class Gimmick : public BoxCollider
{
protected:
	std::unique_ptr<Model> model;
	static bool isStart_;	// trueになったらカメラを引く
	static LightGroup* lightGroup;

public:
	virtual ~Gimmick() { model.release(); }
	virtual void Initialize(const GimmickParam& param);
	virtual void Update() = 0;
	virtual void Draw() { model->Draw(worldTransform); }

	// アクセッサ
	Vector3 GetRotation() { return worldTransform.rotation; }

	static bool GetIsStart() { return isStart_; }
	static void SetIsStart(bool isStart) { isStart_ = isStart; }
};

class Door : public Gimmick
{
private:
	enum class WTType { L, R };

	// ドアが閉じている時にnullptrになる
	void (Door::* Move)() = &Door::Opened;

	UINT16 doorIndex = 0;
	std::unique_ptr<Model> model_back;
	std::array<WorldTransform, 2> door;
	Input* input = Input::GetInstance();
	float rot = 90;

	void Open();	// ドアが開く時に呼び出される関数
	void Close();	// ドアが閉じる時に呼び出される関数
	void Opened();	// ドアが開いている時に呼び出される関数
	void Closed();	// ドアが閉じている時に呼び出される関数
	void OnCollision(BoxCollider* boxCollider);
public:
	// 引数付きコンストラクタ
	Door(UINT16 doorIndex_) { doorIndex = doorIndex_; }
	void Initialize(const GimmickParam& param);
	void Update();
	void Draw() override;

	//当たり判定の大きさを調整
	Vector3 GetRadius() { return { 1.8f,1.9f,1.0f }; }
};

class KeyLock : public Gimmick
{
private:
	// 全ての鍵の数
	static size_t keyNum;
	// 収集した鍵の数
	static size_t collectKeyNum;
	// 鍵を全て集めたか
	static bool isCollectAll;

	// 収集済みかどうか
	bool isCollect = false;

	// 当たり判定
	void OnCollision(BoxCollider* boxCollider);
public:
	void Initialize(const GimmickParam& param);
	void Update();
	void Draw() override;
	static bool GetIsCollectAll() { return isCollectAll; }
	static void ResetKeyNum() { keyNum = 0; collectKeyNum = 0; }
};

class Candle : public Gimmick, public SphereCollider
{
private:
	void (Candle::* Fire)() = &Candle::Dark;

	bool isLight = false;
	size_t lightIndex = 0;
	DiffuseParticle::AddProp particleProp;
	Vector3 lightPos;
	static size_t lightNum;
	int particleTimer = 60; // 方向パーティクル発生時間
	Vector3 playerPos;
	Sprite* ui = nullptr;

	void Dark(); // 光っていない時の処理
	void PreLight(); // 光る前
	void PostLight(); // 光った後
public:
	// 引数付きコンストラクタ
	Candle(size_t index) { lightIndex = index; lightNum++; }
	void OnCollision(RayCollider* rayCollider);
	void Initialize(const GimmickParam& param);
	void Update();
	static size_t GetLightNum() { return lightNum; }
	static void ResetLightNum() { lightNum = 0; }
};

class Wall : public Gimmick
{
public: // 列挙クラス
	// 壁のステータス
	enum class WallStatus {
		NORMAL = 0b000,
		MOVE = 0b001,
		VANISH_RED = 0b010,
		VANISH_BLUE = 0b100,
	};
private:
	// 壁の状態
	int wallState = 0;
	// フラグ
	bool isMove = false;
	// プレイヤー
	static Player* player;

	float speed = 0.1f;
	int interval = 0;

public:
	static void SetPlayerAddress(Player* pPlayer) { player = pPlayer; }
	void Initialize(const GimmickParam& param);
	void Update();
	void Draw() override;
	void Move();
};