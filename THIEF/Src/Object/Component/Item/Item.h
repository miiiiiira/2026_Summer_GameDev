#pragma once

#include "../Component.h"
#include "ItemInfo.h"
#include "../../../Common/Math/Vector2.h"
#include <vector>

// 前方宣言
class Transform;

class Item :public Component
{
protected:

	// リミット設定
	static constexpr int DAMAGE_DRAW_COUNT = 90;	// ダメージ表記用のカウント
	static constexpr int INVINCIBILITY_FRAMES = 20;			// 初期無敵時間
	static constexpr int INVINCIBILITY_FRAMES_ISGRABB = 30;	// ダメージ時の無敵時間
	static constexpr float DEAD_POS_Y = -1000.0f;	// アイテムが壊れる座標

	// 重力
	static constexpr float GRAVITY = -0.25f;	// アイテムにかける重力
	static constexpr float MAX_FALL = -15.0f;	// 最大落下速度
	
	static constexpr float COEFFICIENT = 0.3f;	// 線形補間の係数

public:
	
	virtual ~Item(void)override;	// デストラクタ

	void Init(void)override;		// 初期化
	void Update(void)override;		// 更新
	void Draw2D(void)override;		// 2D描画
	void Draw3D(void)override;		// 3D描画

public:

	Transform* GetTransform();	// Transformを返す

	const ItemInfo& GetInfo(void);	// アイテムの情報を渡す

	int GetModelID(void) { return info_.modelId_; }	// モデルIDを渡す
	
	float GetCameraDistance(void);	// カメラとの距離を渡す

public:

	void SetDamage(VECTOR pos);	// アイテムにダメージを与える(ダメージ数、当たった場所)
	
	void SetPos(const VECTOR& pos);	// 指定された座標をアイテムの座標に反映
	
	void SetPrevPos(const VECTOR& prevPos);	// 指定された座標をアイテムの前回座標に反映

	void SetLocalPosZ(float localPosZ);	// ローカル座標を設定 Z軸のみ
	
	void StartGrabbing(VECTOR localPos);	// 掴まれた状態にする

	void EndGrabbed(void);	// 掴まれた状態を終了する

	void SetHasTouchedDelivery(bool flg) { info_.hasTouchedDeliveryLocation_ = flg; }	// 納品場所に入ったかどうかを変更
	
	void SetHasTouchedCart(bool flg) { info_.hasTouchedCart_ = flg; }	// カートに入ったかどうかを変更

	void SetVelocityYZero(void) { info_.velocity_.y = 0.0f; }	// 下方向の加速度を0にする

	void TrueIsFound(void);	// 発見したことにする
	
	void OnFloor(void);	// 地面についた

protected:

	virtual void SetParam(void) {};	// 個々のパラメータを設定する
	virtual void Break(void) {};	// 個々の破壊時の処理
	virtual void Damage(void) {};	// 個々のダメージ時の処理

	void IsNotAliveTutorial(void);	// 生存していない場合　チュートリアル時の生成処理

	void Gravity(void);	// 重力をかける
	
	void Weight(void);	// アイテムの重み
	
	void TrackingPlayer(void);	// プレイヤーの位置をみて移動処理を行う
	
	void UpdateInvincibility(void);	// 無敵時間の更新処理
	
	void CountUpdate(void);	// ダメージ表記用のカウントを更新

	void FoundCounterUodate(void);		// ハイライトカウンタ更新

	void IsReachedDeadPos(void);	// 死亡座標へ到達しているか

	void PriceDamageDraw(void);	// お金・ダメージ表記描画
	void HighLightDraw(void);	// ハイライト表示描画
	void DrawDebug(void);		// デバッグ用の描画

protected:

	// 発見時のハイライトの大きさ
	static constexpr Vector2 HIGHLIGHT_SIZE_BIG = { 100,160 };	// 大きい
	static constexpr Vector2 HIGHLIGHT_SIZE_MEDIUM = { 60,70 };	// 中くらい
	static constexpr Vector2 HIGHLIGHT_SIZE_SMALL = { 50,60 };	// 小さい

	static constexpr int FOUND_COUNTER_MAX = 60;	// 発見時のハイライトカウンタ時間

	static constexpr int DAMAGE_MULT = 15;	// ダメージの補正値

protected:
	
	Transform* trans_;	// Transform

protected:
	
	ItemInfo info_;	// アイテムの情報

	std::vector<DamageInfo> damageDrawList_;	// ダメージ表記用
};

