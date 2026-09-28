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

private:

	// 静的インスタンス
	static ScoreManager* instance_;

	// コピー・ムーブ操作を禁止
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;
	ScoreManager(ScoreManager&&) = delete;
	ScoreManager& operator=(ScoreManager&&) = delete;

public:

	ScoreManager(void);	// コンストラクタ

	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Destroy(void);	// 解放

public:

	//	次ステージ用のリセット納品金額など
	void ResetGame(void);	

	// トータル金額をリセット
	void ResetTotalPrice(void) { totalPrice_ = 0; }
	
	// アイテム設定
	void SetItems(std::vector<Item*> items);	

	// カート内金額に加算
	void AddCartPrice(const int price) { cartPrice_ += price; }	
	// カート内を返す
	const int GetCartPrice(void) const { return cartPrice_; }

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

	// 目標金額をアイテム全体の金額の50%とする
	static constexpr float TARGET_PRICE_RATIO = 0.5f;

	// 警告文を出すときは全体の金額の70%とする
	static constexpr float SHOW_WARNING_PRICE_RATIO = 0.7f;

	// ボタンのアルファ値
	static constexpr float ALPHA_SPEED = 3.0f;	// 変化速度
	static constexpr float ALPHA_MAX = 255.0f;	// 最大値
	static constexpr float ALPHA_MIN = 0.0f;	// 最小値

private:

	std::vector<Item*> items_;	// アイテム

private:

	// 金額系
	int cartPrice_ = 0;		// カート内金額
	int deliveryPrice_ = 0;	// 納品金額
	int targetPrice_ = 0;	// 目標金額
	int totalPrice_ = 0;	// ゲームクリア後のショップで使える金額
	
	// 警告文
	int warningPrice_ = 0;		// 警告文を出すときの目安金額
	bool showWarning_ = false;	// 警告文出すか　true / 警告文を出す , false / 警告文を出さない

	// アルファ値
	float alpha_ = ALPHA_MAX;
	// ボタンのアルファ値が増加しているかどうか
	bool isIncreasing_ = false;	

private:

	// 警告文の更新処理
	void UpdateWarwning(void);

	// 描画系
	void DrawDeliveryAndTargetPrice(void);	// 納品金額、目標金額
	void DrawCaryPrice(void);				// カート内金額
	void DrawWarning(void);					// 警告文
};

