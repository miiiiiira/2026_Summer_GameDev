#include "Stage.h"

#include "../Render/Render3D.h"
#include "../Collider/DeliveryLocationCollider/DeliveryLocationCollider.h"
#include "../../../Scene/SceneManager.h"
#include "../../Object.h"
#include "DeliveryInfo.h"

Stage::~Stage(void)
{
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

	// ステージ情報を取ってきて初期化処理を行う
	auto stageNum = SceneManager::GetInstance()->GetCurrentStage();
	auto deliveryData = DeliveryTable::Table.find(stageNum);

	// 納品場所の座標
	deliveryPos_ = trans->pos_;
	deliveryPos_ = VAdd(deliveryPos_, deliveryData->second.deliveryLocalPos_);

	// 納品完了スイッチの座標
	doneSwitchPos_ = trans->pos_;
	doneSwitchPos_ = VAdd(doneSwitchPos_, deliveryData->second.doneSwitchLocalPos_);
}

void Stage::Draw3D(void)
{

	// 納品完了スイッチの当たり判定視覚化
	DrawSphere3D(doneSwitchPos_, DONE_SWITCH_RAD, 10, 0x00ff00, 0x00ff00, true);

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG
}

Transform* Stage::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

void Stage::SetItem(Item* items)
{
	items_.push_back(items);
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

void Stage::DrawDebug(void)
{
	// 納品場所の当たり判定の視覚化
	VECTOR startPos, endPos;
	startPos = endPos = deliveryPos_;

	startPos.x -= DELIVERY_SIZE_WID_RAD;
	startPos.y -= DELIVERY_SIZE_HIG_RAD;
	startPos.z -= DELIVERY_SIZE_DEPTH_RAD;

	endPos.x += DELIVERY_SIZE_WID_RAD;
	endPos.y += DELIVERY_SIZE_HIG_RAD;
	endPos.z += DELIVERY_SIZE_DEPTH_RAD;

	DrawCube3D(startPos, endPos, 0x0000ff, 0x0000ff, false);

}