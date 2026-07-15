#include "Animation.h"

#include <DxLib.h>

#include "../Render/Render3D.h"
#include "../../Object.h"

void Animation::Init(void)
{
	render_ = owner_->GetComponent<Render3D>();
}

void Animation::Update(void)
{
	if (!render_) return;
	if (playType_ == -1) return;

	playAnim_.step += playAnim_.speed;

	if (playAnim_.step > playAnim_.totalTime)
	{
		if (isLoop_)
			playAnim_.step = 0.0f;
		else
			playAnim_.step = playAnim_.totalTime;
			playType_ = -1;
	}

	MV1SetAttachAnimTime(render_->GetHandle(), playAnim_.attackNo, playAnim_.step);
}

void Animation::Release(void)
{
	for (auto& pair : animations_)
	{
		if (pair.second.model != -1)
		{
			MV1DeleteModel(pair.second.model);
		}
	}

	animations_.clear();
}

void Animation::Add(int type, float speed, const std::string& path)
{
	AnimationState anim;
	anim.model = MV1LoadModel(path.c_str());
	anim.animIndex = -1;

	AddInternal(type, speed, anim);
}

void Animation::AddInFbx(int type, float speed, int animIndex)
{
	AnimationState anim;
	anim.model = -1;
	anim.animIndex = animIndex;

	AddInternal(type, speed, anim);
}

void Animation::Play(int type, bool loop)
{
	if (!render_)
	{
		playType_ = type;
		isLoop_ = loop;
		return;
	}
	if (playType_ == type) return;

	int handle = render_->GetHandle();

	if (playType_ != -1)
	{
		MV1DetachAnim(handle, playAnim_.attackNo);
	}

	playType_ = type;
	playAnim_ = animations_[type];
	playAnim_.step = 0.0f;

	if (playAnim_.model == -1)
	{
		playAnim_.attackNo = MV1AttachAnim(handle, playAnim_.animIndex);
	}
	else
	{
		playAnim_.attackNo = MV1AttachAnim(handle, 0, playAnim_.model);
	}

	playAnim_.totalTime = MV1GetAttachAnimTotalTime(handle, playAnim_.attackNo);

	isLoop_ = loop;
}

bool Animation::IsEnd(void) const
{
	if(isLoop_)	return false;
	return playAnim_.step >= playAnim_.totalTime;
}

void Animation::AddInternal(int type, float speed, AnimationState& anim)
{
	anim.speed = speed;

	if (animations_.count(type) == 0)
	{
		animations_.emplace(type, anim);
	}
}
