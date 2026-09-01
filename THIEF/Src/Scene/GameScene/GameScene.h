#pragma once

#include <DxLib.h>
#include <vector>
#include <string>
#include <memory>

#include "../../Object/Tag.h"
#include "../SceneBase.h"
#include "../../Object/Actor/Enemy/EnemyCommon.h"

class ObjectManager;

enum STAGE_NUM
{
	STAGE_1,
	STAGE_2,
	STAGE_3,

	STAGE_MAX,
};

class GameScene : public SceneBase
{
public:

	GameScene(void);				// コンストラクタ
	~GameScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// ステージ別の初期化処理
	void Stage1Init(void);	// ステージ1
	void Stage2Init(void);	// ステージ2
	void Stage3Init(void);	// ステージ3

	void CameraCreate(void);	// カメラの作成

	void StageCreate(std::string path, std::string collPath = "NoData");	// ステージの作成

	void WispCreate(void);	// ライトの作成

	void PlayerCreate(void);	// プレイヤーの作成

	void CartCreate(void);	// カートの作成

	// ステージ別アイテムの生成
	void ItemCreateStage1(void);	// ステージ1
	void ItemCreateStage2(void);	// ステージ2
	void ItemCreateStage3(void);	// ステージ3

	void CrosshairCreate(void);	// クロスヘアの作成

	// ステージ別敵の生成
	void EnemyCreateStage1(void);	// ステージ1
	void EnemyCreateStage2(void);	// ステージ2
	void EnemyCreateStage3(void);	// ステージ3

	void ItemCreate(Tag tag, VECTOR pos);	// タグを使用し、アイテムを作る
	void EnemyCreate(ENEMY_TAG tag, VECTOR pos, const EnemySpawnParam& param = {});	// タグを使用し、敵を作る
	void InitPathData(void);

private:

	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	// 経路データ
	std::shared_ptr<StagePathData> stagePathData_;
};
