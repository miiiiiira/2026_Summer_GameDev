#pragma once
#include <vector>
#include <DxLib.h>
#include "SceneBase.h"

class ObjectManager;

class DebugScene : public SceneBase
{
public:
	enum class EditMode
	{
		ENEMY_NODE_POINT,
		ITEM_NODE_POINT,
		SPAWN_POINT,
		MAX
	};

	DebugScene(void);				// コンストラクタ
	~DebugScene(void) override;		// デストラクタ

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

	struct Point
	{
		int id;
		VECTOR pos;
	};

	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	// デバッグポイント群
	std::vector<Point> points_;

	EditMode edit_;

	int pointNum_;
	int time_;

	// デバッグポイントの配置
	void PlaceDebugPoint(void);
	void PlaceEnemyNodePoint(void);
	void PlaceItemNodePoint(void);
	void PlaceSpawnPoint(void);

	// デバッグポイントの保存
	void SavePoints(void);
	void SaveEnemyNodePoints(void);
	void SaveItemNodePoints(void);
	void SaveSpawnPoints(void);

	// デバックポイントの読み込み
	void LoadPoints(void);



	void DebugDraw(void);
};
