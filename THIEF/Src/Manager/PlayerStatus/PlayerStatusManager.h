#pragma once

class PlayerStatusManager
{
public:

	struct Status 
	{
		int hp_;	// HP
		int hpMax_; // 最大HP

		float staminaMax_;	// 最大スタミナ

		float dashMoveSpeed_;	// 移動速度

		int jumpNumMax_;	// ジャンプ可能回数
		
		float rangeMax_;	// 掴み距離
	};

public:

	static constexpr int DEFAULT_HP = 100;	// HP

	static constexpr float DASH_SPEED = 15.0f;	// ダッシュ時の移動速度

	static constexpr float DEFAULT_RENGE = 400.0f;	// プレイヤーの掴み距離

	static constexpr float DEFAULT_STAMINA = 40.0f;	// スタミナ

	static constexpr int DEFAULT_JUMP_NUM = 1;	// ジャンプ可能数

public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new PlayerStatusManager(); } }
	static PlayerStatusManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

	PlayerStatusManager();	// コンストラクタ
	
	void Destroy(void);	// 解放

	void ResetStatus(void);	// デフォルトのステータスにリセット

	void ResetHP(void);	// HPをMaxHPの値でリセットする

	const Status& GetPlayerStatus(void) { return status_; }	// プレイヤーのステータスを渡す

	void HpUp(int upNum);			// 最大HPを上げる
	void StaminaUp(float upNum);	// 最大スタミナを上げる
	void DashSpeedUp(float upNum);	// ダッシュ時のスピードを上げる
	void RangeUp(float upNum);		// 掴み可能範囲を大きくする
	void JumpNumUp(int upNum);		// ジャンプの回数を増やす
	void HealHp(int upNum);			// HPを回復する
	void SetHp(int hp);				// ダメージを与える

private:

	// 静的インスタンス
	static PlayerStatusManager* instance_;

	// コピー・ムーブ操作を禁止
	PlayerStatusManager(const PlayerStatusManager&) = delete;
	PlayerStatusManager& operator=(const PlayerStatusManager&) = delete;
	PlayerStatusManager(PlayerStatusManager&&) = delete;
	PlayerStatusManager& operator=(PlayerStatusManager&&) = delete;

private:

	// プレイヤーのステータス
	Status status_;
};

