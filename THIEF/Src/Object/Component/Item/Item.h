#pragma once

#include "../Component.h"
#include "ItemInfo.h"

// 前方宣言
class Transform;
class PlayerController;

class Item :public Component
{
protected:

	// アイテムにかける重力
	static constexpr float GRAVITY = -0.25f;

	// 最大落下速度
	static constexpr float MAX_FALL = -15.0f;

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.3f;

public:
	// コンストラクタ
	Item();
	// デストラクタ
	virtual ~Item(void);

	// 初期化処理
	virtual void Init(void) = 0;
	// 更新処理
	void Update(void)override;
	// 描画処理
	void Draw(void)override;

	void SetPlayerCameraInfo(PlayerController* player, VECTOR* cameraPos, VECTOR* cameraAngle);

public:

	// Transformを返す
	Transform* GetTransform();

	// アイテムの情報を渡す
	const ItemInfo& GetInfo(void);

	// モデルIDを渡す
	int GetModelID(void) { return info_.modelId_; }

	VECTOR GetLineStartPos(void);
	VECTOR GetLineEndPos(void);
	float GetCameraDistance(VECTOR pos);

public:

	// アイテムにダメージを与える
	void SetDamage(int damage);

	// 指定された座標をアイテムの座標に反映
	void SetPos(const VECTOR& pos);

	// ローカル座標を設定
	// Z軸のみ
	void SetLocalPosZ(float localPosZ);

	// 掴まれた状態にする
	void StartGrabbing(VECTOR localPos);

	// 掴まれた状態を終了する
	void EndGrabbed(void);

	// ステージに接触したため
	void TrueHasTouchedStage(void) { info_.hasTouchedStage_ = true; }

protected:

	// アイテムの情報
	ItemInfo info_;

	// プレイヤー
	PlayerController* player_;

	// カメラの座標
	VECTOR* cameraPos_;

	// カメラの向き
	VECTOR* cameraAngle_;

	// 重力をかける
	void Gravity(void);

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// カメラ情報でローカル座標を回転させる
	VECTOR ToCameraLocalPosRot(void);

	// 方向から回転行列を算出
	MATRIX AngleToMatrix(void);

	// カメラの回転行列を取得
	MATRIX CameraMatrix(void);

	// デバッグ用の描画
	void DrawDebug(void);
};

