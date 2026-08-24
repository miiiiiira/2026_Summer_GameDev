#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Scene/SceneManager.h"
#include "Giggle.h"

Giggle::Giggle(int modelId)
{
}

Giggle::~Giggle(void)
{
}

void Giggle::OnInitialize(void)
{
	ChangeState(STATE::IDLE);
}

void Giggle::Update(void)
{
	switch (state_)
	{
	case Giggle::STATE::IDLE: UpdateIdle(); break;
	case Giggle::STATE::THINK: UpdateThink(); break;
	case Giggle::STATE::GIGGLING: UpdateGiggling(); break;
	default:
		break;
	}
}

void Giggle::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Giggle::STATE::IDLE: ChangeIdle(); break;
	case Giggle::STATE::THINK: ChangeThink(); break;
	case Giggle::STATE::GIGGLING: ChangeGiggling(); break;
	default:
		break;
	}
}

void Giggle::ChangeIdle(void)
{
	step_ = 10.0f;
}

void Giggle::ChangeThink(void)
{
}

void Giggle::ChangeGiggling(void)
{
	step_ = 20.0f;
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_GIGGLE);
}

void Giggle::UpdateIdle(void)
{
	step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}
}

void Giggle::UpdateThink(void)
{
	// 思考
	// ランダムに次の行動を決定	
	// 90%で待機、10%で笑う
	int rand = GetRand(100);
	if (rand < 90)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::GIGGLING);
	}
}

void Giggle::UpdateGiggling(void)
{
	step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::IDLE);
		return;
	}
}

