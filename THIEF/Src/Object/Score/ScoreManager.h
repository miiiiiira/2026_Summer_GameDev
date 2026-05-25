#pragma once


class ScoreManager
{
public:

	// マウス感度
	static constexpr float MOUSE_SENSITIVITY = 0.003f;

	// パッド感度
	static constexpr float PAD_SENSITIVITY = 0.03f;

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ScoreManager& GetInstance(void);

	// 解放処理
	void Destroy(void);

	//リセット
	void ResetGame(void);

	// 納品金額に加算
	void SetDeliveryPrice(const int price) { deliveryPrice_ = price; }

	// 納品金額を返す
	const int GetDeliveryPrice(void) const { return deliveryPrice_; }

	// 持っている全金額に加算
	void SetTotalPrice(const int deliveryPrice) { totalPrice_ = deliveryPrice; }

	// 持っている全金額を返す
	const int GetTotalPrice(void) const { return totalPrice_; }

private:

	// 静的インスタンス
	static ScoreManager* instance_;

	// 納品金額
	int deliveryPrice_;

	// 持っている全金額
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

