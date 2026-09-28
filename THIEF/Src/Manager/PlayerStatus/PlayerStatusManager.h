#pragma once

class PlayerStatusManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new PlayerStatusManager(); } }
	static PlayerStatusManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	// 静的インスタンス
	static PlayerStatusManager* instance_;	

	// コピー・ムーブ操作を禁止
	PlayerStatusManager(const PlayerStatusManager&) = delete;
	PlayerStatusManager& operator=(const PlayerStatusManager&) = delete;
	PlayerStatusManager(PlayerStatusManager&&) = delete;
	PlayerStatusManager& operator=(PlayerStatusManager&&) = delete;

public:

	// アップグレードできるステータス
	struct Status 
	{
		// HP
		int hp_;

		// 最大HP
		int hpMax_; 

		// 最大スタミナ
		float staminaMax_;	

		// 移動速度
		float dashMoveSpeed_;	

		// ジャンプ可能回数
		int jumpNumMax_;	
		
		// 掴み距離
		float rangeMax_;	
	};

public:

	// 強化項目のデフォルト数値
	static constexpr int DEFAULT_HP = 100;			// HP
	static constexpr float DASH_SPEED = 15.0f;		// ダッシュ時の移動速度
	static constexpr float DEFAULT_RENGE = 400.0f;	// プレイヤーの掴み距離
	static constexpr float DEFAULT_STAMINA = 40.0f;	// スタミナ
	static constexpr int DEFAULT_JUMP_NUM = 1;		// ジャンプ可能数

public:

	PlayerStatusManager(void);	// コンストラクタ
	
	void Destroy(void);			// 解放

public:

	// デフォルトのステータスにリセット
	void ResetStatus(void);	

	// HPをMaxHPの値でリセットする
	void ResetHP(void) { status_.hp_ = status_.hpMax_; }

	// プレイヤーのステータスを渡す
	const Status& GetPlayerStatus(void) { return status_; }	

	// 最大HPを上げる
	void HpUp(int upNum) { status_.hp_ += upNum, status_.hpMax_ += upNum; }	

	// スタミナの最大値を上げる
	void StaminaUp(float upNum) { status_.staminaMax_ += upNum; }

	// ダッシュ時のスピードを上げる
	void DashSpeedUp(float upNum) { status_.dashMoveSpeed_ += upNum; }

	// 掴みの範囲を大きくする
	void RangeUp(float upNum) { status_.rangeMax_ += upNum; }

	// ジャンプの最大値を上げる
	void JumpNumUp(int upNum) { status_.jumpNumMax_ += upNum; }

	// HPを回復する
	void HealHp(int upNum);	

	// HPを保持させておく
	void SetHp(int hp) { status_.hp_ = hp; }

private:

	// プレイヤーのステータス
	Status status_;	
};

