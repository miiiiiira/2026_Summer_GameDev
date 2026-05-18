#pragma once

#include <vector>

#include "../SceneBase.h"

class ObjectManager;
class Goblet;

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
	void PlayerCreate(void);		// プレイヤーの作成
	void LanternCreate(void);		// ランタンの作成
	void EnemyCreate(void);			// 敵の作成
	void StageCreate(void);			// ステージの作成

private:
	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	// アイテム
	Goblet* item_;

	// アイテムとプレイヤーの掴み用当たり判定
	void CheckItemPlayerCollision(void);
	// アイテムとステージの当たり判定
	void CheckItemStageCollision(void);

	void DebugDraw(void);
};
