#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Scene/SceneManager.h"
#include "../EnemyCommon.h"
#include "Giggle.h"

Giggle::Giggle(void)
{
}

Giggle::~Giggle(void)
{
}

void Giggle::Init(void)
{
	EnemyBase::Init();

	const auto& data = EnemyTable::Table.at(ENEMY_TAG::GIGGLE);
	SetEnemyData(data);

	// 初期ステートへ遷移
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
	info_.step_ = 10.0f;
}

void Giggle::ChangeThink(void)
{
}

void Giggle::ChangeGiggling(void)
{
	info_.step_ = 20.0f;
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_GIGGLE);
}

void Giggle::UpdateIdle(void)
{
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (info_.step_ < 0.0f)
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
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (info_.step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::IDLE);
		return;
	}
}

