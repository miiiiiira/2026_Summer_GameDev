#pragma once

#include "../Component.h"
#include "ItemInfo.h"
#include <vector>

// 前方宣言
class Transform;

class Item :public Component
{
protected:

	// アイテムにかける重力
	static constexpr float GRAVITY = -0.25f;

	// 最大落下速度
	static constexpr float MAX_FALL = -15.0f;

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.3f;

	// ダメージ表記用のカウント
	static constexpr int DAMAGE_DRAW_COUNT = 90;

	// 無敵時間
	static constexpr int INVINCIBILITY_FRAMES = 20;
	static constexpr int INVINCIBILITY_FRAMES_ISGRABB = 30;

	// アイテムが壊れる座標
	static constexpr float DEAD_POS_Y = -1000.0f;
public:
	// デストラクタ
	virtual ~Item(void)override;

	// 初期化処理
	void Init(void)override;
	// 更新処理
	void Update(void)override;
	// 描画処理
	void Draw2D(void)override;
	void Draw3D(void)override;

public:

	// Transformを返す
	Transform* GetTransform();

	// アイテムの情報を渡す
	const ItemInfo& GetInfo(void);

	// モデルIDを渡す
	int GetModelID(void) { return info_.modelId_; }

	// カメラとの距離を渡す
	float GetCameraDistance(void);

public:

	// アイテムにダメージを与える(ダメージ数、当たった場所)
	void SetDamage(VECTOR pos);

	// 指定された座標をアイテムの座標に反映
	void SetPos(const VECTOR& pos);

	// 指定された座標をアイテムの前回座標に反映
	void SetPrevPos(const VECTOR& prevPos);

	// ローカル座標を設定
	// Z軸のみ
	void SetLocalPosZ(float localPosZ);

	// 掴まれた状態にする
	void StartGrabbing(VECTOR localPos);

	// 掴まれた状態を終了する
	void EndGrabbed(void);

	// 納品場所に入ったかどうかを変更
	void SetHasTouchedDelivery(bool flg) { info_.hasTouchedDeliveryLocation_ = flg; }

	// かーとに入ったかどうかを変更
	void SetHasTouchedCart(bool flg) { info_.hasTouchedCart_ = flg; }

	// 下方向の加速度を0にする
	void SetVelocityYZero(void) { info_.velocity_.y = 0.0f; }

	// 発見したことにする
	void TrueIsFound(void);

	// 地面についた
	void OnFloor(void);

protected:

	static constexpr int DAMAGE_MULT = 15;

	static constexpr int FONT_SIZE = 21;

	struct DamageInfo
	{
		VECTOR pos = {};
		int damage = 0;
		int count = 0;
	};

	// アイテムの情報
	ItemInfo info_;

	// Transform
	Transform* trans_;

	// ダメージ数
	std::vector<DamageInfo> damageDrawList_;

	// 縁フォント
	int edgeFont_;

	// 重力をかける
	void Gravity(void);

	// アイテムの重み
	void Weight(void);

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// 無敵時間の更新処理
	void UpdateInvincibility(void);

	// 個々のパラメータを設定する
	virtual void SetParam(void) {};

	// 個々の破壊時の処理
	virtual void Break(void) {};

	// 個々のダメージ時の処理
	virtual void Damage(void) {};
	
	// ダメージ表記用のカウントを更新
	void CountUpdate(void);

	// デバッグ用の描画
	void DrawDebug(void);
};

