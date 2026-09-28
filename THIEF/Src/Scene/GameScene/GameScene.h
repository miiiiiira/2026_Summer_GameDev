#pragma once

#include <vector>
#include <string>
#include <memory>
#include <DxLib.h>

#include "../../Object/Tag.h"
#include "../SceneBase.h"
#include "../../Object/Component/Enemy/EnemyCommon.h"

class ObjectManager;

// ステージ数
enum STAGE_NUM
{
	STAGE_1,	// ステージ1
	STAGE_2,	// ステージ2
	STAGE_3,	// ステージ3

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

	// オブジェクトマネージャー
	ObjectManager* objectManger_;

private:

	// 経路データ
	std::shared_ptr<StagePathData> stagePathData_;

private:

	// ステージ別の初期化処理
	void Stage1Init(void);	// ステージ1
	void Stage2Init(void);	// ステージ2
	void Stage3Init(void);	// ステージ3

	// カメラの作成
	void CameraCreate(void);	

	// ステージの作成
	void StageCreate(std::string path, std::string collPath = "NoData");	

	// ライトの作成
	void WispCreate(void);	

	// プレイヤーの作成
	void PlayerCreate(void);	

	// カートの作成
	void CartCreate(void);	

	// ステージ別アイテムの生成
	void ItemCreateStage1(void);	// ステージ1
	void ItemCreateStage2(void);	// ステージ2
	void ItemCreateStage3(void);	// ステージ3

	void CrosshairCreate(void);	// クロスヘアの作成

	// ステージ別敵の生成
	void EnemyCreateStage1(void);	// ステージ1
	void EnemyCreateStage2(void);	// ステージ2
	void EnemyCreateStage3(void);	// ステージ3

	// タグを使用し、アイテムを作る
	void ItemCreate(Tag tag, VECTOR pos);	

	// タグを使用し、敵を作る
	void EnemyCreate(ENEMY_TAG tag, VECTOR pos, const EnemySpawnParam& param = {});	
	void InitPathData(void);
};
