#pragma once
#include "../Component.h"
#include <map>
#include <string>

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

	void Init(void) override;
	void Update(void) override;
	void Release(void);

	void Add(int type, float speed, const std::string& path);
	void AddInFbx(int type, float speed, int animIndex);

	void Play(int type, bool loop = true);

	int GetPlayType(void) const { return playType_; }
	bool IsEnd(void) const;

private:
	void AddInternal(int type, float speed, AnimationState& anim);

private:
	Render3D* render_ = nullptr;

	std::map<int, AnimationState> animations_;

	int playType_ = -1;
	AnimationState playAnim_;

	bool isLoop_ = true;
};
