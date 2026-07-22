#include "TutorialWallCollider.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../Object.h"

void TutorialWallCollider::Init(void)
{
	// 最大値で初期化
	limitDistance_ = FLT_MAX;

	// Transfrom取得
	transform_ = owner_->GetComponent<Transform>();
}

void TutorialWallCollider::Update(void)
{
	// 確認項目によって移動制限を設ける
	switch (SceneManager::GetInstance()->GetTutorialState())
	{
	case Tutorial::MOVE:
		limitDistance_ = 630.0f;
		break;
	case Tutorial::JUMP:
		limitDistance_ = 2130.0f;
		break;
	case Tutorial::DASH:
		limitDistance_ = 3600;
		break;
	case Tutorial::CROUCH:
		limitDistance_ = 5030.0f;
		break;
	case Tutorial::SLIDING:
		limitDistance_ = 6530.0f;
		break;
	case Tutorial::LIGHT:
		limitDistance_ = 7980.0f;
		break;
	case Tutorial::MAP:
		limitDistance_ = 9450.0f;
		break;
	case Tutorial::GRAB:
		limitDistance_ = 10920.0f;
		break;
	case Tutorial::RANGE:
		limitDistance_ = 12350.0f;
		break;
	case Tutorial::CART:
		limitDistance_ = FLT_MAX;
		break;
	case Tutorial::DELIVER:
		limitDistance_ = FLT_MAX;
		break;
	default:
		break;
	}

	// 壁の当たり判定
	WallColl();
}

void TutorialWallCollider::WallColl(void)
{
	// チュートリアルシーンだった場合
	if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)
	{
		if (transform_->pos_.z > limitDistance_)
		{
			transform_->pos_.z = limitDistance_;
		}
	}
}
