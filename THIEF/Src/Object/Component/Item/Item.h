#pragma once

#include <vector>

#include "../Component.h"
#include "ItemInfo.h"
#include "../../../Common/Math/Vector2.h"
#include "../Transform/Transform.h"

// 前方宣言
class Transform;

class Item :public Component
{
public:
	
	virtual ~Item(void)override;	// デストラクタ

	void Init(void)override;		// 初期化
	void Update(void)override;		// 更新
	void Draw2D(void)override;		// 2D描画
	void Draw3D(void)override;		// 3D描画

public:

	// Transformを返す
	Transform* GetTransform(void){ return trans_; }

	// アイテムの情報を渡す
	const ItemInfo& GetInfo(void) { return info_; }

	// モデルIDを渡す
	int GetModelID(void) { return info_.modelId_; }	
	
	// カメラとの距離を渡す
	float GetCameraDistance(void);	

public:

	// 掴まれた状態にする
	void StartGrabbing(VECTOR localPos);

	// 掴まれた状態を終了する
	void EndGrabbed(void);

	// アイテムにダメージを与える(ダメージ数、当たった場所)
	void SetDamage(VECTOR pos);	
	
	// 指定された座標をアイテムの座標に反映
	void SetPos(const VECTOR& pos);	
	
	// 指定された座標をアイテムの前回座標に反映
	// 指定座標を前フレーム座標に設定
	void SetPrevPos(const VECTOR& prevPos) { trans_->prevPos_ = prevPos; }

	// ローカル座標を設定 Z軸のみ
	// 指定の距離をローカルZ軸に設定
	void SetLocalPosZ(float localPosZ) { info_.localPos_.z = localPosZ; }

	// 納品場所に入ったかどうかを変更
	void SetHasTouchedDelivery(bool flg) { info_.hasTouchedDeliveryLocation_ = flg; }	
	
	// カートに入ったかどうかを変更
	void SetHasTouchedCart(bool flg) { info_.hasTouchedCart_ = flg; }	

	// 下方向の加速度を0にする
	void SetVelocityYZero(void) { info_.velocity_.y = 0.0f; }	

	// 発見したことにする
	void TrueIsFound(void);	
	
	// 地面についた
	void OnFloor(void);	

protected:
	
	// Transform
	Transform* trans_ = nullptr;	

protected:
	
	// アイテムの情報
	ItemInfo info_;	

	// ダメージ表記用
	std::vector<DamageInfo> damageDrawList_;	

protected:

	// 個々のパラメータを設定する
	virtual void SetParam(void) {};

	// 個々の破壊時の処理
	virtual void Break(void) {};

	// 個々のダメージ時の処理
	virtual void Damage(void) {};

	// 生存していない場合　チュートリアル時の生成処理
	void IsNotAliveTutorial(void);	

	// 重力をかける
	void Gravity(void);	

	// アイテムの重み
	void Weight(void);	

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);	

	// 無敵時間の更新処理
	void UpdateInvincibility(void);	

	// ダメージ表記用のカウントを更新
	void CountUpdate(void);	

	// ハイライトカウンタ更新
	void FoundCounterUodate(void);		

	// 死亡座標へ到達しているか
	void IsReachedDeadPos(void);	

	// 描画系
	void PriceDraw(void);		// 金額表示
	void DamageDraw(void);		// ダメージ表示
	void HighLightDraw(void);	// ハイライト表示

#ifdef _DEBUG
	void DrawDebug(void);		// デバッグ用
#endif // _DEBUG
};

