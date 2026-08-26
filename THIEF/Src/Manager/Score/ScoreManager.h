#pragma once

#include<vector>

class Item;

class ScoreManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new ScoreManager(); } }
	static ScoreManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

public:

	ScoreManager(void);	// コンストラクタ

	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Destroy(void);	// 解放

	void ResetGame(void);	//	次ステージ用のリセット納品金額など

	void ResetTotalPrice(void);	// トータル金額をリセット
	
	void SetItems(std::vector<Item*> items);	// アイテム設定

	void AddCartPrice(const int price) { cartPrice_ += price; }	// カート内金額に加算
	const int GetCartPrice(void) const { return cartPrice_; }	// カート内を返す

	void AddDeliveryPrice(const int price) { deliveryPrice_ += price; }	// 納品金額に加算
	const int GetDeliveryPrice(void) const { return deliveryPrice_; }	// 納品金額を返す

	void SetTargetPrice(const int targetPrice) { targetPrice_ = targetPrice; }	// 指定の目標金額を設定
	const int GetTargetPrice(void) const { return targetPrice_; }				// 指定の目標金額を返す
	
	void AddTotalPrice(const int deliveryPrice) { totalPrice_ += deliveryPrice; }	// ゲームクリア後のショップで使える金額に加算
	void SubTotalPrice(const int deliveryPrice) { totalPrice_ -= deliveryPrice; }	// ゲームクリア後のショップで使える金額を減算
	const int GetTotalPrice(void) const { return totalPrice_; }						// ゲームクリア後のショップで使える金額を返す

private:
	// 目標金額をアイテム全体の金額の50%とする
	static constexpr float TARGET_PRICE_RATIO = 0.5f;

	// 警告文を出すときは全体の金額の70%とする
	static constexpr float SHOW_WARNING_PRICE_RATIO = 0.7f;

	// ボタンのアルファ値の変化速度
	static constexpr float ALPHA_SPEED = 3.0f;

	// ボタンのアルファ値の最大値と最小値
	static constexpr float ALPHA_MAX = 255.0f;
	static constexpr float ALPHA_MIN = 0.0f;

private:

	static ScoreManager* instance_;	// 静的インスタンス

	// コピー・ムーブ操作を禁止
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;
	ScoreManager(ScoreManager&&) = delete;
	ScoreManager& operator=(ScoreManager&&) = delete;

private:

	std::vector<Item*> items_;	// アイテム

private:

	int cartPrice_;		// カート内金額
	int deliveryPrice_;	// 納品金額
	int targetPrice_;	// 目標金額
	int totalPrice_;	// ゲームクリア後のショップで使える金額
	
	int warningPrice_;	// 警告文を出すときの目安金額
	bool showWarning_;	// 警告文出すか　true / 警告文を出す , false / 警告文を出さない

	float alpha_;		// アルファ値
	bool isIncreasing_;	// ボタンのアルファ値が増加しているかどうか
};

