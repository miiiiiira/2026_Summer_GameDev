#include "DeliveryLocationCollider.h"
#include "../../../Object.h"
#include "../../Transform/Transform.h"
#include "../../Stage/Stage.h"
#include "../../Item/Item.h"
#include "../../PlayerController/PlayerController.h"
#include "../../../../Common/Manager/Score/ScoreManager.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../Crosshair/Crosshair.h"
#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Manager/PlayerStatus/PlayerStatusManager.h"
#include "../../../../Common/Manager/System/SystemManager.h"

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

	// 納品完了スイッチとプレイヤーとの当たり判定
	DoneSwitchToPlayerCollision();
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

	// 掴めるか
	bool isGrab = true;

	// 当たっていない
	if (!Collision::HitLineSphere(lineStartPos, lineEndPos, doneSwitchPos, doneSwitchRad))
	{
		// クロスヘアの種類を掴めないに変更
		crosshair_->ChangeCrosshair(CROSSHAIR_NOT_GRAB);

		// 掴めない
		isGrab = false;
	}

	// パッドかつ掴めない判定が出ていたら
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::PAD
		&& !isGrab)
	{
		// プレイヤーの掴める距離になければ処理を行わない
		if (!Collision::HitSpherePoint(
			player_->GetTransform()->pos_,
			PlayerStatusManager::GetInstance().GetPlayerStatus().rangeMax_,
			doneSwitchPos))return;

		// 場所が視界内に入っていないため処理を行わない
		if (CheckCameraViewClip(doneSwitchPos))return;

		// ワールド座標をスクリーン座標にする
		VECTOR pos = ConvWorldPosToScreenPos(doneSwitchPos);
		Vector2 screenPos = { pos.x,pos.y };
		// スクリーン上で掴み可能な範囲
		Vector2 checkBoxPos = { Application::SCREEN_SIZE_X / 2 - SystemManager::CONTROLLER_GRAB_SCREEN_RANGE_RAD ,
			Application::SCREEN_SIZE_Y / 2 - SystemManager::CONTROLLER_GRAB_SCREEN_RANGE_RAD };

		// 掴み可能な範囲に入っている
		if (Collision::HitPoint2Box(
			screenPos,
			checkBoxPos,
			SystemManager::CONTROLLER_GRAB_SCREEN_RANGE,
			SystemManager::CONTROLLER_GRAB_SCREEN_RANGE))
		{
			// 掴める
			isGrab = true;
		}
	}

	// 掴める範囲に無いため処理を行わない
	if (!isGrab)return;

	// クリアカウントが開始されていたら処理を行わない
	if (stage_->GetStartClearCount())return;

	// 当たっている
	// クロスヘアの種類を掴めるに変更
	crosshair_->ChangeCrosshair(CROSSHAIR_CAN_GRAB);

	// 掴もうとしていたら
	if (player_->GetGrabbingState() == GRABBING_STATE::TRY_GRABBING)
	{
		// ボタンを押したことを伝える
		stage_->TrueIsDoneSwitch();

		// 納品済みの金額を確認
		int deliveryPrice = ScoreManager::GetInstance().GetDeliveryPrice();
		// 目標金額を確認
		int targetPrice = ScoreManager::GetInstance().GetTargetPrice();

		// 目標金額を達成していたら
		if (deliveryPrice >= targetPrice)
		{
			// チュートリアルだった場合
			if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)
			{
				// 確認項目が納品だったら
				if (SceneManager::GetInstance()->GetTutorialState() == Tutorial::DELIVER)
				{
					// 納品完了音
					AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_SUC);

					// クリアカウントを開始
					stage_->StartClearCount();
					return;
				}

				// 確認項目が納品出なければ納品失敗音
				AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_FAI);
			}
			// チュートリアル以外のシーンだった場合
			else
			{
				// 納品完了音
				AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_SUC);

				// クリアカウントを開始
				stage_->StartClearCount();
			}
			return;
		}
		else
		{
			// 納品失敗音
			AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_BUTTON_FAI);
		}
	}
}

void DeliveryLocationCollider::DoneSwitchToPlayerCollision(void)
{
	// 納品済みの金額を確認
	int deliveryPrice = ScoreManager::GetInstance().GetDeliveryPrice();
	// 目標金額を確認
	int targetPrice = ScoreManager::GetInstance().GetTargetPrice();

	// 目標金額を達成していなかったら
	if (deliveryPrice < targetPrice)
	{
		// プッシュ画像表示フラグを折る
		stage_->SetIsPushDrawFlg(false);
		return;
	}

	// 線分の上座標
	VECTOR lineStartPos = player_->GetLineStartPos();

	// 納品完了スイッチの座標
	VECTOR lineEndPos = stage_->GetDoneSwitchPos();
	lineEndPos.y += 30.0f;

	// カメラとスイッチを線分をつなげてステージに当たっているか
	MV1_COLL_RESULT_POLY stageHitResult =
		MV1CollCheck_Line(stage_->GetModelId(), -1, lineStartPos, lineEndPos);

	// ステージに当たっていたら
	if (stageHitResult.HitFlag)
	{
		// プッシュ画像表示フラグを折る
		stage_->SetIsPushDrawFlg(false);
		return;
	}

	// チュートリアルだった場合
	if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)
	{
		// 確認項目が納品だったら
		if (SceneManager::GetInstance()->GetTutorialState() == Tutorial::DELIVER)
		{
			// プッシュ画像を表示
			stage_->SetIsPushDrawFlg(true);
		}
	}
	// チュートリアル以外のシーンだった場合
	else
	{
		// プッシュ画像を表示
		stage_->SetIsPushDrawFlg(true);
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
