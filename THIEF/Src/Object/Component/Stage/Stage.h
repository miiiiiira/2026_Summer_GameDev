#pragma once

#include "../Component.h"

#include <string>
#include <vector>
#include <DxLib.h>

// 前方宣言
class Transform;
class Item;

// ステージコンポーネント
class Stage : public Component
{
public:

	static constexpr float DONE_SWITCH_RAD= 20.0f;	// 納品完了スイッチの半径

public:

	Stage(void);				// コンストラクタ
	~Stage(void)override;		// デストラクタ
	
	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Draw2D(void) override;	// 2D描画
	void Draw3D(void) override;	// 3D描画
	
	// モデルIDを返す
	int GetModelId() const { return modelId_; }			// 見た目モデルのId
	int GetCollModelId() const { return collModelId_; }	// 当たり判定モデルのId
	
	Transform* GetTransform(void);	// Transformを返す

	VECTOR GetDeliverySize(void);	// 納品場所の大きさを返す

	VECTOR GetDeliveryPos(void);	// 納品場所の座標を返す

	VECTOR GetDoneSwitchPos(void);	// 納品完了スイッチの座標を返す

	bool GetStartClearCount(void);	// カウントが開始されているか

	std::vector<Item*> GetItems(void);	// アイテムたちのポインタを渡す

	void SetItem(Item* items);	// アイテムのポインタをセット

	void SetCollModel(std::string path);	// 当たり判定用のモデルを設定

	VECTOR ToWorldPos(VECTOR local);	// ワールド座標に変換

	VECTOR ToLocalPos(VECTOR world);	// ローカル座標に変換
	
	void StartClearCount(void);	// クリアカウントを開始させる

	void TrueIsDoneSwitch(void);	// 納品完了スイッチを押したことを知らせる

	bool GetIsPushDrawFlg(void);	// プッシュ画像表示フラグを渡す
	
	void SetIsPushDrawFlg(bool flg);	// プッシュ画像表示フラグを設定

private:
	
	void DrawDebug(void);	// デバック用描画
	
	void ClearCountUpdate(void);	// クリアカウントの更新処理

	void CallCountUpdate(void);	// 納品場所が呼ぶカウントの更新処理

	void PushUpDownUpdate(void);	// プッシュ画像の上下させる更新処理

private:

	static constexpr int CLEAR_COUNT_MAX = 180;	// クリアカウントの規定値

	static constexpr int COLL_COUNT_MAX = 600;	// 納品場所が呼ぶカウントの規定値
	
	// プッシュ画像
	static constexpr float PUSH_IMG_OFFSET_Y = 30.0f;		// オフセット座標
	static constexpr float PUSH_IMG_OFFSET_Y_MAX = 30.0f;	// 最大オフセット座標

	// チュートリアル
	static constexpr VECTOR TUTORIAL_DELIVERY_SIZE = { 220.0f,220.0f,240.0f };			// 納品場所の大きさ
	static constexpr VECTOR TUTORIAL_DELIVERY_POS = { -1.0f,220.0f,14630.0f };			// 納品場所の座標
	static constexpr VECTOR TUTORIAL_DELIVERY_SWITCH_POS = { 273.0f,148.0f,14310.0f };	// 納品スイッチの場所

private:

	std::vector<Item*> items_;	// アイテムたちのポインタを保持

private:

	// モデルID
	int modelId_ = -1;		// 見た目モデル
	int collModelId_ = -1;	// 当たり判定モデル


	// 納品場所
	VECTOR deliverySize_;	// 大きさ
	VECTOR deliveryPos_;	// 座標

	// 納品完了スイッチ
	VECTOR doneSwitchPos_;	// 座標
	bool isDoneSwitch_;		// 押されたか　true / 押された , false / 押されていない

	int clearCount_ = 0;	// クリアカウント

	int callCount_ = 0;	// 納品場所が呼ぶカウント

	// プッシュ画像
	int pushImg_  =  -1;		// ハンドル
	bool isPushDrawFlg_;		// 表示するか　true / 表示 , false / 非表示
	float pushUpDownOffsetPos_;	// 位置を上下させる座標
	bool isPushUp_;				// 座標の上下を変更する　true / 上へ , false / 下へ
};
