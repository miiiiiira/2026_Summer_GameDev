#pragma once

#include<vector>

class Item;

class ScoreManager
{
public:
	// 目標金額
	static constexpr int TARGET_PRICE = 500;

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ScoreManager& GetInstance(void);

	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);

	// 解放処理
	void Destroy(void);

	//リセット
	void ResetGame(void);

	// トータル金額をリセット
	void ResetTotalPrice(void);

	// アイテム設定
	void SetItems(std::vector<Item*> items) { items_ = items; }

	// 納品金額に加算
	void AddDeliveryPrice(const int price) { deliveryPrice_ += price; }

	// 納品金額を返す
	const int GetDeliveryPrice(void) const { return deliveryPrice_; }

	// 指定の目標金額を設定
	void SetTargetPrice(const int targetPrice) { targetPrice_ = targetPrice; }

	// 指定の目標金額を返す
	const int GetTargetPrice(void) const { return targetPrice_; }
	
	// ゲームクリア後のショップで使える金額に加算
	void AddTotalPrice(const int deliveryPrice) { totalPrice_ += deliveryPrice; }
	
	// ゲームクリア後のショップで使える金額を減算
	void SubTotalPrice(const int deliveryPrice) { totalPrice_ -= deliveryPrice; }

	// ゲームクリア後のショップで使える金額を返す
	const int GetTotalPrice(void) const { return totalPrice_; }

private:

	// 静的インスタンス
	static ScoreManager* instance_;

	// アイテム
	std::vector<Item*> items_;

	// 納品金額
	int deliveryPrice_;

	// 目標金額
	int targetPrice_;

	// ゲームクリア後のショップで使える金額
	int totalPrice_;

	// 操作デバイス(true = マウス操作 / false = ゲームパッド操作）
	bool isDeviceMouse_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ScoreManager(void);
	// コピーコンストラクタも同様
	ScoreManager(const ScoreManager& manager) = default;
	// デストラクタも同様
	~ScoreManager(void) = default;
};

