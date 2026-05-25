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
	// デストラクタ
	virtual ~Item(void)override;

	// 初期化処理
	void Init(void)override;
	// 更新処理
	void Update(void)override;
	// 描画処理
	void Draw(void)override;

	// プレイヤー設定
	void SetPlayer(PlayerController* player);

public:

	// Transformを返す
	Transform* GetTransform();

	// アイテムの情報を渡す
	const ItemInfo& GetInfo(void);

	// モデルIDを渡す
	int GetModelID(void) { return info_.modelId_; }

	// カメラとの距離を渡す
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

	// ステージに接触したためフラグを立てる
	void TrueHasTouchedStage(void) { info_.hasTouchedStage_ = true; }

	// 納品場所に入ったかどうかを変更
	void TrueHasTouchedDelivery(bool flg) { info_.hasTouchedDeliveryLocation_ = flg; }

protected:

	// アイテムの情報
	ItemInfo info_;

	// Transform
	Transform* trans_;

	// プレイヤー
	PlayerController* player_;

	// 重力をかける
	void Gravity(void);

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// 個々のパラメータを設定する
	virtual void SetParam(void) = 0;

	// デバッグ用の描画
	void DrawDebug(void);
};

