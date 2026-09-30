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
	// EnemyBaseの初期化
	EnemyBase::Init();

	// テーブルからデータを取得してセット
	const auto& data = EnemyTable::Table.at(ENEMY_TAG::GIGGLE);
	SetEnemyData(data);

	// タグを設定
	info_.tag_ = ENEMY_TAG::GIGGLE;

	// 初期ステートへ遷移
	ChangeState(STATE::IDLE);
}

void Giggle::Update(void)
{
	// 現在のステートに応じた更新処理を呼び分ける
	switch (state_)
	{
	case STATE::IDLE: 
		UpdateIdle(); 
		break;

	case STATE::THINK: 
		UpdateThink(); 
		break;

	case STATE::GIGGLING: 
		UpdateGiggling(); 
		break;

	default:
		break;
	}
}

void Giggle::ChangeState(STATE state)
{
	state_ = state;

	// 遷移時に一度だけ行う初期化処理を呼び出す
	switch (state_)
	{
	case STATE::IDLE: 
		ChangeIdle(); 
		break;

	case STATE::THINK: 
		ChangeThink(); 
		break;

	case STATE::GIGGLING: 
		ChangeGiggling(); 
		break;

	default:
		break;
	}
}

void Giggle::ChangeIdle(void)
{
	// 待機状態に遷移
	info_.step_ = static_cast<float>(STEP_IDLE);
}

void Giggle::ChangeThink(void)
{
}

void Giggle::ChangeGiggling(void)
{
	// 笑う状態に遷移
	info_.step_ = static_cast<float>(STEP_GIGGLING);

	// SE再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_GIGGLE);
}

void Giggle::UpdateIdle(void)
{
	// 待機状態の更新
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	// ステップタイマーが0以下になったら、考える状態に遷移
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
	int randomVal = GetRand(MAX_PERCENTAGE);

	if (randomVal < IDLE_PROBABILITY)
	{
		// 待機状態に遷移
		ChangeState(STATE::IDLE);
	}
	else
	{
		// 笑う状態に遷移
		ChangeState(STATE::GIGGLING);
	}
}

void Giggle::UpdateGiggling(void)
{
	// 笑う状態の更新
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	// ステップタイマーが0以下になったら、待機状態に遷移
	if (info_.step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::IDLE);
		return;
	}
}

