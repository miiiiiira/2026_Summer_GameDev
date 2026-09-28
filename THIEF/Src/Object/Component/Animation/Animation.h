#pragma once
#include <map>
#include <string>

#include "../Component.h"

class Render3D;

class Animation : public Component
{
public:

	struct AnimationState
	{
		int model = -1;
		int attackNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

public:

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Release(void);			// 解放

public:

	// アニメーション登録
	// モデル自体にアニメーションがついていない時に使用
	void Add(int type, float speed, const std::string& path);
	// モデル自体にアニメーションがついている時に使用
	void AddInFbx(int type, float speed, int animIndex);

	// アニメーションを再生する
	void Play(int type, bool loop = true);

	// 現在のアニメーションの種類を返す
	int GetPlayType(void) const { return playType_; }

	// アニメーションが終了していたらtrueを返す
	bool IsEnd(void) const;

private:

	Render3D* render_ = nullptr;

private:

	std::map<int, AnimationState> animations_;

	int playType_ = -1;
	AnimationState playAnim_;

	bool isLoop_ = true;

private:

	// アニメーション再生速度設定
	void AddInternal(int type, float speed, AnimationState& anim);
};
