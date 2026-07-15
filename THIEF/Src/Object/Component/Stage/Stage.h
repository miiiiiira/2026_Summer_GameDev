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

	// 納品完了スイッチの半径
	static constexpr float DONE_SWITCH_RAD= 20.0f;

	~Stage(void)override;

	// 初期化
	void Init(void) override;
	void Update(void) override;
	void Draw3D(void) override;
	
	// モデルIDを返す
	int GetModelId() const { return modelId_; }
	int GetCollModelId() const { return collModelId_; }

	// Transformを返す
	Transform* GetTransform();

	// 納品場所の大きさを返す
	VECTOR GetDeliverySize(void);

	// 納品場所の座標を返す
	VECTOR GetDeliveryPos(void);

	// 納品完了スイッチの座標を返す
	VECTOR GetDoneSwitchPos(void);

	std::vector<Item*> GetItems(void);

	// アイテム設定
	void SetItem(Item* items);

	// 当たり判定用のモデルを設定
	void SetCollModel(std::string path);

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

	// カウントが開始されているか
	bool GetStartClearCount(void);
	// クリアカウントを開始させる
	void StartClearCount(void);

	// 納品完了スイッチを押したことを知らせる
	void TrueIsDoneSwitch(void);

private:
	// クリアカウントの規定値
	static constexpr int CLEAR_COUNT_MAX = 180;

	// モデルID
	int modelId_ = -1;

	int collModelId_ = -1;

	std::vector<Item*> items_;

	// クリアカウント
	int clearCount_ = 0;

	// 納品場所の大きさ
	VECTOR deliverySize_;

	// 納品場所の座標
	VECTOR deliveryPos_;

	// 納品完了スイッチの座標
	VECTOR doneSwitchPos_;

	// 納品完了スイッチが押されたか　true / 押された , false / 押されていない
	bool isDoneSwitch_;

	// デバック用描画
	void DrawDebug(void);
};
