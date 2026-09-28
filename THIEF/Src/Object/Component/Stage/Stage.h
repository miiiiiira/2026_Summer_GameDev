#pragma once

#include <string>
#include <vector>
#include <DxLib.h>

#include "../../Object.h"
#include "../Component.h"
#include "../Transform/Transform.h"

// 前方宣言
class Transform;
class Item;

// ステージコンポーネント
class Stage : public Component
{
public:

	// 納品完了スイッチの半径
	static constexpr float DONE_SWITCH_RAD = 20.0f;

public:

	Stage(void);				// コンストラクタ
	~Stage(void)override;		// デストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Draw2D(void) override;	// 2D描画
	void Draw3D(void) override;	// 3D描画

public:

	// モデルIDを返す
	int GetModelId() const { return modelId_; }			// 見た目モデルのId
	int GetCollModelId() const { return collModelId_; }	// 当たり判定モデルのId

	// Transformを返す
	Transform* GetTransform(void) { return owner_->GetComponent<Transform>(); }

	// 納品場所の大きさを返す
	VECTOR GetDeliverySize(void) { return deliverySize_; }

	// 納品場所の座標を返す
	VECTOR GetDeliveryPos(void) { return deliveryPos_; }

	// 納品完了スイッチの座標を返す
	VECTOR GetDoneSwitchPos(void) { return doneSwitchPos_; }

	// カウントが開始されているか
	bool GetStartClearCount(void) { return clearCount_ > 0; }

	// アイテムたちのポインタを渡す
	std::vector<Item*> GetItems(void) { return items_; }

	// アイテムのポインタをセット
	void SetItem(Item* items) { items_.push_back(items); }

	// 当たり判定用のモデルを設定
	void SetCollModel(std::string path);

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

	// クリアカウントを開始させる
	void StartClearCount(void) { clearCount_++; }

	// 納品完了スイッチを押したことを知らせる
	void TrueIsDoneSwitch(void) { isDoneSwitch_ = true; }

	// プッシュ画像表示フラグを渡す
	bool GetIsPushDrawFlg(void) { return isPushDrawFlg_; }

	// プッシュ画像表示フラグを設定
	void SetIsPushDrawFlg(bool flg) { isPushDrawFlg_ = flg; }

private:

	// クリアカウントの規定値
	static constexpr int CLEAR_COUNT_MAX = 180;

	// 納品場所が呼ぶカウントの規定値
	static constexpr int COLL_COUNT_MAX = 600;

	// プッシュ画像
	static constexpr float PUSH_IMG_OFFSET_Y = 30.0f;		// オフセット座標
	static constexpr float PUSH_IMG_OFFSET_Y_MAX = 30.0f;	// 最大オフセット座標

	// チュートリアル
	static constexpr VECTOR TUTORIAL_DELIVERY_SIZE = { 220.0f,220.0f,240.0f };			// 納品場所の大きさ
	static constexpr VECTOR TUTORIAL_DELIVERY_POS = { -1.0f,220.0f,14630.0f };			// 納品場所の座標
	static constexpr VECTOR TUTORIAL_DELIVERY_SWITCH_POS = { 273.0f,148.0f,14310.0f };	// 納品スイッチの場所

private:

	// アイテムたちのポインタを保持
	std::vector<Item*> items_;

private:

	// モデルID
	int modelId_ = -1;		// 見た目モデル
	int collModelId_ = -1;	// 当たり判定モデル

	// 納品場所
	VECTOR deliverySize_ = { 0.0f,0.0f,0.0f };	// 大きさ
	VECTOR deliveryPos_ = { 0.0f,0.0f,0.0f };	// 座標

	// 納品完了スイッチ
	VECTOR doneSwitchPos_ = { 0.0f,0.0f,0.0f };	// 座標
	bool isDoneSwitch_ = false;					// 押されたか　true / 押された , false / 押されていない

	// クリアカウント
	int clearCount_ = 0;

	// 納品場所が呼ぶカウント
	int callCount_ = 0;

	// プッシュ画像
	int pushImg_ = -1;				// ハンドル
	bool isPushDrawFlg_ = false;	// 表示するか　true / 表示 , false / 非表示
	float pushUpDownOffsetPos_;		// 位置を上下させる座標
	bool isPushUp_ = true;			// 座標の上下を変更する　true / 上へ , false / 下へ

private:

	// デバック用描画
	void DrawDebug(void);

	// クリアカウントの更新処理
	void ClearCountUpdate(void);

	// 納品場所が呼ぶカウントの更新処理
	void CallCountUpdate(void);

	// プッシュ画像の上下させる更新処理
	void PushUpDownUpdate(void);
};
