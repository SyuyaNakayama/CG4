#include "Stage.h"
#include <cassert>
#include <fstream>

UINT16 Stage::stageNum = static_cast<UINT16>(StageNum::Stage4);

void LoadVector3Stream(std::istringstream& stream, Vector3& vec);

void Stage::Initialize()
{
	// 床のモデル、テクスチャ設定
	floorModel_ = Model::Create("cube");
	std::unique_ptr<Sprite> sprite = Sprite::Create("stages/floor.png");
	sprite->SetSize(sprite->GetSize() / 5.0f);
	floorModel_->SetSprite(std::move(sprite));
	// 囲う壁のモデル
	wallModel_ = Model::Create("cube");
	// ワールド行列初期化
	floorWTrans_.Initialize();
	floorWTrans_.translation = { 0.0f,-2.0f,0.0f };
	for (auto& wallWTrans : wallAroundWTrans_) { wallWTrans.Initialize(); }
	// ステージセット
	LoadMap(stageNum);
}

void Stage::Update()
{
	Sprite* sprite = floorModel_->GetSprite();
	sprite->SetColor({ 1,1,1,1 });
	floorModel_->Update();
	floorWTrans_.Update();
	wallModel_->Update();
	for (auto& wallWTrans : wallAroundWTrans_) { wallWTrans.Update(); }
	for (auto& gimmick : gimmicks_) { gimmick->Update(); }
}

void Stage::Draw()
{
	floorModel_->Draw(floorWTrans_);
	for (auto& wallWTrans : wallAroundWTrans_) { wallModel_->Draw(wallWTrans); }
	for (auto& gimmick : gimmicks_) { gimmick->Draw(); }
}

void Stage::LoadMap(UINT16 stageNum)
{
	// バッファをクリア
	stageCommands_.str("");
	// 状態をクリア
	stageCommands_.clear(std::stringstream::goodbit);
	// ギミックコンテナの中身を空にする
	gimmicks_.clear();
	// ライト関連の変数リセット
	lightIndex = 1;
	Candle::ResetLightNum();
	// 鍵関連の変数リセット
	KeyLock::ResetKeyNum();
	// マップ読み込み
	LoadStageFile(stageNum);
	LoadStageCommands();
}

void Stage::LoadStageFile(UINT16 stageNum)
{
	// ファイル
	std::ifstream file;
	// パスを取得
	std::string stage[static_cast<UINT16>(StageNum::StageNum)] = { "_select", "_tutorial","_alpha" ,"1", "2", "3", "4", "5" };
	const std::string stagefile = "stages/";
	const std::string filename = "stage" + stage[stageNum] + ".txt";
	const std::string directoryPath = "Resources/" + stagefile + filename;
	// ファイルを開く
	file.open(directoryPath);
	assert(file.is_open());
	// ファイルの内容を文字列ストリームにコピー
	stageCommands_ << file.rdbuf();
	// ファイルを閉じる
	file.close();
}

void Stage::LoadStageCommands()
{
	// 1行分の文字列を入れる変数
	std::string line;
	// 引数用変数
	GimmickParam gimmickParam;

	// コマンド実行ループ
	while (getline(stageCommands_, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);
		// 文字列
		std::string word;
		// 空白区切りで行の先頭文字列を取得
		getline(line_stream, word, ' ');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を読み飛ばす
			continue;
		}

		// どのギミックを読み込んだかの判別
		int gimmickType = -1;
		GimmickNum gimmickNum = GimmickNum::None;
		if (word.find("floor") == 0) { gimmickType = 0; }
		else if (word.find("door") == 0) { gimmickType = 1; gimmickNum = GimmickNum::Door; }
		else if (word.find("key") == 0) { gimmickType = 0; gimmickNum = GimmickNum::Key; }
		else if (word.find("candle") == 0) { gimmickType = 2; gimmickNum = GimmickNum::Candle; }
		else if (word.find("wall") == 0) { gimmickType = 3; gimmickNum = GimmickNum::Wall; }
		else if (word.find("start") == 0) { gimmickType = 4; }
		else { continue; } // 何も読み込まれてなければ次へ

		// コマンド読み込み
		LoadStreamCommands(line_stream, word, gimmickParam);

		// 固有処理
		switch (gimmickType)
		{
		case 0: // 床
			// 床のスケールをセット
			floorWTrans_.scale = gimmickParam.scale;
			// 囲う壁のスケール、座標をセット
			// 上
			wallAroundWTrans_[0].scale.x = gimmickParam.scale.x + 2.0f;
			wallAroundWTrans_[0].translation.z = gimmickParam.scale.z + 1.0f;
			// 下
			wallAroundWTrans_[1].scale.x = gimmickParam.scale.x + 2.0f;
			wallAroundWTrans_[1].translation.z = -gimmickParam.scale.z - 1.0f;
			// 右
			wallAroundWTrans_[2].scale.z = gimmickParam.scale.z + 2.0f;
			wallAroundWTrans_[2].translation.x = gimmickParam.scale.x + 1.0f;
			// 左
			wallAroundWTrans_[3].scale.z = gimmickParam.scale.z + 2.0f;
			wallAroundWTrans_[3].translation.x = -gimmickParam.scale.x - 1.0f;
			// ステージサイズをセット
			stageSize_ = { gimmickParam.scale.x, gimmickParam.scale.z };
			continue;
		case 1: // ドア
			doorPos = gimmickParam.pos;
			break;
		case 4: // スタート地点
			// 座標セット
			startPos = gimmickParam.pos;
			continue;
		}

		// ギミック生成
		PopGimmick(gimmickNum, gimmickParam);
	}
}

void Stage::LoadStreamCommands(std::istringstream& stream, std::string& word, GimmickParam& gimmickParam)
{
	// パラメータリセット
	gimmickParam.pos = { 0.0f, 0.0f, 0.0f };
	gimmickParam.scale = { 1.0f, 1.0f, 1.0f };
	gimmickParam.rot = { 0.0f, 0.0f, 0.0f };
	gimmickParam.flag = false;

	// (区切りで先頭文字列を取得
	while (getline(stream, word, '('))
	{
		// 座標取得
		if (word.find("pos") == 0) { LoadVector3Stream(stream, gimmickParam.pos); }
		// スケール取得
		else if (word.find("scale") == 0) { LoadVector3Stream(stream, gimmickParam.scale); }
		// 回転角取得
		else if (word.find("rot") == 0) { stream >> gimmickParam.rot.y; }
		// フラグ取得
		else if (word.find("flag") == 0) { stream >> gimmickParam.flag; }
		// 空白まで飛ばす
		getline(stream, word, ' ');
	}
}

void Stage::PopGimmick(GimmickNum gimmickNum, const GimmickParam& gimmickParam)
{
	// 宣言、生成
	std::unique_ptr<Gimmick> gimmick;
	switch (gimmickNum)
	{
	case GimmickNum::Door:		gimmick = std::make_unique<Door>(doorIndex++);		break;
	case GimmickNum::Key:		gimmick = std::make_unique<KeyLock>();				break;
	case GimmickNum::Candle:	gimmick = std::make_unique<Candle>(lightIndex++);	break;
	case GimmickNum::Wall:		gimmick = std::make_unique<Wall>();					break;
	}

	//初期設定
	gimmick->Initialize(gimmickParam);
	// コンテナにプッシュ
	gimmicks_.push_back(std::move(gimmick));
}