#pragma once
#include "../../Scene/Tutorial/TutorialInfo.h"
class PlayerActionCounter
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new PlayerActionCounter(); } }
	static PlayerActionCounter* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

public:

	PlayerActionCounter(void);

	void Init(void);	// 初期化

	// 指定されたステートのカウンターを渡す
	float GetCounter(Tutorial::STATE state) 
	{ return state != Tutorial::STATE::MAX ? counter_[state] : 100.0f; }

	// 指定されたステートのカウンターに指定された数足す
	void SetCounter(Tutorial::STATE state, float counter)
	{ if (state != Tutorial::STATE::MAX)counter_[state] += counter; }

	// 指定されたステートのカウンターを初期化する
	void ResetCounter(Tutorial::STATE state) 
	{ if(Tutorial::STATE::MAX != state) counter_[state] = 0.0f; }

private:

	static PlayerActionCounter* instance_;	// 静的インスタンス

	// コピー・ムーブ操作を禁止
	PlayerActionCounter(const PlayerActionCounter&) = delete;
	PlayerActionCounter& operator=(const PlayerActionCounter&) = delete;
	PlayerActionCounter(PlayerActionCounter&&) = delete;
	PlayerActionCounter& operator=(PlayerActionCounter&&) = delete;

private:

	float counter_[Tutorial::STATE::MAX];	// 確認項目ごとのカウンター
};

