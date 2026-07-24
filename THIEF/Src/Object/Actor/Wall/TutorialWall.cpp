#include <algorithm>
#include "../../../Scene/SceneManager.h"
#include "TutorialWall.h"


TutorialWall::TutorialWall(void)
	:
	modelId_(-1)
{
	walls_.clear();
}

TutorialWall::~TutorialWall(void)
{
}

void TutorialWall::Init(void)
{
	VECTOR scale = { 10.0f, 10.0f, 1.0f };
	MV1SetScale(modelId_, scale);
}

void TutorialWall::Load(void)
{
	modelId_ = MV1LoadModel("Data/Model/Wall/GateMist.mv1");
}


void TutorialWall::Update(Tutorial::STATE currentState, bool isClearState, int clearStateEndCount, int maxClearCount)
{
	Tutorial::STATE currentWallState; // クリア判定の対象になる壁のステート
	Tutorial::STATE nextWallState;    // 残して表示しておく次の壁のステート

	if (isClearState)
	{
		currentWallState = static_cast<Tutorial::STATE>(static_cast<int>(currentState) - 1);
		nextWallState = currentState;
	}
	else
	{
		currentWallState = currentState;
		nextWallState = static_cast<Tutorial::STATE>(static_cast<int>(currentState) + 1);
	}

	// 範囲外チェック
	if (nextWallState >= Tutorial::STATE::MAX)
	{
		nextWallState = Tutorial::STATE::MAX;
	}

	float fadeProgress = 0.0f;
	if (maxClearCount > 0)
	{
		fadeProgress = static_cast<float>(clearStateEndCount) / static_cast<float>(maxClearCount);
		fadeProgress = std::clamp(fadeProgress, 0.0f, 1.0f);
	}

	for (auto& wall : walls_)
	{
		// 消す対象の壁か
		auto itCurrent = std::find(wall.activeStates_.begin(), wall.activeStates_.end(), currentWallState);
		bool isCurrentWall = (itCurrent != wall.activeStates_.end());

		// 残す対象（次の壁）か
		auto itNext = std::find(wall.activeStates_.begin(), wall.activeStates_.end(), nextWallState);
		bool isNextWall = (itNext != wall.activeStates_.end());

		// 次の壁
		if (isNextWall)
		{
			wall.fadeProgress_ = 0.0f;
			wall.isActive_ = true;
		}
		// 消す対象の壁
		else if (isCurrentWall)
		{
			if (isClearState)
			{
				wall.fadeProgress_ = fadeProgress;
				wall.isActive_ = (fadeProgress < 1.0f);
			}
			else
			{
				wall.fadeProgress_ = 0.0f;
				wall.isActive_ = true;
			}
		}
		// それ以外の壁
		else
		{
			wall.fadeProgress_ = 0.0f;
			wall.isActive_ = false;
		}
	}
}

void TutorialWall::Draw(void)
{
	if (modelId_ == -1) return;

	for (const auto& wall : walls_)
	{
		if (!wall.isActive_) continue;

		// 座標を設定して描画
		MV1SetPosition(modelId_, wall.pos_);

		float opacity = 1.0f - wall.fadeProgress_;

		MV1SetOpacityRate(modelId_, opacity);
		MV1DrawModel(modelId_);
		MV1SetOpacityRate(modelId_, 1.0f);
	}
}

void TutorialWall::Release(void)
{
	if (modelId_ != -1)
	{
		MV1DeleteModel(modelId_);
		modelId_ = -1;
	}
	walls_.clear();
}

void TutorialWall::AddWall(VECTOR pos, const std::vector<Tutorial::STATE>& states)
{
	WallData data{};
	data.pos_ = pos;
	data.activeStates_ = states;
	data.isActive_ = false;

	walls_.push_back(data);
}