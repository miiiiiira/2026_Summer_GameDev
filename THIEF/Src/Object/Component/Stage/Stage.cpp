#include "Stage.h"

#include "../Render/Render3D.h"
#include "../Collider/DeliveryLocationCollider/DeliveryLocationCollider.h"
#include "../../../Scene/SceneManager.h"
#include "../../Object.h"
#include "DeliveryInfo.h"
#include "../../../Common/Manager/Score/ScoreManager.h"
#include "../../../Common/Manager/Audio/AudioManager.h"
#include "../Animation/Animation.h"

Stage::~Stage(void)
{
	MV1DeleteModel(collModelId_);
	items_.clear();
}

void Stage::Init()
{
	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	modelId_ = render->GetHandle();

	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// 座標の更新
	MV1SetPosition(modelId_, trans->pos_);

	// 衝突情報構築
	MV1SetupCollInfo(modelId_, -1);

	// オーナーからアニメーションコンポーネントを取得
	auto anim = owner_->GetComponent<Animation>();
	if (anim != nullptr)
	{
		anim->AddInFbx(0, 0.5f, 0);
	}

	if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)
	{
		deliverySize_ = { 230.0f,220.0f,240.0f };
		deliveryPos_ =  {-10.0f,220.0f,14665.0f};
		doneSwitchPos_ = { 286.0f,148.0f,14392.0f };
	}
	else
	{

		// ステージ情報を取ってきて初期化処理を行う
		auto stageNum = SceneManager::GetInstance()->GetCurrentStage();
		auto deliveryData = DeliveryTable::Table.find(stageNum);

		if (deliveryData != DeliveryTable::Table.end())
		{
			// 納品場所の大きさ
			deliverySize_ = deliveryData->second.deliverySize_;

			// 納品場所の座標
			deliveryPos_ = trans->pos_;
			deliveryPos_ = VAdd(deliveryPos_, deliveryData->second.deliveryLocalPos_);

			// 納品完了スイッチの座標
			doneSwitchPos_ = trans->pos_;
			doneSwitchPos_ = VAdd(doneSwitchPos_, deliveryData->second.doneSwitchLocalPos_);
		}
		else
		{
			// データがなかった場合は0初期化
			deliverySize_ = deliveryPos_ = doneSwitchPos_ = {};
		}
	}

	// 納品完了スイッチフラグ
	isDoneSwitch_ = false;
}

void Stage::Update(void)
{
	// 納品完了スイッチが押されていたら
	if (isDoneSwitch_)
	{
		auto anim = owner_->GetComponent<Animation>();
		if (anim != nullptr)
		{
			// アニメーションを再生
			anim->Play(0, false);
		}

		// 納品完了スイッチフラグを折る
		isDoneSwitch_ = false;
	}

	// クリアカウントが開始されていなければ処理を行わない
	if (clearCount_ <= 0)return;
	// クリアまでのカウントを進める
	clearCount_++;

	// クリアカウントが規定量に達したらステージクリアへ
	if (clearCount_ >= CLEAR_COUNT_MAX)
	{
		if (SceneManager::GetInstance()->GetNowSceneTag() == SCENE_TAG::TUTORIAL)
		{
			// チュートリアル時にカウンタに加算される
			SceneManager::GetInstance()->TutorialCounter(Tutorial::DELIVER);
		}
		else
		{
			// ステージクリアへ
			SceneManager::GetInstance()->TrueStageClear();
		}

		// TODO 完全納品完了音
		AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_SUC);
		return;
	}

	// 一秒ごとにカウント音を出す
	if (clearCount_ % 60 == 0)
	{
		// TODO 納品完了待ち時間のカウント音
		AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_SUC);
	}

	// 納品済みの金額を確認
	int deliveryPrice = ScoreManager::GetInstance().GetDeliveryPrice();
	// 目標金額を確認
	int targetPrice = ScoreManager::GetInstance().GetTargetPrice();

	// 目標金額を達成していなかったら
	if (deliveryPrice < targetPrice)
	{
		// カウントを終了
		clearCount_ = 0;

		// TODO 納品失敗音長めのやつ
		AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_FAI);
	}
}

void Stage::Draw3D(void)
{
#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG
}

Transform* Stage::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

VECTOR Stage::GetDeliverySize(void)
{
	return deliverySize_;
}

void Stage::SetItem(Item* items)
{
	items_.push_back(items);
}

void Stage::SetCollModel(std::string path)
{
	// 既に読み込み済みだったら削除して読み込み
	if (collModelId_ != -1)
	{
		// モデルハンドルを削除
		MV1DeleteModel(collModelId_);
		collModelId_ = -1;
	}

	// 当たり判定モデルが入ってなかったら
	if (path == "NoData")
	{
		collModelId_ = -1;
	}
	// 当たり判定モデルが入っていたら
	else
	{
		// 当たり判定モデルを読み込み
		collModelId_ = MV1LoadModel(path.c_str());
	}
}

VECTOR Stage::GetDeliveryPos(void)
{
	return deliveryPos_;
}

VECTOR Stage::GetDoneSwitchPos(void)
{
	return doneSwitchPos_;
}

std::vector<Item*> Stage::GetItems(void)
{
	return items_;
}

VECTOR Stage::ToWorldPos(VECTOR local)
{
	auto trans = owner_->GetComponent<Transform>();
	return VAdd(local, trans->pos_);
}

VECTOR Stage::ToLocalPos(VECTOR world)
{
	auto trans = owner_->GetComponent<Transform>();
	return VSub(world, trans->pos_);
}

bool Stage::GetStartClearCount(void)
{
	return clearCount_ > 0;
}

void Stage::StartClearCount(void)
{
	// クリアカウントを開始させる
	clearCount_++;
}

void Stage::TrueIsDoneSwitch(void)
{
	// 押されたことを知らせる
	isDoneSwitch_ = true;
}

void Stage::DrawDebug(void)
{
	// 納品完了スイッチの当たり判定視覚化
	DrawSphere3D(doneSwitchPos_, DONE_SWITCH_RAD, 10, 0x00ff00, 0x00ff00, true);

	// 納品場所の当たり判定の視覚化
	VECTOR startPos, endPos;
	startPos = endPos = deliveryPos_;

	startPos = VSub(startPos, deliverySize_);

	endPos = VAdd(endPos, deliverySize_);

	DrawCube3D(startPos, endPos, 0x0000ff, 0x0000ff, false);

}