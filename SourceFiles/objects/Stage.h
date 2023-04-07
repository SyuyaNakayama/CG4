#pragma once
#include <sstream>
#include "Gimmick.h"

class Stage
{
public: // 列挙クラス
	/// <summary>
	/// ギミックの番号
	/// </summary>
	enum class GimmickNum {
		None,
		Door,
		Key,
		Candle,
		Wall,
		GimmickKinds
	};

	/// <summary>
	/// ステージの番号
	/// </summary>
	enum class StageNum {
		Select,		// セレクト
		Tutorial,	// チュートリアル
		Alpha,		//アルファ版
		Stage1,		// ステージ1
		Stage2,		// ステージ2
		Stage3,		// ステージ3
		Stage4,		// ステージ4
		Stage5,		// ステージ5
		StageNum,	// ステージ数
	};

private:
	// 床
	std::unique_ptr<Model> floorModel_;
	WorldTransform floorWTrans_;
	// 周りの壁
	std::unique_ptr<Model> wallModel_;
	WorldTransform wallAroundWTrans_[4];
	size_t lightIndex = 1;
	size_t doorIndex = 1;
	// ギミック
	std::vector<std::unique_ptr<Gimmick>> gimmicks_;
	// ファイルコマンド
	std::stringstream stageCommands_;
	// ステージ横幅、縦幅
	Vector2 stageSize_;
	// ドア座標
	Vector3 doorPos;
	// スタート地点
	Vector3 startPos;
	// ステージ番号
	static UINT16 stageNum;
	
	// ステージマップ読み込み
	void LoadMap(UINT16 stageNum);
	// ステージファイル読み込み
	void LoadStageFile(UINT16 stageNum);
	// コマンド読み込み
	void LoadStageCommands();
	// ギミック生成
	void PopGimmick(GimmickNum gimmickNum, const GimmickParam& gimmickParam);
	// ストリームコマンド読み込み
	void LoadStreamCommands(std::istringstream& stream, std::string& word, GimmickParam& gimmickParam);
public:
	// デストラクタ
	~Stage() { floorModel_.release(); }
	void Initialize();
	void Update();
	void Draw();

	// アクセッサ
	Vector2 GetStageSize() { return stageSize_; }
	Vector3 GetStartPos() { return startPos; }
	Vector3 GetDoorPos() { return doorPos; }
	
	static UINT16 GetStageNum() { return stageNum; }
	static void SetStageNum(UINT16 stageNum_) { stageNum = stageNum_; }
};