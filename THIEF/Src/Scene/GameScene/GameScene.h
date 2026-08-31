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

	void CameraCreate(void);		// カメラの作成
	void StageCreate(std::string path, std::string collPath = "NoData");			// ステージの作成
	void WispCreate(void);			// ライトの作成
	void PlayerCreate(void);		// プレイヤーの作成
	void CartCreate(void);			// カートの作成
	void ItemCreateStage1(void);	// ステージ1アイテムの作成
	void ItemCreateStage2(void);	// ステージ2アイテムの作成
	void ItemCreateStage3(void);	// ステージ3アイテムの作成
	void CrosshairCreate(void);		// クロスヘアの作成
	void EnemyCreateStage1(void);	// 敵の作成
	void EnemyCreateStage2(void);	// 敵の作成
	void EnemyCreateStage3(void);	// 敵の作成
private:

	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	// 経路データ
	std::shared_ptr<StagePathData> stagePathData_;

	// ステージ数別の初期化処理
	void Stage1Init(void);
	void Stage2Init(void);
	void Stage3Init(void);

	// タグを使用し、アイテムを作る
	void ItemCreate(Tag tag, VECTOR pos);

	// タグを使用し、敵を作る
	void EnemyCreate(ENEMY_TAG tag, VECTOR pos, const EnemySpawnParam& param = {});
	void InitPathData(void);
};
