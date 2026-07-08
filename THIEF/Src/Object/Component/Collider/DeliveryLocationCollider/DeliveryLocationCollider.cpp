#include "DeliveryLocationCollider.h"
#include "../../../Object.h"
#include "../../Transform/Transform.h"
#include "../../Stage/Stage.h"
#include "../../Item/Item.h"
#include "../../PlayerController/PlayerController.h"
#include "../../../../Common/Manager/Score/ScoreManager.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../../../Common/Crosshair/Crosshair.h"
#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"

void DeliveryLocationCollider::Init(void)
{
	// ステージの確保
	stage_ = owner_->GetComponent<Stage>();
}

void DeliveryLocationCollider::Update(void)
{
	if (!stage_)return;
	if (stage_->GetItems().empty())return;

	// アイテムと納品場所の当たり判定
	ItemToDeliveryLocationCollision();

	if (!player_)return;

	// 納品完了スイッチとプレイヤー掴み機能との当たり判定
	DoneSwitchToPlayerGrabbingCollision();
}

void DeliveryLocationCollider::Draw2D(void)
{
	if (!stage_)return;
	if (!player_)return;
#ifdef _DEBUG
	DebugDraw();
#endif // _DEBUG

}

void DeliveryLocationCollider::ItemToDeliveryLocationCollision(void)
{
	// 納品場所の座標
	VECTOR deliveryPos = stage_->GetDeliveryPos();

	// 納品場所のサイズ
	VECTOR deliverySize = stage_->GetDeliverySize();

	// 判定をするアイテムのポインタ
	std::vector<Item*> items = stage_->GetItems();

	// アイテムの数分減らす
	for (Item* item : items)
	{

		// アイテムの座標
		VECTOR itemPos = item->GetTransform()->pos_;

		// アイテムのサイズ(中心から端までの半径)
		VECTOR itemSize = { item->GetInfo().collisionRadiusX_
			, item->GetInfo().collisionRadiusY_
			,item->GetInfo().collisionRadiusX_ };

		// 当たっているかつ、納品場所に入っていないフラグが立っていたら
		if (Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize)
			&& !item->GetInfo().hasTouchedDeliveryLocation_)
		{
			// アイテムが範囲内に入ったサウンド
			AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_ITEM_ON);

			// アイテム事体に納品場所にはいっていることを伝える
			item->SetHasTouchedDelivery(true);

			// そのアイテム分納品金額に足す
			ScoreManager::GetInstance().AddDeliveryPrice(item->GetInfo().price_);
		}
		// 当たっていないかつ、納品場所に入っているフラグが立っていたら
		else if (!Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize)
			&& item->GetInfo().hasTouchedDeliveryLocation_)
		{
			// アイテム事体に納品場所にはいっていないことを伝える
			item->SetHasTouchedDelivery(false);

			// そのアイテム分納品金額から引く
			ScoreManager::GetInstance().AddDeliveryPrice(-item->GetInfo().price_);
		}
	}

}

void DeliveryLocationCollider::DoneSwitchToPlayerGrabbingCollision(void)
{
	// プレイヤーが何かを掴んでいる状態だったら処理を行わない
	if (player_->GetGrabbingState() == GRABBING_STATE::IS_GRABBING)return;

	// 納品完了スイッチの座標
	VECTOR doneSwitchPos = stage_->GetDoneSwitchPos();

	// 納品完了スイッチの半径
	float doneSwitchRad = Stage::DONE_SWITCH_RAD;

	// 線分の上座標
	VECTOR lineStartPos = player_->GetLineStartPos();

	// 線分の下座標
	VECTOR lineEndPos = player_->GetLineEndPos();

	// 当たっていない
	if (!Collision::HitLineSphere(lineStartPos, lineEndPos, doneSwitchPos, doneSwitchRad))
	{
		// クロスヘアの種類を掴めないに変更
		crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::NOT_GRAB);
		return;
	}

	// 線分とステージモデルの衝突判定
	MV1_COLL_RESULT_POLY stageHitResult =
		MV1CollCheck_Line(stage_->GetModelId(), -1, lineStartPos, doneSwitchPos);

	// ステージに当たっていたら
	if (stageHitResult.HitFlag)return;

	// 当たっている
	// クロスヘアの種類を掴めるに変更
	crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::CAN_GRAB);

	// 掴もうとしていたら
	if (player_->GetGrabbingState() == GRABBING_STATE::TRY_GRABBING)
	{
		// 納品済みの金額を確認
		int deliveryPrice = ScoreManager::GetInstance().GetDeliveryPrice();
		// 目標金額を確認
		int targetPrice = ScoreManager::GetInstance().GetTargetPrice();

		// 目標金額を達成していたら
		if (deliveryPrice >= targetPrice)
		{
			// 納品完了音
			AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_SUC);

			// ステージクリアへ
			SceneManager::GetInstance()->TrueStageClear();
			return;
		}
		else
		{
			// 納品失敗音
			AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_FAI);
		}
	}
}

void DeliveryLocationCollider::DebugDraw(void)
{

	// 判定をするアイテムのポインタ
	std::vector<Item*> items = stage_->GetItems();

	// アイテムの数分減らす
	for (Item* item : items)
	{
#pragma region 納品場所

		// 納品場所の座標
		VECTOR deliveryPos = stage_->GetDeliveryPos();

		// 納品場所のサイズ
		VECTOR deliverySize = stage_->GetDeliverySize();

		// アイテムの座標
		VECTOR itemPos = item->GetTransform()->pos_;

		// アイテムのサイズ(中心から端までの半径)
		VECTOR itemSize = { item->GetInfo().collisionRadiusX_
			, item->GetInfo().collisionRadiusY_
			,item->GetInfo().collisionRadiusX_ };

		if (Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize))
		{
			DrawString(20, 400, "納品場所に入った！", 0xffffff);
		}
		else
		{
			DrawString(20, 400, "納品場所に入ってない…", 0xffffff);
		}

#pragma endregion
	}
}
