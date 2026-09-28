#include <DxLib.h>

#include "../Render/Render3D.h"
#include "../../Object.h"

#include "Animation.h"

void Animation::Init(void)
{
	render_ = owner_->GetComponent<Render3D>();
}

void Animation::Update(void)
{
	if (!render_) return;

	// アニメーションが再生されていなかったら処理を行わない
	if (playType_ == -1) return;

	// アニメーションを進める
	playAnim_.step += playAnim_.speed;

	// 再生時間が総再生時間を超えていたら
	if (playAnim_.step > playAnim_.totalTime)
	{
		// ループ再生の場合
		if (isLoop_)
		{
			// 再生時間を初期化
			playAnim_.step = 0.0f;
		}
		// ループ再生ではない場合
		else
		{
			// 再生時間を総再生時間に設定
			playAnim_.step = playAnim_.totalTime;
			// 種類を初期化
			playType_ = -1;
		}
	}

	// アタッチしているアニメーションの再生時間を設定する
	MV1SetAttachAnimTime(render_->GetHandle(), playAnim_.attackNo, playAnim_.step);
}

void Animation::Release(void)
{
	// アニメーションの解放
	for (auto& pair : animations_)
	{
		if (pair.second.model != -1)
		{
			MV1DeleteModel(pair.second.model);
		}
	}

	// 使い終わったらクリア
	animations_.clear();
}

void Animation::Add(int type, float speed, const std::string& path)
{
	AnimationState anim;
	// アニメーション読み込み
	anim.model = MV1LoadModel(path.c_str());
	anim.animIndex = -1;

	// アニメーションの情報を設定
	AddInternal(type, speed, anim);
}

void Animation::AddInFbx(int type, float speed, int animIndex)
{
	AnimationState anim;
	anim.model = -1;
	anim.animIndex = animIndex;

	// アニメーションの情報を設定
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

	// 既に同じアニメーションが再生中の場合は何もしない
	if (playType_ == type) return;

	// モデルハンドルを取得
	int handle = render_->GetHandle();

	// 以前に再生していたアニメーションがあればアタッチ解除する
	if (playType_ != -1)
	{
		MV1DetachAnim(handle, playAnim_.attackNo);
	}

	// 再生するアニメーション情報を更新し、再生時間を初期化
	playType_ = type;
	playAnim_ = animations_[type];
	playAnim_.step = 0.0f;

	// アニメーションをモデルにアタッチ
	if (playAnim_.model == -1)
	{
		// 単一モデル内のアニメーションインデックスからアタッチする場合
		playAnim_.attackNo =
			MV1AttachAnim(
				handle,
				playAnim_.
				animIndex);
	}
	else
	{
		// 外部のモデルハンドルからアタッチする場合
		playAnim_.attackNo =
			MV1AttachAnim(
				handle,
				0,
				playAnim_.
				model);
	}

	// アタッチしたアニメーションの総再生時間を取得
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(handle, playAnim_.attackNo);

	// ループフラグの更新
	isLoop_ = loop;
}

bool Animation::IsEnd(void) const
{
	// ループ再生だったら処理を行わない
	if(isLoop_)	return false;

	// 再生時間が総再生時間を超えていたら
	return playAnim_.step >= playAnim_.totalTime;
}

void Animation::AddInternal(int type, float speed, AnimationState& anim)
{
	// アニメーションの速度を設定
	anim.speed = speed;

	if (animations_.count(type) == 0)
	{
		// アニメーション情報を追加
		animations_.emplace(type, anim);
	}
}
