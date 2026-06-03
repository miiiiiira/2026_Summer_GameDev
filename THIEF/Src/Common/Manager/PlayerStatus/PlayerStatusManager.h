#pragma once

class PlayerStatusManager
{
public:

	// HP
	static constexpr float DEFAULT_HP = 100.0f;

	// ダッシュ時の移動速度
	static constexpr float DASH_SPEED = 20.0f;

	// プレイヤーの掴み距離
	static constexpr float DEFAULT_RENGE = 400.0f;

	// スタミナ
	static constexpr float DEFAULT_STAMINA = 40.0f;

	// ジャンプ可能数
	static constexpr int DEFAULT_JUMP_NUM = 1;

	struct Status {

		// HP
		float hp_;
		float hpMax_;

		// 移動速度
		float dashMoveSpeed_;

		// スタミナ
		float staminaMax_;

		// ジャンプ可能回数
		int jumpNumMax_;

		// 掴み距離
		float rangeMax_;
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static PlayerStatusManager& GetInstance(void);

	// 更新処理
	void Update(void);

	// 解放処理
	void Destroy(void);

	//リセット
	void ResetStatus(void);

	// プレイヤーのステータスを渡す
	const Status& GetPlayerStatus(void) { return status_; }

	// 最大HPを上げる
	void HpUp(float upNum);

	// 最大スタミナを上げる
	void StaminaUp(float upNum);

	// ダッシュ時のスピードを上げる
	void DashSpeedUp(float upNum);

	// 掴み可能範囲を大きくする
	void RangeUp(float upNum);

	// ジャンプの回数を増やす
	void JumpNumUp(int upNum);

	// HPを回復する
	void HealHp(float upNum);

private:

	// 静的インスタンス
	static PlayerStatusManager* instance_;

	// プレイヤーのステータス
	Status status_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	PlayerStatusManager(void);
	// コピーコンストラクタも同様
	PlayerStatusManager(const PlayerStatusManager& manager) = default;
	// デストラクタも同様
	~PlayerStatusManager(void) = default;
};

