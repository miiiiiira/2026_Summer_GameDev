#pragma once

#include <vector>

#include "../SceneBase.h"

class ObjectManager;
class Goblet;
class Yeti;
class Crosshair;

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
	void StageCreate(void);			// ステージの作成
	void LanternCreate(void);		// ランタンの作成
	void PlayerCreate(void);		// プレイヤーの作成
	void EnemyCreate(void);			// 敵の作成
	void ItemCreate(void);			// アイテムの作成

private:
	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	Yeti* enemy_;

	// クロスヘア
	Crosshair* crosshair_;

	// ステージ数別の初期化処理
	void Stage1Init(void);
	void Stage2Init(void);
	void Stage3Init(void);

	// 敵の当たり判定処理
	// 敵の攻撃とプレイヤーの当たり判定
	void CheckEnemyAttack(void);

	// 敵とステージの当たり判定
	void CollisionEnemyToStage(void);
};
