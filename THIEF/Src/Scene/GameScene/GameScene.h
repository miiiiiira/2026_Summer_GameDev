#pragma once

#include <DxLib.h>
#include <vector>
#include <string>

#include "../../Object/Tag.h"
#include "../SceneBase.h"

class ObjectManager;
class EnemyManager;
class EnemyBase;

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
	void EnemyCreate(void);			// 敵の作成
	void CartCreate(void);			// カートの作成
	void ItemCreateStage1(void);	// ステージ1アイテムの作成
	void ItemCreateStage2(void);	// ステージ2アイテムの作成
	void ItemCreateStage3(void);	// ステージ3アイテムの作成
	void CrosshairCreate(void);		// クロスヘアの作成

private:

	// ステージ数別の初期化処理
	void Stage1Init(void);
	void Stage2Init(void);
	void Stage3Init(void);

	// 敵の当たり判定処理
	void CheckEnemyAttack(void);	// 敵の攻撃とプレイヤーの当たり判定
	void CollisionEnemyToStage(void);	// 敵とステージの当たり判定
	bool CanStepUp(EnemyBase* enemy, const VECTOR& pos, const VECTOR& move, float stepHeight);	// 小さな段差を登れるか判定する
	void CollisionEnemy2Player(void);	// プレイヤーと敵の当たり判定
	void CollisionEnemy2PlayerGrab(void);	// プレイヤーのつかみとと敵の当たり判定

	void ItemCreate(Tag tag, VECTOR pos);	// タグを使用し、アイテムを作る

private:

	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	EnemyManager* enemyManager_;
};
