#pragma once
#include <vector>

class SystemManager
{
public:

	// マウス感度
	static constexpr float MOUSE_SENSITIVITY = 0.003f;

	// パッド感度
	static constexpr float PAD_SENSITIVITY = 0.03f;

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static SystemManager& GetInstance(void);

	// 更新処理
	void Update(void);

	// 解放処理
	void Destroy(void);

	//リセット
	void ResetGame(void);

	// マウス感度を格納
	void SetMouseSensitivity(const float num) { MouseSensitivity_ = num; }

	// マウス感度を返す
	const float GetMouseSensitivity(void) const { return MouseSensitivity_; }

	// パッド感度を格納
	void SetPadSensitivity(const float num) { PadSensitivity_ = num; }

	// パッド感度を返す
	const float GetPadSensitivity(void) const { return PadSensitivity_; }

	// スコアを格納
	void SetScore(const int score) { score_ = score; }

	// スコアを返す
	const int GetScore(void) const { return score_; }

	// 操作デバイス種類を返す	true = マウス操作 / false = ゲームパッド操作
	const bool GetIsDevice(void)const { return isDeviceMouse_; }

private:

	// 静的インスタンス
	static SystemManager* instance_;

	// マウス感度
	float MouseSensitivity_;

	// パッド感度
	float PadSensitivity_;

	// スコア
	int score_;

	// 操作デバイス(true = マウス操作 / false = ゲームパッド操作）
	bool isDeviceMouse_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SystemManager(void);
	// コピーコンストラクタも同様
	SystemManager(const SystemManager& manager) = default;
	// デストラクタも同様
	~SystemManager(void) = default;
};

